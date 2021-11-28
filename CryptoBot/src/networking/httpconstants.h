#pragma once

#include <string_view>

namespace HttpResponseCodes
{
	constexpr int OK = 200;
}

namespace HttpVerbs
{
	constexpr std::string_view GET = "GET";
}