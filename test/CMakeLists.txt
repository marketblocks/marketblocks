include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        release-1.11.0)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(marketblocks_test 
"unittest/mocks.h" 
"unittest/common/utils/stringutils_test.cpp" 
"unittest/common/utils/mathutils_test.cpp" 
"unittest/common/utils/financeutils_test.cpp" 
"unittest/common/utils/retry_test.cpp" 
"unittest/testing/paper_trading/paper_trader_test.cpp" 
"unittest/common/security/hash_test.cpp"
"unittest/exchanges/kraken/kraken_results_test.cpp"
"unittest/exchanges/kraken/kraken_test.cpp"
"unittest/assertion_helpers.h" 
"unittest/common_matchers.h"
"test_data/test_data_constants.h"
"test_data/test_data_constants.cpp" 
"unittest/exchanges/coinbase/coinbase_test.cpp" 
"unittest/exchanges/coinbase/coinbase_results_test.cpp"
"unittest/assertion_helpers.cpp" 
"test_data/test_exchange_configs/test_config_loader.h"
"unittest/exchanges/kraken/kraken_websocket_test.cpp" 
"unittest/exchanges/kraken/kraken_integration_test.cpp" 
"unittest/exchanges/websockets/order_book_cache_test.cpp"
"unittest/exchanges/coinbase/coinbase_integration_test.cpp"
"unittest/exchanges/coinbase/coinbase_websocket_test.cpp" 
"unittest/common/types/set_queue_test.cpp"
"unittest/common/csv/csv_test.cpp"
"unittest/common/csv/csv_row_test.cpp"  
"unittest/runner/backtest_runner_test.cpp" 
"unittest/testing/back_testing/back_testing_data_test.cpp"
"unittest/testing/back_testing/back_testing_report_test.cpp"
"unittest/exchanges/bybit/bybit_integration_test.cpp" 
"unittest/exchanges/bybit/bybit_integration_test.cpp" 
"unittest/exchanges/bybit/bybit_results_test.cpp" 
"unittest/exchanges/digifinex/digifinex_integration_test.cpp" 
"unittest/exchanges/digifinex/digifinex_results_test.cpp"
"unittest/exchanges/digifinex/digifinex_test.cpp" 
"unittest/exchanges/exchange_test_common.h"
"unittest/exchanges/websockets/exchange_websocket_stream_test.cpp"  "unittest/common/types/concurrent_wrapper_test.cpp" "unittest/testing/back_testing/data_loading/csv_data_source_test.cpp" "unittest/testing/back_testing/data_loading/data_factory_test.cpp")

target_link_libraries(marketblocks_test LINK_PUBLIC marketblocks_lib)
target_link_libraries(marketblocks_test PRIVATE gtest_main gmock_main)

add_custom_command(TARGET marketblocks_test POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_CURRENT_SOURCE_DIR}/test_data/ $<TARGET_FILE_DIR:marketblocks_test>/test_data)

include(GoogleTest)
gtest_discover_tests(marketblocks_test)

find_package(absl CONFIG REQUIRED)
target_link_libraries(marketblocks_test PRIVATE absl::any absl::base absl::bits absl::city)

target_include_directories (marketblocks_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})