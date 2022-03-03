#include "exchange_id.h"

#include "common/utils/containerutils.h"

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
		return to_vector<std::string>(_container, [](const std::pair<std::string, exchange_id>& pair) { return pair.first; });
	}
}
