#pragma once

template<typename Duration>
long long time_since_epoch() noexcept
{
	return 
		std::chrono::duration_cast<Duration>(
		std::chrono::system_clock::now().time_since_epoch())
		.count();
}
