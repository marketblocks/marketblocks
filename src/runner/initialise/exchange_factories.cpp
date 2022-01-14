#include "exchange_factories.h"

#include "config_file_readers.h"
#include "exchanges/kraken/kraken.h"
#include "networking/websocket/websocket_connection.h"

namespace cb
{
	std::unique_ptr<exchange> make_kraken(std::shared_ptr<websocket_client> websocketClient)
	{
		kraken_config config = load_kraken_config();
		return std::make_unique<kraken_api>(std::move(config), http_service{}, websocketClient);
	}
}