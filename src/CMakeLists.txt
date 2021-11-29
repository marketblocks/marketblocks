# Add source to this project's executable.
add_executable (CryptoBot "main.cpp"   "runner.h"   "exchanges/exchange.h" "misc/data.h" "misc/trading.h"   "strategies/tri_arb.cpp" "utils/vectorutils.h"       "networking/httpservice.h" "networking/httpservice.cpp" "networking/httpconstants.h" "exchanges/kraken/kraken.h" "exchanges/kraken/kraken.cpp"  "networking/json.h" "networking/json.cpp"  "utils/stringutils.h" "utils/stringutils.cpp"     "exchanges/paper_trader/paper_trader.cpp")

# TODO: Add tests and install targets if needed.

find_package(CURL CONFIG REQUIRED)
target_link_libraries(CryptoBot PRIVATE CURL::libcurl)

find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(CryptoBot PRIVATE rapidjson)