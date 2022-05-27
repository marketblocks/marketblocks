#include "generalutils.h"
#include "common/exceptions/mb_exception.h"

namespace mb
{
	void assert_throw(bool condition, std::string_view message)
	{
		if (!condition)
		{
			throw mb_exception{ message.data() };
		}
	}
}