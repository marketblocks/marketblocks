#include "exchange_status.h"

namespace cb
{
	std::string to_string(exchange_status status)
	{
		switch (status)
		{
		case exchange_status::ONLINE:
			return "Online";
		case exchange_status::MAINTENANCE:
			return "Maintenance";
		case exchange_status::CANCEL_ONLY:
			return "Cancel Only";
		case exchange_status::LIMIT_ONLY:
			return "Limit Only";
		case exchange_status::POST_ONLY:
			return "Post Only";
		case exchange_status::OFFLINE:
			return "Offline";
		default:
			return "Unknown exchange status";
		}
	}
}