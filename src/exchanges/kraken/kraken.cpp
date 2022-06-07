#include <chrono>

#include "kraken.h"
#include "kraken_results.h"
#include "kraken_websocket.h"
#include "networking/http/http_constants.h"
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
}

namespace mb
{
	kraken_api::kraken_api(
		kraken_config config, 
		std::unique_ptr<http_service> httpService,
		std::unique_ptr<websocket_stream> websocketStream)
		:
		_publicKey{ config.public_key() },
		_decodedPrivateKey{ b64_decode(config.private_key()) },
		_httpService{ std::move(httpService) },
		_websocketStream{ std::move(websocketStream) }
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

	ohlcv_data kraken_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.to_string();

		return send_public_request<ohlcv_data>("Ticker", kraken::read_24h_stats, query);
	}

	double kraken_api::get_price(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.to_string();

		return send_public_request<double>("Ticker", kraken::read_price, query);
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

	unordered_string_map<double> kraken_api::get_balances() const
	{
		return send_private_request<unordered_string_map<double>>("Balance", kraken::read_balances);
	}

	std::vector<order_description> kraken_api::get_open_orders() const
	{
		return send_private_request<std::vector<order_description>>("OpenOrders", kraken::read_open_orders);
	}

	std::vector<order_description> kraken_api::get_closed_orders() const
	{
		return send_private_request<std::vector<order_description>>("ClosedOrders", kraken::read_closed_orders);
	}

	std::string kraken_api::add_order(const trade_description& description)
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", description.pair().to_string())
			.add_parameter("type", ::to_string(description.action()))
			.add_parameter("ordertype", ::to_string(description.order_type()))
			.add_parameter("price", std::to_string(description.asset_price()))
			.add_parameter("volume", std::to_string(description.volume()))
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

	std::unique_ptr<exchange> make_kraken(kraken_config config, std::shared_ptr<websocket_client> websocketClient)
	{
		return std::make_unique<kraken_api>(
			std::move(config), 
			std::make_unique<http_service>(),
			std::make_unique<exchange_websocket_stream>(std::make_unique<internal::kraken_websocket_stream>(), websocketClient));
	}
}