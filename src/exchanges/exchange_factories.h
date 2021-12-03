#pragma once

#include <memory>

#include "exchange.h"

std::shared_ptr<Exchange> make_kraken();