#pragma once

#include <stdexcept>
#include <fmt/format.h>

namespace mb
{
	class not_implemented_exception : public std::exception
	{
	public:
		not_implemented_exception(std::string functionName)
			: std::exception{ fmt::format("Function: '{}' not implemented", functionName).c_str() }
		{}
	};
}