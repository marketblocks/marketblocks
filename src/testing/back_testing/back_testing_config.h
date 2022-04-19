#pragma once

#include <vector>
#include <string>

#include "common/json/json.h"

namespace mb
{
	class back_testing_config
	{
	private:
		std::time_t _startTime;
		int _stepSize;
		std::vector<std::string> _includedPairs;
		std::vector<std::string> _excludedPairs;

	public:
		constexpr back_testing_config()
			: _startTime{ 0 }, _stepSize{ 60 }, _includedPairs{}, _excludedPairs{}
		{}

		constexpr back_testing_config(
			std::time_t startTime,
			int stepSize,
			std::vector<std::string> includedPairs,
			std::vector<std::string> excludedPairs)
			:
			_startTime{ startTime }, 
			_stepSize{ stepSize }, 
			_includedPairs{ std::move(includedPairs) },
			_excludedPairs{ std::move(excludedPairs) }
		{}

		static constexpr std::string name() noexcept { return "back_testing"; }

		constexpr std::time_t start_time() const noexcept { return _startTime; }
		constexpr int step_size() const noexcept { return _stepSize; }
		constexpr const std::vector<std::string>& included_pairs() const noexcept { return _includedPairs; }
		constexpr const std::vector<std::string>& excluded_pairs() const noexcept { return _excludedPairs; }
	};

	template<>
	back_testing_config from_json<back_testing_config>(const json_document& json);

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer);
}
