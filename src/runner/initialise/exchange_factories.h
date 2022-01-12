#pragma once

#include <memory>

#include "exchanges/exchange.h"
#include "networking/websocket/websocket_client.h"

std::unique_ptr<Exchange> make_kraken(std::shared_ptr<WebsocketClient> websocketClient);