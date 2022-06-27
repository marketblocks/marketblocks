#include "bybit.h"
#include "bybit_websocket.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

namespace
{
	using namespace mb;

	constexpr std::string_view select_base_url(bool enableTesting)
	{
		constexpr std::string_view LIVE_BASE_URL = "https://api.bybit.com";
		constexpr std::string_view TEST_BASE_URL = "https://api-testnet.bybit.com";

		if (enableTesting)
		{
			return TEST_BASE_URL;
		}
		return LIVE_BASE_URL;
	}

	constexpr double get_order_quantity(const trade_description& tradeDescription)
	{
		if (tradeDescription.order_type() == order_type::MARKET && tradeDescription.action() == trade_action::BUY)
		{
			return calculate_cost(tradeDescription.asset_price(), tradeDescription.volume());
		}

		return tradeDescription.volume();
	}

	constexpr std::string to_side_string(trade_action action)
	{
		return action == trade_action::BUY
			? "Buy" : "Sell";
	}

	constexpr std::string to_type_string(order_type orderType)
	{
		return orderType == order_type::MARKET
			? "MARKET" : "LIMIT";
	}
}

namespace mb
{
	bybit_api::bybit_api(
		bybit_config config,
		std::unique_ptr<http_service> httpService,
		bool enableTesting)
		: 
		_baseUrl{ select_base_url(enableTesting) },
		_apiKey{ config.api_key() },
		_apiSecret{ config.api_secret() },
		_httpService{ std::move(httpService) }
	{}

	std::shared_ptr<websocket_stream> bybit_api::get_websocket_stream()
	{
		if (!_websocketStream)
		{
			_websocketStream = create_exchange_websocket_stream<internal::bybit_websocket_stream>();
		}

		return _websocketStream;
	}

	std::string bybit_api::get_time_stamp() const
	{
		return std::to_string(time_since_epoch<std::chrono::milliseconds>());
	}

	std::string bybit_api::compute_api_sign(std::string_view query) const
	{
		std::vector<unsigned char> signData{ hmac_sha256(query, _apiSecret) };
		return hex_encode(signData);
	}

	exchange_status bybit_api::get_status() const
	{
		return send_public_request<exchange_status>("spot/v1/time", bybit::read_system_status);
	}

	std::vector<tradable_pair> bybit_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>("spot/v1/symbols", bybit::read_tradable_pairs);
	}

	std::vector<ohlcv_data> bybit_api::get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string())
			.add_parameter("interval", to_string(interval))
			.add_parameter("limit", std::to_string(count))
			.to_string();

		return send_public_request<std::vector<ohlcv_data>>("/spot/quote/v1/kline", bybit::read_ohlcv, query);
	}

	double bybit_api::get_price(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string())
			.to_string();

		return send_public_request<double>("/spot/quote/v1/ticker/price", bybit::read_price, query);
	}

	order_book_state bybit_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		std::string query = url_query_builder{}
			.add_parameter("symbol", tradablePair.to_string())
			.add_parameter("limit", std::to_string(depth))
			.to_string();

		return send_public_request<order_book_state>("/spot/quote/v1/depth", bybit::read_order_book, query);
	}

	double bybit_api::get_fee(const tradable_pair& tradablePair) const
	{
		return 0.1;
	}

	unordered_string_map<double> bybit_api::get_balances() const
	{
		return send_private_request<unordered_string_map<double>>(http_verb::GET, "spot/v1/account", bybit::read_balances);
	}

	std::vector<order_description> bybit_api::get_open_orders() const
	{
		return send_private_request<std::vector<order_description>>(http_verb::GET, "/spot/v1/open-orders", bybit::read_open_orders);
	}

	std::vector<order_description> bybit_api::get_closed_orders() const
	{
		return send_private_request<std::vector<order_description>>(http_verb::GET, "/spot/v1/history-orders", bybit::read_closed_orders);
	}

	std::string bybit_api::add_order(const trade_description& description)
	{
		std::map<std::string, std::string> queryParams
		{
			{ "symbol", description.pair().to_string() },
			{ "qty", std::to_string(get_order_quantity(description)) },
			{ "side", to_side_string(description.action()) },
			{ "type", to_type_string(description.order_type()) },
			{ "price", std::to_string(description.asset_price()) }
		};
				
		return send_private_request<std::string>(http_verb::POST, "spot/v1/order", bybit::read_add_order, queryParams);
	}

	void bybit_api::cancel_order(std::string_view orderId)
	{
		std::map<std::string, std::string> queryParams
		{
			{ "orderId", std::string{ orderId } }
		};

		send_private_request<void>(http_verb::HTTP_DELETE, "spot/v1/order", bybit::read_cancel_order, queryParams);
	}

	std::unique_ptr<exchange> make_bybit(bybit_config config, bool enableTesting)
	{
		return std::make_unique<bybit_api>(
			std::move(config),
			std::make_unique<http_service>(),
			enableTesting);
	}
}