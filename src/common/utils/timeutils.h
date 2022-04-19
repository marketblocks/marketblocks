#pragma once

#include <chrono>

namespace mb
{
	template<typename Duration>
	long long time_since_epoch() noexcept
	{
		return
			std::chrono::duration_cast<Duration>(
				std::chrono::system_clock::now().time_since_epoch())
			.count();
	}

	std::time_t to_time_t(std::string_view dateTime, std::string_view format);
}

