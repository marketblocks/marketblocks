#include "digifinex.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"
#include "common/exceptions/not_implemented_exception.h"
#include "common/file/config_file_reader.h"

namespace
{
	using namespace mb;

	std::string get_ohlcv_period(ohlcv_interval interval)
	{
		if (interval == ohlcv_interval::D1)
		{
			return "1D";
		}
		else if (interval == ohlcv_interval::W1)
		{
			return "1W";
		}
		else
		{
			return std::to_string(to_seconds(interval) / 60);
		}
	}

	constexpr std::string_view get_order_type(order_type orderType, trade_action action)
	{
		if (orderType == order_type::LIMIT)
		{
			return action == trade_action::BUY ? "buy" : "sell";
		}
		else if (orderType == order_type::MARKET)
		{
			return action == trade_action::BUY ? "buy_market" : "sell_market";
		}

		throw mb_exception{ "Order type not supported" };
	}

	template<typename GetPrice>
	double get_amount(const order_request& orderRequest, GetPrice getPrice)
	{
		double volume{ orderRequest.get(order_request_parameter::VOLUME) };

		if (orderRequest.order_type() == order_type::MARKET)
		{
			return calculate_cost(getPrice(), volume);
		}

		return volume;
	}
}

namespace mb
{
	digifinex_api::digifinex_api(
		digifinex_config config,
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream,
		bool enableTesting)
		: 
		exchange{ exchange_ids::DIGIFINEX, std::move(websocketStream) },
		_apiKey{ config.api_key() },
		_apiSecret{ config.api_secret() },
		_httpService{ std::move(httpService) }
	{}

	std::string digifinex_api::compute_api_sign(std::string_view query) const
	{
		std::vector<unsigned char> signData{ hmac_sha256(query, _apiSecret) };
		return hex_encode(signData);
	}

	exchange_status digifinex_api::get_status() const
	{
		return send_public_request<exchange_status>("/ping", digifinex::read_system_status);
	}

	std::vector<tradable_pair> digifinex_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>("/spot/symbols", digifinex::read_tradable_pairs);
	}

	std::vector<ohlcv_data> digifinex_api::get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string(_pairSeparator))
			.add_parameter("period", get_ohlcv_period(interval))
			.to_string();

		return send_public_request<std::vector<ohlcv_data>>(
			"/kline", 
			[count](std::string_view result) { return digifinex::read_ohlcv_data(result, count); },
			query);
	}

	double digifinex_api::get_price(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string(_pairSeparator))
			.to_string();

		return send_public_request<double>("/ticker", digifinex::read_price, query);
	}

	order_book_state digifinex_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string('_'))
			.add_parameter("limit", std::to_string(depth))
			.to_string();

		return send_public_request<order_book_state>("order_book", digifinex::read_order_book, query);
	}

	double digifinex_api::get_fee(const tradable_pair& tradablePair) const
	{
		return 0.2;
	}

	std::unordered_map<std::string,double> digifinex_api::get_balances() const
	{
		return send_private_request<std::unordered_map<std::string,double>>(http_verb::GET, "spot/assets", digifinex::read_balances);
	}

	std::vector<order_description> digifinex_api::get_open_orders() const
	{
		return send_private_request<std::vector<order_description>>(http_verb::GET, "spot/order/current", digifinex::read_open_orders);
	}

	std::vector<order_description> digifinex_api::get_closed_orders() const
	{
		return send_private_request<std::vector<order_description>>(http_verb::GET, "spot/order/history", digifinex::read_closed_orders);
	}

	std::string digifinex_api::add_order(const order_request& description)
	{
		std::string query = url_query_builder{}
			.add_parameter("market", "spot")
			.add_parameter("symbol", description.pair().to_string('_'))
			.add_parameter("type", get_order_type(description.order_type(), description.action()))
			.add_parameter("amount", std::to_string(get_amount(description, [this, &description]() { return get_price(description.pair()); })))
			.add_parameter("price", std::to_string(description.get(order_request_parameter::ASSET_PRICE)))
			.to_string();				

		return send_private_request<std::string>(http_verb::POST, "spot/order/new", digifinex::read_add_order, query);
	}

	void digifinex_api::cancel_order(std::string_view orderId)
	{
		std::string query = url_query_builder{}
			.add_parameter("market", "spot")
			.add_parameter("order_id", orderId)
			.to_string();

		return send_private_request<void>(http_verb::POST, "spot/order/cancel", digifinex::read_cancel_order, query);
	}

	template<>
	std::unique_ptr<websocket_stream> create_exchange_websocket_stream<internal::digifinex_websocket_stream>()
	{
		std::unique_ptr<digifinex_api> marketApi{ std::make_unique<digifinex_api>(
			digifinex_config{},
			std::make_unique<http_service>(),
			nullptr,
			false) };

		return std::make_unique<internal::digifinex_websocket_stream>(
			std::make_unique<websocket_connection_factory>(),
			std::move(marketApi));
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<digifinex_api>(bool testing)
	{
		return std::make_unique<digifinex_api>(
			internal::load_or_create_config<digifinex_config>(),
			std::make_unique<http_service>(),
			create_exchange_websocket_stream<internal::digifinex_websocket_stream>(),
			testing);
	}
}