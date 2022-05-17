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

	std::string to_string(std::time_t time, std::string_view format)
	{
		constexpr int BUFFER_SIZE = 50;
		char buffer[BUFFER_SIZE];
		strftime(buffer, BUFFER_SIZE, format.data(), localtime(&time));

		return std::string{ buffer };
	}
}