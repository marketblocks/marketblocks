#pragma once

#include <memory>

#include "exchange.h"

std::shared_ptr<Exchange> assemble_live(std::unique_ptr<Exchange> api);

std::shared_ptr<Exchange> assemble_live_test(std::unique_ptr<Exchange> api);