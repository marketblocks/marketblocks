#pragma once

#include "common/exceptions/mb_exception.h"

namespace mb
{
	class initialisation_error : public mb_exception
	{
	public:
		initialisation_error(std::string message)
			: mb_exception{ std::move(message) }
		{}
	};
}
