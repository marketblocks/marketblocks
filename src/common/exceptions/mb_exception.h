#pragma once

#include <stdexcept>

namespace mb
{
	class mb_exception : public std::exception
	{
	public:
		mb_exception(std::string message)
			: std::exception{ message.c_str() }
		{}
	};
}