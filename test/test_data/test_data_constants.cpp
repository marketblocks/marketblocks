#include "test_data_constants.h"

namespace cb::test
{
	std::filesystem::path kraken_results_test_data_path(std::string_view fileName)
	{
		std::filesystem::path directory{ std::format("{0}\\{1}", TEST_DATA_FOLDER, "kraken_results_test") };
		directory.append(fileName);

		return directory;
	}

	std::filesystem::path coinbase_results_test_data_path(std::string_view fileName)
	{
		std::filesystem::path directory{ std::format("{0}\\{1}", TEST_DATA_FOLDER, "coinbase_results_test") };
		directory.append(fileName);

		return directory;
	}
}