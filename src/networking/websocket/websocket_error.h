#pragma once

#include "common/exceptions/cb_exception.h"

namespace mb
{
	class websocket_error : public cb_exception
	{
	public:
		websocket_error(std::string message)
			: cb_exception{ "Websocket Error: " + std::move(message)}
		{}
	};
}