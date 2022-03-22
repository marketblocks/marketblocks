#pragma once

#include <filesystem>
#include <format>

namespace cb::test
{
	static constexpr std::string_view TEST_DATA_FOLDER = "test_data";

	std::filesystem::path kraken_results_test_data_path(std::string_view fileName);
	std::filesystem::path kraken_websocket_test_data_path(std::string_view fileName);
	std::filesystem::path coinbase_results_test_data_path(std::string_view fileName);
}