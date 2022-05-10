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
		std::string _dataDirectory;

	public:
		constexpr back_testing_config()
			: 
			_startTime{ 0 }, 
			_stepSize{ 60 }, 
			_dataDirectory{ "back_test_data" }
		{}

		constexpr back_testing_config(
			std::time_t startTime,
			int stepSize,
			std::string dataDirectory)
			:
			_startTime{ startTime },
			_stepSize{ stepSize },
			_dataDirectory{ std::move(dataDirectory) }
		{}

		static constexpr std::string name() noexcept { return "back_testing"; }

		constexpr std::time_t start_time() const noexcept { return _startTime; }
		constexpr int step_size() const noexcept { return _stepSize; }
		constexpr const std::string& data_directory() const noexcept { return _dataDirectory; }
	};

	template<>
	back_testing_config from_json<back_testing_config>(const json_document& json);

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer);
}
