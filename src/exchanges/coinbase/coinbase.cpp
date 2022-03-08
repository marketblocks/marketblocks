#include "coinbase.h"
#include "coinbase_results.h"

namespace cb
{
	coinbase_api::coinbase_api(
		std::unique_ptr<http_service> httpService,
		std::unique_ptr<websocket_stream> websocketStream)
		: 
		_constants{},
		_httpService{ std::move(httpService) }, 
		_websocketStream{ std::move(websocketStream) }
	{}

	exchange_status coinbase_api::get_status() const
	{
		return exchange_status::ONLINE;
	}

	std::vector<tradable_pair> coinbase_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>(_constants.methods.PRODUCTS, internal::read_tradable_pairs);
	}

	ticker_data coinbase_api::get_ticker_data(const tradable_pair& tradablePair) const
	{
		return ticker_data{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	}

	order_book_state coinbase_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return order_book_state{ std::vector<order_book_level>{} };
	}

	double coinbase_api::get_fee(const tradable_pair& tradablePair) const
	{
		return 0.0;
	}

	std::unordered_map<asset_symbol, double> coinbase_api::get_balances() const
	{
		return std::unordered_map<asset_symbol, double>{};
	}

	std::vector<order_description> coinbase_api::get_open_orders() const
	{
		return std::vector<order_description>{};
	}

	std::vector<order_description> coinbase_api::get_closed_orders() const
	{
		return std::vector<order_description>{};
	}

	std::string coinbase_api::add_order(const trade_description& description)
	{
		return "";
	}

	void coinbase_api::cancel_order(std::string_view orderId)
	{

	}

	std::unique_ptr<exchange> make_coinbase(std::shared_ptr<websocket_client> websocketClient)
	{
		return std::make_unique<coinbase_api>(
			std::make_unique<http_service>(),
			std::make_unique<coinbase_websocket_stream>(websocketClient));
	}
}