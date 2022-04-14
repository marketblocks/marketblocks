#pragma once

#include "common/exceptions/mb_exception.h"

namespace mb
{
	class http_error : public mb_exception
	{
	public:
		http_error(std::string message)
			: mb_exception{ "HTTP Error: " + std::move(message)}
		{}
	};
}
