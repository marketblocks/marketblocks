#include "exchange_id.h"

namespace cb
{
	exchange_id_lookup::exchange_id_lookup()
		: _container
	{
		{ "kraken", exchange_id::KRAKEN }
	}
	{}

	std::vector<std::string> exchange_id_lookup::all_ids() const
	{
		std::vector<std::string> ids;
		ids.reserve(_container.size());

		for (auto& pair : _container)
		{
			ids.push_back(pair.first);
		}

		return ids;
	}
}
