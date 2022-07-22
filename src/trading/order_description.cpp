#include "order_description.h"

namespace mb
{
	order_description::order_description(
		std::time_t timeStamp,
		std::string orderId,
		std::string pairName,
		trade_action action,
		double price,
		double volume)
		:
		_timeStamp{ timeStamp },
		_orderId{ std::move(orderId) },
		_pairName{ std::move(pairName) },
		_action{ action },
		_price{ price },
		_volume{ volume }
	{}

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