#include "back_testing_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view START_TIME = "startTime";
		static constexpr std::string_view STEP_SIZE = "stepSize";
		static constexpr std::string_view DATA_DIRECTORY = "dataDirectory";
	}
}

namespace mb
{
	template<>
	back_testing_config from_json<back_testing_config>(const json_document& json)
	{
		return back_testing_config
		{
			json.get<std::time_t>(json_property_names::START_TIME),
			json.get<int>(json_property_names::STEP_SIZE),
			json.get<std::string>(json_property_names::DATA_DIRECTORY)
		};
	}

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer)
	{
		writer.add(json_property_names::START_TIME, config.start_time());
		writer.add(json_property_names::STEP_SIZE, config.step_size());
		writer.add(json_property_names::DATA_DIRECTORY, config.data_directory());
	}
}