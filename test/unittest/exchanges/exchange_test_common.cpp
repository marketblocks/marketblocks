#include "exchange_test_common.h"

namespace mb::test
{
	std::string read_response_file(std::string_view apiId, std::string_view fileName)
	{
		std::filesystem::path path{ TEST_DATA_FOLDER };
		path /= apiId;
		path /= "responses";
		path /= fileName;
		path.replace_extension(".json");

		return read_file(path);
	}

	std::string read_request_file(std::string_view apiId, std::string_view fileName)
	{
		std::filesystem::path path{ TEST_DATA_FOLDER };
		path /= apiId;
		path /= "requests";
		path /= fileName;
		path.replace_extension(".json");

		return read_file(path);
	}
}