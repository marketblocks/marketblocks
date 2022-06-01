#include "bybit.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

namespace mb
{
	bybit_api::bybit_api(
		bybit_config config,
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream,
		bool enableTesting)
		: 
		_constants{ enableTesting },
		_apiKey{ config.api_key() },
		_apiSecret{ config.api_secret() },
		_httpService{ std::move(httpService) },
		_websocketStream{ websocketStream }
	{}

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
		return mb::bybit::read_system_status("").value();
	}

	std::vector<tradable_pair> bybit_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>(_constants.methods.SYMBOLS, bybit::read_tradable_pairs);
	}

	ohlcv_data bybit_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		return mb::bybit::read_24h_stats("").value();
	}

	double bybit_api::get_price(const tradable_pair& tradablePair) const
	{
		return mb::bybit::read_price("").value();
	}

	order_book_state bybit_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return mb::bybit::read_order_book("").value();
	}

	double bybit_api::get_fee(const tradable_pair& tradablePair) const
	{
		return mb::bybit::read_fee("").value();
	}

	unordered_string_map<double> bybit_api::get_balances() const
	{
		return send_private_request<unordered_string_map<double>>(_constants.methods.BALANCE, bybit::read_balances);
	}

	std::vector<order_description> bybit_api::get_open_orders() const
	{
		return mb::bybit::read_open_orders("").value();
	}

	std::vector<order_description> bybit_api::get_closed_orders() const
	{
		return mb::bybit::read_closed_orders("").value();
	}

	std::string bybit_api::add_order(const trade_description& description)
	{
		return mb::bybit::read_add_order("").value();
	}

	void bybit_api::cancel_order(std::string_view orderId)
	{
		mb::bybit::read_cancel_order("").value();
	}

	std::unique_ptr<exchange> make_bybit(bybit_config config, std::shared_ptr<websocket_client> websocketClient, bool enableTesting)
	{
		return std::make_unique<bybit_api>(
			std::move(config),
			std::make_unique<http_service>(),
			nullptr,
			enableTesting);
	}
}