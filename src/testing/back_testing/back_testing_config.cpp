#include "back_testing_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view START_TIME = "startTime";
		static constexpr std::string_view STEP_SIZE = "stepSize";
		static constexpr std::string_view REFRESH_DATA = "refreshData";
		static constexpr std::string_view DATA_PATH = "dataPath";
		static constexpr std::string_view INCLUDED_PAIRS = "includedPairs";
		static constexpr std::string_view EXCLUDED_PAIRS = "excludedPairs";
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
			json.get<bool>(json_property_names::REFRESH_DATA),
			json.get<std::string>(json_property_names::DATA_PATH),
			json.get<std::vector<std::string>>(json_property_names::INCLUDED_PAIRS),
			json.get<std::vector<std::string>>(json_property_names::EXCLUDED_PAIRS)
		};
	}

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer)
	{
		writer.add(json_property_names::START_TIME, config.start_time());
		writer.add(json_property_names::STEP_SIZE, config.step_size());
		writer.add(json_property_names::REFRESH_DATA, config.refresh_data());
		writer.add(json_property_names::DATA_PATH, config.data_path());
		writer.add(json_property_names::INCLUDED_PAIRS, config.included_pairs());
		writer.add(json_property_names::EXCLUDED_PAIRS, config.excluded_pairs());
	}
}