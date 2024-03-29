#pragma once

#include <gtest/gtest.h>

#include "common/file/file.h"
#include "common/file/config_file_reader.h"
#include "test_data/test_data_constants.h"
#include "mbtest/assertion_helpers.h"
#include "mbtest/mocks.h"

namespace mb::test
{
	using ::testing::Types;

	template<typename Api>
	tradable_pair get_testing_pair()
	{
		static_assert(sizeof(Api) == 0, "No specialization for get_testing_pair exists");
	}

	template<typename Api>
	tradable_pair get_testing_pair_2()
	{
		static_assert(sizeof(Api) == 0, "No specialization for get_testing_pair_2 exists");
	}

	template<typename Api>
	std::unique_ptr<exchange> create_exchange_api(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		static_assert(sizeof(Api) == 0, "No specialization for create_exchange_api exists");
	}

	template<typename WebsocketStream>
	std::unique_ptr<WebsocketStream> create_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
	{
		return std::make_unique<WebsocketStream>(std::move(connectionFactory));
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
			false);
	}

	std::string read_response_file(std::string_view apiId, std::string_view fileName);
	std::string read_request_file(std::string_view apiId, std::string_view fileName);
	std::string read_websockets_file(std::string_view apiId, std::string_view fileName);
}