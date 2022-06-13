#pragma once

#include "mb_exception.h"

namespace mb
{
	class validation_exception : public mb_exception
	{
	public:
		validation_exception(std::string message)
			: mb_exception{ std::move(message) }
		{}
	};
}