#include "historical_trade.h"

namespace
{
	namespace csv_column_indices
	{
		static constexpr int TIME_STAMP = 0;
		static constexpr int TRADE_ACTION = 1;
		static constexpr int PRICE = 2;
		static constexpr int VOLUME = 3;
	}

	static constexpr std::string_view BUY = "buy";
	static constexpr std::string_view SELL = "sell";
}

namespace mb
{
	template<>
	historical_trade from_csv_row(const csv_row& row)
	{
		return historical_trade
		{
			std::stoll(row.get_cell(csv_column_indices::TIME_STAMP)),
			row.get_cell(csv_column_indices::TRADE_ACTION) == BUY ? trade_action::BUY : trade_action::SELL,
			std::stod(row.get_cell(csv_column_indices::PRICE)),
			std::stod(row.get_cell(csv_column_indices::VOLUME))
		};
	}

	template<>
	csv_row to_csv_row(const historical_trade& data)
	{
		return csv_row
		{
			std::vector<std::string>
			{
				std::to_string(data.time_stamp()),
				data.action() == trade_action::BUY ? BUY.data() : SELL.data(),
				std::to_string(data.asset_price()),
				std::to_string(data.asset_price())
			}
		};
	}
}