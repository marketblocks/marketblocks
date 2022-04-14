#pragma once

#include "common/exceptions/mb_exception.h"

namespace mb
{
	class websocket_error : public mb_exception
	{
	public:
		websocket_error(std::string message)
			: mb_exception{ "Websocket Error: " + std::move(message)}
		{}
	};
}