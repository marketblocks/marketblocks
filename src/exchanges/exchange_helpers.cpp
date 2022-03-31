#include "exchange_helpers.h"
#include "common/utils/containerutils.h"

namespace cb
{
	double get_balance(std::shared_ptr<exchange> exchange, std::string_view tickerId)
	{
		unordered_string_map<double> balances{ exchange->get_balances() };

		auto it = balances.find(tickerId);
		if (it != balances.end())
		{
			return it->second;
		}

		return 0.0;
	}
}