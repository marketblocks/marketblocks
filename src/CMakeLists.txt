# Add source to this project's executable.
add_library(CryptoBot_lib 
"runner.h" 
"common/data/data.h" 
"common/trading/fee_schedule.cpp" 
"common/trading/fee_schedule.h" 
"common/trading/tradable_pair.cpp" 
"common/trading/tradable_pair.h" 
"common/trading/trade_description.h"
"common/trading/trade_description.cpp"
"common/trading/trading_constants.h"
"common/trading/trading_options.cpp"
"common/trading/trading_options.h"
"exchanges/exchange.cpp"
"exchanges/exchange.h"
"exchanges/exchange_factories.cpp"
"exchanges/exchange_factories.h"
"exchanges/market_data.h"
"exchanges/trader.h"
"exchanges/kraken/kraken.cpp"
"exchanges/kraken/kraken.h"
"exchanges/paper_trading/paper_trader.cpp"
"exchanges/paper_trading/paper_trader.h"
"networking/httpconstants.h"
"networking/httpservice.cpp"
"networking/httpservice.h"
"networking/json_wrapper.cpp"
"networking/json_wrapper.h"
"strategies/tri_arb.cpp"
"strategies/tri_arb.h"
"utils/financeutils.h"
"utils/mathutils.h"
"utils/stringutils.cpp"
"utils/stringutils.h"
"utils/vectorutils.h" "utils/financeutils.cpp")

# TODO: Add tests and install targets if needed.
find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE rapidjson)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE CURL::libcurl)

target_include_directories (CryptoBot_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})