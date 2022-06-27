#include "ohlcv_data.h"

namespace mb
{
	template<>
	ohlcv_data from_csv_row(const csv_row& row)
	{
		return ohlcv_data
		{
			std::time_t{ std::stoll(row.get_cell(0)) },
			std::stod(row.get_cell(1)),
			std::stod(row.get_cell(2)),
			std::stod(row.get_cell(3)),
			std::stod(row.get_cell(4)),
			std::stod(row.get_cell(5))
		};
	}
}