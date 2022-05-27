#pragma once

#include <string_view>

namespace mb
{
	void assert_throw(bool condition, std::string_view message = "");
}