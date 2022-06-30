#include "back_testing_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view START_TIME = "startTime";
		static constexpr std::string_view END_TIME = "endTime";
		static constexpr std::string_view STEP_SIZE = "stepSize";
		static constexpr std::string_view DATA_DIRECTORY = "dataDirectory";
		static constexpr std::string_view DYNAMIC_LOAD = "dynamicDataLoad";
	}
}

namespace mb
{
	back_testing_config::back_testing_config(
		std::time_t startTime,
		std::time_t endTime,
		int stepSize,
		std::string dataDirectory,
		bool dynamicLoad)
		:
		_startTime{ startTime },
		_endTime{ endTime },
		_stepSize{ stepSize },
		_dataDirectory{ std::move(dataDirectory) },
		_dynamicLoad{ dynamicLoad }
	{
		validate();
	}

	void back_testing_config::validate()
	{
		if (_endTime != 0)
		{
			assert_throw(_endTime > _startTime, "End time must be greater than start time");
		}

		if (_dynamicLoad)
		{
			assert_throw(_startTime != 0, "Start time cannot be 0 when dynamic data load enabled");
			assert_throw(_endTime != 0, "End time cannot be 0 when dynamic data load enabled");
		}

		assert_throw(_stepSize > 0, "Step size must be greater than zero");
	}

	template<>
	back_testing_config from_json<back_testing_config>(const json_document& json)
	{
		return back_testing_config
		{
			json.get<std::time_t>(json_property_names::START_TIME),
			json.get<std::time_t>(json_property_names::END_TIME),
			json.get<int>(json_property_names::STEP_SIZE),
			json.get<std::string>(json_property_names::DATA_DIRECTORY),
			json.get<bool>(json_property_names::DYNAMIC_LOAD)
		};
	}

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer)
	{
		writer.add(json_property_names::START_TIME, config.start_time());
		writer.add(json_property_names::END_TIME, config.end_time());
		writer.add(json_property_names::STEP_SIZE, config.step_size());
		writer.add(json_property_names::DATA_DIRECTORY, config.data_directory());
		writer.add(json_property_names::DYNAMIC_LOAD, config.dynamic_load());
	}
}