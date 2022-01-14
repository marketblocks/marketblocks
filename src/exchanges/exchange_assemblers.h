#pragma once

#include <memory>

#include "exchange.h"

namespace cb
{
	std::shared_ptr<exchange> assemble_live(std::unique_ptr<exchange> api);
	std::shared_ptr<exchange> assemble_live_test(std::unique_ptr<exchange> api);
}