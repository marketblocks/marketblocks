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
		POST,
		HTTP_DELETE
	};

	constexpr std::string to_string(http_verb verb)
	{
		switch (verb)
		{
		case http_verb::GET:
			return "GET";
		case http_verb::POST:
			return "POST";
		case http_verb::HTTP_DELETE:
			return "DELETE";
		}

		throw std::invalid_argument("HTTP verb not recognized");
	}
}
