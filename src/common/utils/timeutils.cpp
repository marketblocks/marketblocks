#include "timeutils.h"

namespace mb
{
	std::time_t to_time_t(std::string_view dateTime, std::string_view format)
	{
		std::tm time{};
		std::istringstream inputStream(dateTime.data());
		inputStream >> std::get_time(&time, format.data());
		return mktime(&time);
	}
}