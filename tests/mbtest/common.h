#include "common/utils/timeutils.h"

namespace mb::test
{
	using namespace mb;

	template<typename Predicate>
	bool wait_for_condition(Predicate pred, int msWait)
	{
		using namespace std::chrono;
		std::time_t start{ time_since_epoch<milliseconds>() };

		while (time_since_epoch<milliseconds>() < start + msWait)
		{
			if (pred())
			{
				return true;
			}
		}

		return false;
	}
}