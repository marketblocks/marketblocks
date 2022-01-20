#pragma once

#include <string>

namespace cb
{
	enum class exchange_status
	{
		ONLINE,
		MAINTENANCE,
		CANCEL_ONLY,
		POST_ONLY
	};

	std::string to_string(exchange_status status);
}