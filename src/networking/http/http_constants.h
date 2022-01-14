#pragma once

#include <string>

namespace cb
{
	namespace HttpResponseCodes
	{
		constexpr int OK = 200;
	}

	enum class http_verb
	{
		GET,
		POST
	};
}
