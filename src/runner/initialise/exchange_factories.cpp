#include "exchange_factories.h"

#include "config_file_readers.h"
#include "exchanges/kraken/kraken.h"

std::unique_ptr<Exchange> make_kraken()
{
	KrakenConfig config = load_kraken_config();

	return std::make_unique<KrakenApi>(std::move(config), HttpService{});;
}