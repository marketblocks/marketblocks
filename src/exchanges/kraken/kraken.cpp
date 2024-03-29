#include <chrono>

#include "kraken.h"
#include "kraken_results.h"
#include "kraken_websocket.h"
#include "networking/http/http_constants.h"
#include "common/file/config_file_reader.h"
#include "common/types/result.h"
#include "common/utils/stringutils.h"
#include "common/utils/containerutils.h"
#include "common/utils/timeutils.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

namespace
{
	using namespace mb;

	constexpr std::string_view to_string(const order_type& orderType)
	{
		switch (orderType)
		{
		case order_type::LIMIT:
			return "limit";
		case order_type::MARKET:
			return "market";
		case order_type::STOP_LOSS:
			return "stop-loss";
		case order_type::TAKE_PROFIT:
			return "take-profit";
		default:
			throw std::invalid_argument{ "Order Type not recognized" };
		}
	}

	constexpr std::string_view to_string(const trade_action& tradeAction)
	{
		return tradeAction == trade_action::BUY ? "buy" : "sell";
	}

	std::string create_pair_list(const std::vector<tradable_pair>& pairs)
	{
		std::string pairList;

		for (auto& pair : pairs)
		{
			pairList.append(pair.to_string());
			pairList.append(",");
		}

		pairList.pop_back();
		return pairList;
	}
}

namespace mb
{
	kraken_api::kraken_api(
		kraken_config config, 
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream,
		bool enableTesting)
		:
		exchange{ exchange_ids::KRAKEN, std::move(websocketStream) },
		_publicKey{ config.public_key() },
		_decodedPrivateKey{ b64_decode(config.private_key()) },
		_httpService{ std::move(httpService) }
	{}

	std::string kraken_api::get_nonce() const
	{
		return std::to_string(time_since_epoch<std::chrono::milliseconds>());
	}

	std::string kraken_api::compute_api_sign(std::string_view uriPath, std::string_view urlPostData, std::string_view nonce) const
	{
		std::vector<unsigned char> nonce_postData = sha256(std::string{ nonce }.append(urlPostData));

		std::vector<unsigned char> message{ uriPath.begin(), uriPath.end() };
		message.insert(message.end(), nonce_postData.begin(), nonce_postData.end());

		return b64_encode(hmac_sha512(message, _decodedPrivateKey));
	}

	std::string kraken_api::build_kraken_path(std::string access, std::string method) const
	{
		return "/0/" + std::move(access) + "/" + std::move(method);
	}

	exchange_status kraken_api::get_status() const
	{
		return send_public_request<exchange_status>("SystemStatus", kraken::read_system_status);
	}

	std::vector<tradable_pair> kraken_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>("AssetPairs", kraken::read_tradable_pairs);
	}

	std::vector<ohlcv_data> kraken_api::get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.add_parameter("interval", std::to_string(to_seconds(interval) / 60))
			.to_string();

		return send_public_request<std::vector<ohlcv_data>>(
			"OHLC", 
			[count](std::string_view result) { return kraken::read_ohlcv_data(result, count); },
			query);
	}

	double kraken_api::get_price(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.to_string();

		return send_public_request<double>("Ticker", kraken::read_price, query);
	}

	std::unordered_map<tradable_pair, double> kraken_api::get_prices(const std::vector<tradable_pair>& pairs) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", create_pair_list(pairs))
			.to_string();

		std::unordered_map<std::string, double> namedPrices{ send_public_request<std::unordered_map<std::string, double>>("Ticker", kraken::read_prices, query) };
		return internal::create_pair_result_map(pairs, namedPrices);
	}

	order_book_state kraken_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.add_parameter("count", std::to_string(depth))
			.to_string();

		return send_public_request<order_book_state>("Depth", kraken::read_order_book, query);
	}

	double kraken_api::get_fee(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.to_string();

		return send_private_request<double>("TradeVolume", kraken::read_fee, query);
	}

	std::unordered_map<std::string,double> kraken_api::get_balances() const
	{
		return send_private_request<std::unordered_map<std::string,double>>("Balance", kraken::read_balances);
	}

	std::vector<order_description> kraken_api::get_open_orders() const
	{
		return send_private_request<std::vector<order_description>>("OpenOrders", kraken::read_open_orders);
	}

	std::vector<order_description> kraken_api::get_closed_orders() const
	{
		return send_private_request<std::vector<order_description>>("ClosedOrders", kraken::read_closed_orders);
	}

	std::string kraken_api::add_order(const order_request& description)
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", description.pair().to_string())
			.add_parameter("type", ::to_string(description.action()))
			.add_parameter("ordertype", ::to_string(description.order_type()))
			.add_parameter("price", std::to_string(description.get(order_request_parameter::ASSET_PRICE)))
			.add_parameter("volume", std::to_string(description.get(order_request_parameter::VOLUME)))
			.to_string();

		return send_private_request<std::string>("AddOrder", kraken::read_add_order, query);
	}

	void kraken_api::cancel_order(std::string_view orderId)
	{
		std::string query = url_query_builder{}
			.add_parameter("txid", orderId)
			.to_string();

		send_private_request<void>("CancelOrder", kraken::read_cancel_order, query);
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<kraken_api>(bool testing)
	{
		return std::make_unique<kraken_api>(
			internal::load_or_create_config<kraken_config>(),
			std::make_unique<http_service>(),
			create_exchange_websocket_stream<internal::kraken_websocket_stream>(),
			testing);
	}
}