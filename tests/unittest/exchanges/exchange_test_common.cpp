#include "exchange_test_common.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	std::string read_data_file(std::string_view apiId, std::string_view fileName, std::string_view category)
	{
		std::filesystem::path path{ TEST_DATA_FOLDER };
		path /= apiId;
		path /= category;
		path /= fileName;
		path.replace_extension(".json");

		return read_file(path);
	}
}

namespace mb::test
{
	std::string read_response_file(std::string_view apiId, std::string_view fileName)
	{
		return read_data_file(apiId, fileName, "responses");
	}

	std::string read_request_file(std::string_view apiId, std::string_view fileName)
	{
		return read_data_file(apiId, fileName, "requests");
	}

	std::string read_websockets_file(std::string_view apiId, std::string_view fileName)
	{
		return read_data_file(apiId, fileName, "websockets");
	}
}