#pragma once

#include <memory>

#include "exchanges/exchange.h"
#include "networking/websocket/websocket_client.h"

namespace cb
{
	std::unique_ptr<exchange> make_kraken(std::shared_ptr<websocket_client> websocketClient);
}