#include "timeutils.h"

#include <chrono>

long long milliseconds_since_epoch() noexcept
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch())
		.count();
}