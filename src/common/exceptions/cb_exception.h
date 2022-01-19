#pragma once

#include <stdexcept>

namespace cb
{
	class cb_exception : public std::exception
	{
	public:
		cb_exception(std::string message)
			: std::exception{ message.c_str() }
		{}
	};
}