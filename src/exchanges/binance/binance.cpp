#include "binance.h"
#include "binance_results.h"
#include "exchanges/exchange_ids.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"
#include "common/file/config_file_reader.h"
#include "common/utils/containerutils.h"

#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

	constexpr std::string_view select_base_url(bool enableTesting)
	{
		constexpr std::string_view LIVE_BASE_URL = "https://api.binance.com";
		constexpr std::string_view TEST_BASE_URL = "https://testnet.binance.vision";

		if (enableTesting)
		{
			return TEST_BASE_URL;
		}

		return LIVE_BASE_URL;
	}

	std::string to_order_side(trade_action action)
	{
		return action == trade_action::BUY
			? "BUY"
			: "SELL";
	}

	std::string to_order_type_str(order_type orderType)
	{
		switch (orderType)
		{
		case order_type::LIMIT:
			return "LIMIT";
		case order_type::MARKET:
			return "MARKET";
		default:
			throw mb_exception{ "Order type not supported" };
		}
	}
}

namespace mb
{
	binance_api::binance_api(
		binance_config config,
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream,
		bool enableTesting)
		: 
		exchange{ exchange_ids::BINANCE, websocketStream },
		_baseUrl{ select_base_url(enableTesting) },
		_apiKey{ std::move(config.api_key()) },
		_secretKey{ std::move(config.secret_key()) },
		_httpService{ std::move(httpService) }
	{}

	std::string binance_api::compute_api_sign(std::string query) const
	{
		return hex_encode(hmac_sha256(query, _secretKey));
	}

	exchange_status binance_api::get_status() const
	{
		return send_public_request<exchange_status>("/api/v3/time", binance::read_system_status);
	}

	std::vector<tradable_pair> binance_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>("/api/v3/exchangeInfo", binance::read_tradable_pairs);
	}

	std::vector<ohlcv_data> binance_api::get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string())
			.add_parameter("interval", to_string(interval))
			.add_parameter("limit", std::to_string(count))
			.to_string();

		return send_public_request<std::vector<ohlcv_data>>("/api/v3/klines", binance::read_ohlcv, query);
	}

	double binance_api::get_price(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string())
			.to_string();

		return send_public_request<double>("/api/v3/ticker/price", binance::read_price, query);
	}

	std::unordered_map<tradable_pair, double> binance_api::get_prices(const std::vector<tradable_pair>& pairs) const
	{
		std::string symbols{ "[" };

		for (auto& pair : pairs)
		{
			symbols.append("\"" + pair.to_string() + "\"" + ",");
		}

		symbols.pop_back();
		symbols.append("]");

		std::string query = url_query_builder{}
			.add_parameter("symbols", std::move(symbols))
			.to_string();

		std::unordered_map<std::string, tradable_pair> pairLookup{ to_unordered_map<std::string, tradable_pair>(
			pairs,
			[](const tradable_pair& pair) { return pair.to_string(); },
			[](const tradable_pair& pair) { return pair; }) };

		std::unordered_map<std::string, double> namedPrices{ send_public_request<std::unordered_map<std::string, double>>("/api/v3/ticker/price", binance::read_prices, query) };

		std::unordered_map<tradable_pair, double> prices;
		prices.reserve(namedPrices.size());

		for (auto& [pairName, price] : namedPrices)
		{
			prices.emplace(pairLookup.at(pairName), price);
		}

		return prices;
	}

	order_book_state binance_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string())
			.add_parameter("limit", std::to_string(depth))
			.to_string();

		return send_public_request<order_book_state>("/api/v3/depth", binance::read_order_book, query);
	}

	double binance_api::get_fee(const tradable_pair& tradablePair) const
	{
		return send_private_request<double>(http_verb::GET, "/api/v3/account", binance::read_fee);
	}

	std::unordered_map<std::string, double> binance_api::get_balances() const
	{
		return send_private_request<std::unordered_map<std::string, double>>(http_verb::GET, "/api/v3/account", binance::read_balances);
	}

	std::vector<order_description> binance_api::get_open_orders() const
	{
		return send_private_request<std::vector<order_description>>(http_verb::GET, "/api/v3/openOrders", binance::read_open_orders);
	}

	std::vector<order_description> binance_api::get_closed_orders() const
	{
		throw not_implemented_exception{ "binance::get_closed_orders" };
	}

	std::string binance_api::add_order(const order_request& description)
	{
		url_query_builder query = url_query_builder{}
			.add_parameter("symbol", description.pair().to_string())
			.add_parameter("side", to_order_side(description.action()))
			.add_parameter("type", to_order_type_str(description.order_type()))
			.add_parameter("quantity", std::to_string(description.volume()));

		if (description.order_type() != order_type::MARKET)
		{
			query.add_parameter("price", std::to_string(description.asset_price()));
			query.add_parameter("timeInForce", "GTC");
		}

		return send_private_request<std::string>(http_verb::POST, "/api/v3/order", binance::read_add_order, query);
	}

	void binance_api::cancel_order(std::string_view orderId)
	{
		std::vector<order_description> openOrders{ get_open_orders() };

		std::string pair;
		for (auto& order : openOrders)
		{
			if (order.order_id() == orderId)
			{
				pair = order.pair_name();
			}
		}

		if (pair.empty())
		{
			return;
		}

		url_query_builder query = url_query_builder{}
			.add_parameter("symbol", std::move(pair))
			.add_parameter("orderId", std::string{ orderId });

		send_private_request<void>(http_verb::HTTP_DELETE, "/api/v3/order", binance::read_cancel_order, query);
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<binance_api>(bool testing)
	{
		return std::make_unique<binance_api>(
			internal::load_or_create_config<binance_config>(),
			std::make_unique<http_service>(),
			create_exchange_websocket_stream<internal::binance_websocket_stream>(),
			testing);
	}
}