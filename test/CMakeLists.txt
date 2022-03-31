include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        release-1.11.0)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(marketblocks_test 
"unittest/mocks.h" 
"unittest/utils/stringutils_test.cpp" 
"unittest/utils/mathutils_test.cpp" 
"unittest/utils/financeutils_test.cpp" 
"unittest/strategies/tri_arb_test.cpp" 
"unittest/exchanges/paper_trader/paper_trader_test.cpp" 
 
"unittest/common/security/hash_test.cpp"
"unittest/utils/retry_test.cpp" 
"unittest/exchanges/kraken/kraken_results_test.cpp"
"unittest/assertion_helpers.h" 
"unittest/exchanges/kraken/kraken_test.cpp" 
"unittest/common_matchers.h"
"test_data/test_data_constants.h"
"test_data/test_data_constants.cpp" 
"unittest/exchanges/coinbase/coinbase_test.cpp" 
"unittest/exchanges/coinbase/coinbase_results_test.cpp"
"unittest/assertion_helpers.cpp" 
"integration/coinbase_test.cpp"
"test_data/test_exchange_configs/test_config_loader.h"
"integration/kraken_test.cpp"
"unittest/exchanges/kraken/kraken_websocket_test.cpp" 
"unittest/exchanges/websockets/order_book_cache_test.cpp"
"unittest/exchanges/coinbase/coinbase_websocket_test.cpp"  "unittest/common/types/set_queue_test.cpp")

target_link_libraries(marketblocks_test LINK_PUBLIC marketblocks_lib)
target_link_libraries(marketblocks_test PRIVATE gtest_main gmock_main)

add_custom_command(TARGET marketblocks_test POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_CURRENT_SOURCE_DIR}/test_data/ $<TARGET_FILE_DIR:marketblocks_test>/test_data)

include(GoogleTest)
gtest_discover_tests(marketblocks_test)

target_include_directories (marketblocks_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})