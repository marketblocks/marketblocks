#pragma once

#include "common/exceptions/cb_exception.h"

namespace mb
{
	class initialisation_error : public cb_exception
	{
	public:
		initialisation_error(std::string message)
			: cb_exception{ std::move(message) }
		{}
	};
}
