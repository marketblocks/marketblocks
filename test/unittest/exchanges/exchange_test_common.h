#pragma once

#include "common/file/file.h"
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