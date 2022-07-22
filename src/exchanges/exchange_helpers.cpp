#include "exchange_helpers.h"
#include "common/utils/containerutils.h"

namespace mb
{
	double get_balance(std::shared_ptr<exchange> exchange, std::string_view tickerId)
	{
		std::unordered_map<std::string,double> balances{ exchange->get_balances() };

		auto it = balances.find(tickerId.data());
		if (it != balances.end())
		{
			return it->second;
		}

		return 0.0;
	}
}