#pragma once

#include <stdexcept>
#include <fmt/format.h>

namespace mb
{
	class not_implemented_exception : public std::exception
	{
	private:
		std::string _message;

	public:
		not_implemented_exception(std::string functionName)
			: _message{ fmt::format("Function: '{}' not implemented", functionName) }
		{}

		const char* what() const override { return _message.c_str(); }
	};
}