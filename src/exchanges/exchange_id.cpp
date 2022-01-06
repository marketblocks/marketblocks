#include "exchange_id.h"

ExchangeIdLookup::ExchangeIdLookup()
	: _map
{
	{ "kraken", ExchangeId::KRAKEN }
}
{}