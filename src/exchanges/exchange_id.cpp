#include "exchange_id.h"

ExchangeIdLookup::ExchangeIdLookup()
	: _container
{
	{ "kraken", ExchangeId::KRAKEN }
}
{}