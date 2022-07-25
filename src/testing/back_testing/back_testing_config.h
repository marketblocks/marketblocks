#pragma once

#include <vector>
#include <string>

#include "common/json/json.h"
#include "common/utils/generalutils.h"

namespace mb
{
	class back_testing_config
	{
	private:
		std::time_t _startTime;
		std::time_t _endTime;
		int _stepSize;
		std::string _dataDirectory;
		bool _dynamicLoad;

		void validate();

	public:
		back_testing_config();

		back_testing_config(
			std::time_t startTime,
			std::time_t endTime,
			int stepSize,
			std::string dataDirectory,
			bool dynamicLoad);

		static std::string name() noexcept { return "back_testing"; }

		std::time_t start_time() const noexcept { return _startTime; }
		std::time_t end_time() const noexcept { return _endTime; }
		int step_size() const noexcept { return _stepSize; }
		const std::string& data_directory() const noexcept { return _dataDirectory; }
		bool dynamic_load() const noexcept { return _dynamicLoad; }
	};

	template<>
	back_testing_config from_json<back_testing_config>(const json_document& json);

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer);
}
