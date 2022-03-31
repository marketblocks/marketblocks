#pragma once

#include <stdexcept>
#include <format>

namespace mb
{
	class not_implemented_exception : public std::exception
	{
	public:
		not_implemented_exception(std::string functionName)
			: std::exception{ std::format("Function: '{}' not implemented", functionName).c_str() }
		{}
	};
}