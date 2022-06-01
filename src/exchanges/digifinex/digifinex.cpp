#include "digifinex.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"
#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	digifinex_api::digifinex_api(
		digifinex_config config,
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
		: 
		_constants{},
		_apiKey{ config.api_key() },
		_apiSecret{ config.api_secret() },
		_httpService{ std::move(httpService) },
		_websocketStream{ websocketStream }
	{}

	std::string digifinex_api::compute_api_sign(std::string_view query) const
	{
		std::vector<unsigned char> signData{ hmac_sha256(query, _apiSecret) };
		return hex_encode(signData);
	}

	exchange_status digifinex_api::get_status() const
	{
		throw not_implemented_exception{ "digifinex::get_status" };
	}

	std::vector<tradable_pair> digifinex_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>(_constants.methods.SYMBOLS, digifinex::read_tradable_pairs);
	}

	ohlcv_data digifinex_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		throw not_implemented_exception{ "digifinex::get_24h_stats" };
	}

	double digifinex_api::get_price(const tradable_pair& tradablePair) const
	{
		throw not_implemented_exception{ "digifinex::get_price" };
	}

	order_book_state digifinex_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		throw not_implemented_exception{ "digifinex::get_order_book" };
	}

	double digifinex_api::get_fee(const tradable_pair& tradablePair) const
	{
		return 0.2;
	}

	unordered_string_map<double> digifinex_api::get_balances() const
	{
		return send_private_request<unordered_string_map<double>>(_constants.methods.ASSETS, digifinex::read_balances);
	}

	std::vector<order_description> digifinex_api::get_open_orders() const
	{
		throw not_implemented_exception{ "digifinex::get_open_orders" };
	}

	std::vector<order_description> digifinex_api::get_closed_orders() const
	{
		throw not_implemented_exception{ "digifinex::get_closed_orders" };
	}

	std::string digifinex_api::add_order(const trade_description& description)
	{
		throw not_implemented_exception{ "digifinex::add_order" };
	}

	void digifinex_api::cancel_order(std::string_view orderId)
	{
		throw not_implemented_exception{ "digifinex::cancel_order" };
	}

	std::unique_ptr<exchange> make_digifinex(digifinex_config config, std::shared_ptr<websocket_client> websocketClient)
	{
		return std::make_unique<digifinex_api>(
			std::move(config),
			std::make_unique<http_service>(),
			nullptr);
	}
}