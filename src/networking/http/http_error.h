#pragma once

#include "common/exceptions/cb_exception.h"

namespace mb
{
	class http_error : public cb_exception
	{
	public:
		http_error(std::string message)
			: cb_exception{ "HTTP Error: " + std::move(message)}
		{}
	};
}
