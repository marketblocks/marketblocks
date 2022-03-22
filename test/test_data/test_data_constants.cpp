#include "test_data_constants.h"

namespace
{
	std::filesystem::path get_file_path(std::string_view folder, std::string_view fileName)
	{
		std::filesystem::path path{ std::format("{0}\\{1}", cb::test::TEST_DATA_FOLDER, folder) };
		path.append(fileName);

		return path;
	}
}

namespace cb::test
{
	std::filesystem::path kraken_results_test_data_path(std::string_view fileName)
	{
		return get_file_path("kraken_results_test", fileName);
	}

	std::filesystem::path kraken_websocket_test_data_path(std::string_view fileName)
	{
		return get_file_path("kraken_websocket_test", fileName);
	}

	std::filesystem::path coinbase_results_test_data_path(std::string_view fileName)
	{
		return get_file_path("coinbase_results_test", fileName);
	}
}