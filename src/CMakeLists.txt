# Add source to this project's executable.
add_library(CryptoBot_lib "runner.h" "exchanges/exchange.h" "misc/data.h" "misc/trading.h" "utils/vectorutils.h"   "networking/httpconstants.h" "exchanges/kraken/kraken.h" "exchanges/kraken/kraken.cpp"  "networking/json.h" "networking/json.cpp"  "utils/stringutils.h" "utils/stringutils.cpp"  "exchanges/paper_trader/paper_trader.h"   "exchanges/paper_trader/paper_trader.cpp" "utils/mathutils.h" "utils/financeutils.h"  "networking/httpservice.h" "networking/httpservice.cpp" "networking/httpconstants.h" "exchanges/exchange_factories.h" "exchanges/exchange_factories.cpp"  "strategies/tri_arb.h" "strategies/tri_arb.cpp" "misc/trading.cpp" "misc/options.h" )

# TODO: Add tests and install targets if needed.
find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE rapidjson)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(CryptoBot_lib PRIVATE CURL::libcurl)

target_include_directories (CryptoBot_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})