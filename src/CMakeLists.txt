# Add source to this project's executable.
add_library(CryptoBot_lib 

"common/file/json.cpp"
"common/file/json.h"
"common/file/json_iterator.cpp"
"common/file/json_iterator.h" 
"common/file/file.cpp"
"common/file/file.h"
"common/file/config_file_reader.cpp" 
"common/file/config_file_reader.h" 

"common/security/hash.cpp"
"common/security/hash.h" 
"common/security/encoding.cpp"
"common/security/encoding.h" 

"common/utils/containerutils.h"
"common/utils/financeutils.cpp"
"common/utils/financeutils.h"
"common/utils/mathutils.h"
"common/utils/stringutils.cpp"
"common/utils/stringutils.h"
"common/utils/vectorutils.h"
"common/utils/timeutils.cpp"
"common/utils/timeutils.h" 

"common/exceptions/cb_exception.h" 
"common/exceptions/not_implemented_exception.h" 

"common/types/result.h"
"common/types/basic_iterator.h" 

"exchanges/exchange.h"
"exchanges/exchange_assemblers.cpp"  
"exchanges/exchange_assemblers.h" 
"exchanges/exchange_id.cpp"
"exchanges/exchange_id.h"
"exchanges/exchange_helpers.cpp"
"exchanges/exchange_helpers.h"
"exchanges/exchange_status.cpp"
"exchanges/exchange_status.h" 

"exchanges/kraken/kraken.cpp"
"exchanges/kraken/kraken.h"
"exchanges/kraken/kraken_results.cpp"
"exchanges/kraken/kraken_results.h" 
"exchanges/kraken/kraken_config.cpp"
"exchanges/kraken/kraken_config.h"
"exchanges/kraken/kraken_websocket.h"
"exchanges/kraken/kraken_websocket.cpp"
"exchanges/paper_trading/paper_trader.cpp"
"exchanges/paper_trading/paper_trader.h"

"exchanges/websockets/websocket_stream.cpp"
"exchanges/websockets/order_book_cache.h"  
"exchanges/websockets/order_book_cache.cpp"

"logging/logger.h"
"logging/logger.cpp"

"networking/http/http_constants.h"
"networking/http/http_service.cpp"
"networking/http/http_service.h"
"networking/http/http_request.cpp"
"networking/http/http_request.h"
"networking/http/http_header.cpp"
"networking/http/http_header.h"
"networking/http/http_response.cpp"
"networking/http/http_response.h" 
"networking/http/http_error.h"
"networking/websocket/websocket_error.h"
"networking/websocket/websocket_connection.cpp" 
"networking/websocket/websocket_connection.h" 
"networking/websocket/websocket_client.cpp"
"networking/websocket/websocket_client.h" 
"networking/url.cpp"
"networking/url.h"

"runner/runner.h" 
"runner/initialise/runner_initialise.cpp"
"runner/initialise/runner_initialise.h"
"runner/initialise/strategy_initialiser.h"
"runner/runner_config.cpp" 
"runner/runner_config.h"
"runner/initialise/initialisation_error.h" 

"strategies/tri_arb.cpp"
"strategies/tri_arb.h"

"trading/order_book.cpp"
"trading/order_book.h"
"trading/asset_symbol.cpp"
"trading/asset_symbol.h"
"trading/fee_schedule.cpp" 
"trading/fee_schedule.h" 
"trading/tradable_pair.cpp" 
"trading/tradable_pair.h" 
"trading/trade_description.h"
"trading/trade_description.cpp"
"trading/trading_constants.h"
"trading/trading_options.cpp"
"trading/trading_options.h")

# TODO: Add tests and install targets if needed.
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE CURL::libcurl)

find_package(websocketpp CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE websocketpp::websocketpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PUBLIC spdlog::spdlog spdlog::spdlog_header_only)

target_include_directories (CryptoBot_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})