#pragma once

#include <optional>
#include <variant>

#include "websocket_stream_constants.h"
#include "trading/tradable_pair.h"

namespace mb
{
	template<typename TradablePairItem>
	class basic_websocket_subscription
	{
	private:
		using parameter_variant = std::variant<std::monostate, ohlcv_interval>;

		websocket_channel _channel;
		TradablePairItem _pairItem;
		parameter_variant _parameter;

		constexpr basic_websocket_subscription(
			websocket_channel channel,
			TradablePairItem pairItem,
			parameter_variant parameter = {})
			:
			_channel{ channel },
			_pairItem{ std::move(pairItem) },
			_parameter{ std::move(parameter) }
		{}

	public:
		static constexpr basic_websocket_subscription<TradablePairItem> create_trade_sub(TradablePairItem pairItem)
		{
			return basic_websocket_subscription<TradablePairItem>{ websocket_channel::TRADE, std::move(pairItem) };
		}

		static constexpr basic_websocket_subscription<TradablePairItem> create_order_book_sub(TradablePairItem pairItem)
		{
			return basic_websocket_subscription<TradablePairItem>{ websocket_channel::ORDER_BOOK, std::move(pairItem) };
		}

		static constexpr basic_websocket_subscription<TradablePairItem> create_ohlcv_sub(TradablePairItem pairItem, ohlcv_interval interval)
		{
			return basic_websocket_subscription<TradablePairItem>{ websocket_channel::OHLCV, std::move(pairItem), interval };
		}

		constexpr websocket_channel channel() const noexcept { return _channel; }
		constexpr const TradablePairItem& pair_item() const noexcept { return _pairItem; }

		constexpr ohlcv_interval get_ohlcv_interval() const
		{
			assert(_channel == websocket_channel::OHLCV);
			return std::get<ohlcv_interval>(_parameter);
		}
	};

	using websocket_subscription = basic_websocket_subscription<std::vector<tradable_pair>>;
	using unique_websocket_subscription = basic_websocket_subscription<tradable_pair>;
}