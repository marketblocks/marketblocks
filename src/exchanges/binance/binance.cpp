#include "binance.h"
#include "binance_results.h"
#include "exchanges/exchange_ids.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"
#include "common/file/config_file_reader.h"
#include "common/utils/containerutils.h"
#include "common/utils/mathutils.h"

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
		case order_type::STOP_LOSS:
		case order_type::TRAILING_STOP_LOSS:
			return "STOP_LOSS_LIMIT";
		default:
			throw mb_exception{ "Order type not supported" };
		}
	}

	std::string create_symbols_list(const std::vector<tradable_pair>& pairs)
	{
		std::string symbols{ "[" };

		for (auto& pair : pairs)
		{
			symbols.append("\"" + pair.to_string() + "\"" + ",");
		}

		symbols.pop_back();
		symbols.append("]");

		return symbols;
	}

	bool is_limit_order(order_type orderType)
	{
		return orderType == order_type::LIMIT ||
			orderType == order_type::STOP_LOSS ||
			orderType == order_type::TRAILING_STOP_LOSS;
	}

	url_query_builder create_order_query(const order_request& request, const internal::binance_order_filters& filters)
	{
		url_query_builder query = url_query_builder{}
			.add_parameter("symbol", request.pair().to_string())
			.add_parameter("side", to_order_side(request.action()))
			.add_parameter("type", to_order_type_str(request.order_type()))
			.add_parameter("quantity", to_string(request.get(order_request_parameter::VOLUME), filters.qty_precision()));

		if (is_limit_order(request.order_type()))
		{
			query.add_parameter("price", to_string(request.get(order_request_parameter::ASSET_PRICE), filters.price_precision()));
			query.add_parameter("timeInForce", "GTC");
		}

		switch (request.order_type())
		{
		case order_type::STOP_LOSS:
		{
			query.add_parameter("stopPrice", to_string(request.get(order_request_parameter::STOP_PRICE), filters.price_precision()));
			break;
		}
		case order_type::TRAILING_STOP_LOSS:
		{
			int delta{ static_cast<int>(request.get(order_request_parameter::TRAILING_DELTA) * 10000) };
			query.add_parameter("trailingDelta", std::to_string(delta));
			break;
		}
		default:
			break;
		}

		return query;
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
		_orderFilters = send_public_request<std::unordered_map<tradable_pair, internal::binance_order_filters>>("/api/v3/exchangeInfo", binance::read_tradable_pairs);

		std::vector<tradable_pair> pairs;
		pairs.reserve(_orderFilters.size());

		double minValue = 0.0;

		for (auto& [pair, filter] : _orderFilters)
		{
			pairs.emplace_back(pair);

			if (pair.price_unit() == "USDT")
			{
				minValue = std::max(minValue, filter.min_value());
			}
		}

		return pairs;
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
		std::string query = url_query_builder{}
			.add_parameter("symbols", create_symbols_list(pairs))
			.to_string();

		std::unordered_map<std::string, double> namedPrices{ send_public_request<std::unordered_map<std::string, double>>("/api/v3/ticker/price", binance::read_prices, query) };
		return internal::create_pair_result_map(pairs, namedPrices);
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

	std::string binance_api::add_order(const order_request& request)
	{
		url_query_builder query{ create_order_query(request, _orderFilters[request.pair()])};
		return send_private_request<std::string>(http_verb::POST, "/api/v3/order", binance::read_add_order, query);
	}

	order_confirmation binance_api::add_order_confirm(const order_request& request)
	{
		url_query_builder query{ create_order_query(request, _orderFilters[request.pair()]) };
		return send_private_request<order_confirmation>(http_verb::POST, "/api/v3/order", binance::read_add_order_confirm, query);
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
	std::unique_ptr<websocket_stream> create_exchange_websocket_stream<internal::binance_websocket_stream>()
	{
		std::unique_ptr<binance_api> marketApi{ std::make_unique<binance_api>(
			binance_config{},
			std::make_unique<http_service>(),
			nullptr,
			false) };

		return std::make_unique<internal::binance_websocket_stream>(
			std::make_unique<websocket_connection_factory>(),
			std::move(marketApi));
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