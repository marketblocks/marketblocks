#include "order_description.h"

namespace mb
{
	template<>
	csv_row to_csv_row(const order_description& data)
	{
		return csv_row
		{ std::vector<std::string>{
			std::to_string(data.time_stamp()),
			data.order_id(),
			data.pair_name(),
			std::string{ to_string(data.action()) },
			std::to_string(data.price()),
			std::to_string(data.volume())
		}};
	}
}