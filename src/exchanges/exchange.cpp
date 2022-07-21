#include "exchange.h"

namespace mb
{
	exchange::exchange(std::string_view id, std::shared_ptr<websocket_stream> websocketStream)
		: _id{ std::move(id) }, _websocketStream{ std::move(websocketStream) }, _websocketConnected{ false }
	{}

	std::shared_ptr<websocket_stream> exchange::get_websocket_stream()
	{
		if (!_websocketConnected)
		{
			if (_websocketStream->connection_status() == ws_connection_status::CLOSED)
			{
				_websocketStream->reset();
			}

			_websocketConnected = true;
		}

		return _websocketStream;
	}

	std::unordered_map<tradable_pair, double> market_api::get_prices(const std::vector<tradable_pair>& pairs) const
	{
		std::unordered_map<tradable_pair, double> prices;
		prices.reserve(pairs.size());

		for (auto& pair : pairs)
		{
			prices.emplace(pair, get_price(pair));
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		return prices;
	}
}