#pragma once

#include <optional>
#include <variant>

#include "websocket_stream_constants.h"
#include "trading/tradable_pair.h"

namespace mb
{
	class websocket_subscription
	{
	private:
		using parameter_variant = std::variant<std::monostate, ohlcv_interval, order_book_depth>;

		websocket_channel _channel;
		std::vector<tradable_pair> _pairs;
		parameter_variant _parameter;

		constexpr websocket_subscription(
			websocket_channel channel,
			std::vector<tradable_pair> pairs,
			parameter_variant parameter)
			:
			_channel{ channel },
			_pairs{ std::move(pairs) },
			_parameter{ std::move(parameter) }
		{}

	public:
		static constexpr websocket_subscription create_price_sub(std::vector<tradable_pair> pairs)
		{
			return websocket_subscription{ websocket_channel::PRICE, std::move(pairs), std::monostate{} };
		}

		static constexpr websocket_subscription create_order_book_sub(std::vector<tradable_pair> pairs, order_book_depth depth)
		{
			return websocket_subscription{ websocket_channel::ORDER_BOOK, std::move(pairs), depth };
		}

		static constexpr websocket_subscription create_ohclv_sub(std::vector<tradable_pair> pairs, ohlcv_interval interval)
		{
			return websocket_subscription{ websocket_channel::OHLCV, std::move(pairs), interval };
		}

		constexpr websocket_channel channel() const noexcept { return _channel; }
		constexpr const std::vector<tradable_pair>& pairs() const noexcept { return _pairs; }

		constexpr ohlcv_interval get_ohlcv_interval() const
		{
			assert(_channel == websocket_channel::OHLCV);
			return std::get<ohlcv_interval>(_parameter);
		}

		constexpr order_book_depth get_order_book_depth() const
		{
			assert(_channel == websocket_channel::ORDER_BOOK);
			return std::get<order_book_depth>(_parameter);
		}
	};
}