#include "exchange_id.h"

namespace cb
{
	exchange_id_lookup::exchange_id_lookup()
		: _container
	{
		{ "kraken", exchange_id::KRAKEN }
	}
	{}
}
