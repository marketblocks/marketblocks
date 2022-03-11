#pragma once

#include "test_data/test_data_constants.h"
#include "common/file/file.h"
#include "common/file/json.h"

namespace cb::test
{
	template<typename Config>
	Config load_test_config()
	{
		static constexpr std::string_view TEST_CONFIG_FOLDER = "test_exchange_configs";

		std::filesystem::path path{ TEST_DATA_FOLDER };
		path.append(TEST_CONFIG_FOLDER);
		path.append(Config::name() + "_test");
		path.replace_extension(JSON_FILE_EXTENSION);

		std::string jsonString{ read_file(path) };
		return from_json<Config>(jsonString);
	}
}