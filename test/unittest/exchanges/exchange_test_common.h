#pragma once

#include <gtest/gtest.h>

#include "common/file/file.h"
#include "common/file/config_file_reader.h"
#include "test_data/test_data_constants.h"
#include "unittest/assertion_helpers.h"
#include "unittest/mocks.h"
#include "unittest/common_matchers.h"

namespace
{
	using namespace mb;

	template<typename ResultReader>
	auto execute_reader(const std::filesystem::path& dataPath, ResultReader reader)
	{
		std::string json = read_file(dataPath);
		return reader(json);
	}
}

namespace mb::test
{
	using ::testing::Types;

	template<typename Api>
	tradable_pair get_testing_pair()
	{
		static_assert(sizeof(Api) == 0, "No specialization for get_testing_pair exists");
	}

	template<typename Api>
	std::unique_ptr<exchange> create_exchange_api(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		static_assert(sizeof(Api) == 0, "No specialization for create_exchange_api exists");
	}

	template<typename Api, typename Config>
	std::unique_ptr<exchange> create_exchange_api(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		Config config{ internal::load_or_create_config<Config>() };
		return std::make_unique<Api>(
			std::move(config),
			std::move(httpService),
			websocketStream,
			true);
	}


	static const std::string ERROR_MESSAGE = "This is an error";
	static const std::string ERROR_RESPONSE_FILE_NAME = "error_response.json";

	using ::testing::Return;

	template<typename T, typename ResultReader, typename ValueAsserter>
	void execute_reader_test(const std::filesystem::path& dataPath, const std::filesystem::path& errorFilePath, const ResultReader& reader, T expectedValue, const ValueAsserter& valueAsserter)
	{
		assert_result_equal(
			execute_reader(dataPath, reader),
			result<T>::success(std::move(expectedValue)),
			valueAsserter);

		assert_result_equal(
			execute_reader(errorFilePath, reader),
			result<T>::fail(ERROR_MESSAGE),
			no_assert<T>);
	}

	template<typename T, typename ResultReader>
	void execute_reader_test(const std::filesystem::path& dataPath, const std::filesystem::path& errorFilePath, const ResultReader& reader, const T& expectedValue)
	{
		execute_reader_test(dataPath, errorFilePath, reader, expectedValue, default_expect_eq<T>);
	}

	template<typename ResultReader>
	void execute_reader_test(const std::filesystem::path& dataPath, const std::filesystem::path& errorFilePath, const ResultReader& reader)
	{
		EXPECT_EQ(execute_reader(dataPath, reader).is_success(), true);
		EXPECT_EQ(execute_reader(errorFilePath, reader).error(), ERROR_MESSAGE);
	}

	template<typename HttpRequestMatcher>
	std::unique_ptr<mock_http_service> create_mock_http_service(http_request expectedRequest, http_response response, HttpRequestMatcher matcher)
	{
		std::unique_ptr<mock_http_service> mockHttpService{ std::make_unique<mock_http_service>() };
		EXPECT_CALL(*mockHttpService, send(matcher(std::move(expectedRequest))))
			.Times(1)
			.WillOnce(Return(std::move(response)));

		return mockHttpService;
	}
}