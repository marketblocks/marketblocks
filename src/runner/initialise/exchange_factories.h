#pragma once

#include <memory>

#include "exchanges/exchange.h"

std::unique_ptr<Exchange> make_kraken();