# Add source to this project's executable.
add_library(marketblocks_lib 

"common/json/json.cpp"
"common/json/json.h"
"common/json/json_iterator.cpp"
"common/json/json_iterator.h" 
"common/file/file.cpp"
"common/file/file.h"
"common/file/config_file_reader.cpp" 
"common/file/config_file_reader.h" 


"common/security/hash.h" 

"common/security/encoding.h" 

"common/utils/containerutils.h"

"common/utils/financeutils.h"
"common/utils/mathutils.h"
"common/utils/retry.h"
"common/utils/stringutils.cpp"
"common/utils/stringutils.h"


"common/utils/timeutils.h" 

"common/exceptions/mb_exception.h" 
"common/exceptions/not_implemented_exception.h" 

"common/types/result.h"
"common/types/basic_iterator.h" 

"exchanges/exchange.h"
"runner/initialise/exchange_assemblers.cpp"  
"runner/initialise/exchange_assemblers.h" 

"exchanges/exchange_ids.h"
"exchanges/exchange_helpers.cpp"
"exchanges/exchange_helpers.h"

"exchanges/exchange_status.h" 

"exchanges/kraken/kraken.cpp"
"exchanges/kraken/kraken.h"
"exchanges/kraken/kraken_results.cpp"
"exchanges/kraken/kraken_results.h" 
"exchanges/kraken/kraken_config.cpp"
"exchanges/kraken/kraken_config.h"
"exchanges/kraken/kraken_websocket.h"
"exchanges/kraken/kraken_websocket.cpp"
"testing/paper_trading/paper_trade_api.cpp"
"testing/paper_trading/paper_trade_api.h"

"exchanges/websockets/websocket_stream.cpp"
"exchanges/websockets/order_book_cache.h"  
"exchanges/websockets/order_book_cache.cpp"

"logging/logger.h"
"logging/logger.cpp"

"networking/http/http_constants.h"
"networking/http/http_service.cpp"
"networking/http/http_service.h"

"networking/http/http_request.h"

"networking/http/http_header.h"

"networking/http/http_response.h" 
"networking/http/http_error.h"
"networking/websocket/websocket_error.h"
"networking/websocket/websocket_connection.cpp" 
"networking/websocket/websocket_connection.h" 
"networking/websocket/websocket_client.cpp"
"networking/websocket/websocket_client.h" 

"networking/url.h"

"runner/runner.h" 
"runner/initialise/runner_initialise.cpp"
"runner/initialise/runner_initialise.h"
"runner/initialise/strategy_initialiser.h"
"runner/runner_config.cpp" 
"runner/runner_config.h"
"runner/initialise/initialisation_error.h" 

"trading/order_book.h"


"trading/fee_schedule.cpp" 
"trading/fee_schedule.h" 
 
"trading/tradable_pair.h" 
"trading/trade_description.h"

"trading/trading_constants.h"

 "trading/ohlcv_data.h"  "trading/order_description.h"  "trading/order_type.h"  "common/types/unordered_string_map.h" "exchanges/coinbase/coinbase.h" "exchanges/coinbase/coinbase.cpp" "exchanges/coinbase/coinbase_websocket.h" "exchanges/coinbase/coinbase_websocket.cpp" "exchanges/coinbase/coinbase_results.h" "exchanges/coinbase/coinbase_results.cpp" "exchanges/coinbase/coinbase_config.h" "exchanges/coinbase/coinbase_config.cpp" "exchanges/websockets/local_order_book.cpp"    "exchanges/websockets/websocket_stream_implementation.h"   "common/types/set_queue.h" "testing/paper_trading/paper_trading_config.h" "testing/paper_trading/paper_trading_config.cpp" "common/json/json_constants.h" "common/json/json_writer.h"  "common/json/json_writer.cpp" "testing/back_testing/backtest_market_api.h" "testing/back_testing/backtest_market_api.cpp" "testing/back_testing/back_testing_config.h" "testing/back_testing/back_testing_config.cpp" "common/types/partial_data_result.h" "common/utils/timeutils.cpp" "common/csv/csv.h"   "common/csv/csv_row.h" "common/csv/csv.cpp" "testing/back_testing/back_testing_data.h" "testing/back_testing/back_testing_data.cpp" "testing/back_testing/data_loading.h" "testing/back_testing/data_loading.cpp" "common/csv/csv_row.cpp" "trading/tradable_pair.cpp" "trading/ohlcv_data.cpp" "testing/back_testing/backtest_websocket_stream.h")

# TODO: Add tests and install targets if needed.
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(marketblocks_lib PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(marketblocks_lib PRIVATE CURL::libcurl)

find_package(websocketpp CONFIG REQUIRED)
target_link_libraries(marketblocks_lib PRIVATE websocketpp::websocketpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(marketblocks_lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(marketblocks_lib PUBLIC spdlog::spdlog spdlog::spdlog_header_only)

target_include_directories (marketblocks_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(marketblocks_lib PRIVATE ${CMAKE_BINARY_DIR}/generated)