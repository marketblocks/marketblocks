#pragma once

#include "networking/http/http_service.h"
#include "common/types/result.h"

namespace mb::internal
{
	template<typename Value, typename ResponseReader>
	Value send_http_request(const http_service& httpService, http_request& request, const ResponseReader& reader)
	{
		http_response response{ httpService.send(request) };

		if (response.response_code() != HttpResponseCodes::OK)
		{
			throw mb_exception{ response.message() };
		}

		result<Value> result{ reader(response.message()) };

		if (result.is_success())
		{
			return result.value();
		}

		throw mb_exception{ result.error() };
	}

	template<typename T>
	std::unordered_map<tradable_pair, T> create_pair_result_map(const std::vector<tradable_pair>& pairs, std::unordered_map<std::string, T> namedResults)
	{
		std::unordered_map<std::string, tradable_pair> pairLookup{ to_unordered_map<std::string, tradable_pair>(
			pairs,
			[](const tradable_pair& pair) { return pair.to_string(); },
			[](const tradable_pair& pair) { return pair; }) };

		std::unordered_map<tradable_pair, double> result;
		result.reserve(namedResults.size());

		for (auto& [name, value] : namedResults)
		{
			result.emplace(pairLookup.at(name), std::move(value));
		}

		return result;
	}
}