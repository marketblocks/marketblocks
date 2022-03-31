#pragma once

#include <gmock/gmock.h>

namespace mb::test
{
	MATCHER_P(IsHttpRequest, expected, "")
	{
		return
			arg.verb() == expected.verb() &&
			arg.url() == expected.url() &&
			arg.content() == expected.content();
	}
}