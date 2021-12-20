# Add source to this project's executable.
add_library(CryptoBot_lib 
"runner.h" 
"common/trading/order_book.cpp"
"common/trading/order_book.h"
"common/security/hash.cpp"
"common/security/hash.h" 
"common/security/encoding.cpp"
"common/security/encoding.h" 
"common/trading/asset_symbol.cpp"
"common/trading/asset_symbol.h"
"common/trading/fee_schedule.cpp" 
"common/trading/fee_schedule.h" 
"common/trading/tradable_pair.cpp" 
"common/trading/tradable_pair.h" 
"common/trading/trade_description.h"
"common/trading/trade_description.cpp"
"common/trading/trading_constants.h"
"common/trading/trading_options.cpp"
"common/trading/trading_options.h"

"exchanges/exchange.h"
"exchanges/exchange_factories.cpp"
"exchanges/exchange_factories.h"
"exchanges/exchange_helpers.cpp"
"exchanges/exchange_helpers.h"


"exchanges/kraken/kraken.cpp"
"exchanges/kraken/kraken.h"
"exchanges/kraken/kraken_results.cpp"
"exchanges/kraken/kraken_results.h" 
"exchanges/paper_trading/paper_trader.cpp"
"exchanges/paper_trading/paper_trader.h"
"networking/http/http_constants.h"
"networking/http/http_service.cpp"
"networking/http/http_service.h"
"networking/http/http_request.cpp"
"networking/http/http_request.h"
"networking/http/http_header.cpp"
"networking/http/http_header.h"
"networking/http/http_response.cpp"
"networking/http/http_response.h" 
"networking/url.cpp"
"networking/url.h"
"networking/json_wrapper.cpp"
"networking/json_wrapper.h"
"strategies/tri_arb.cpp"
"strategies/tri_arb.h"
"utils/containerutils.h"
"utils/financeutils.cpp"
"utils/financeutils.h"
"utils/mathutils.h"
"utils/stringutils.cpp"
"utils/stringutils.h"
"utils/vectorutils.h")

# TODO: Add tests and install targets if needed.
find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE rapidjson)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE CURL::libcurl)

find_package(OpenSSL REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories (CryptoBot_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})