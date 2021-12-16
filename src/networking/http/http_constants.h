#pragma once

#include <string>

namespace HttpResponseCodes
{
	constexpr int OK = 200;
}

enum class HttpVerb
{
	GET,
	POST
};