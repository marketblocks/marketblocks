# CMakeList.txt : CMake project for CryptoBot, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (CryptoBot "main.cpp"   "runner.h"   "exchanges/exchange.h" "data.h" "trading.h"   "strategies/tri_arb.cpp" "utils/vectorutils.h"     "exchanges/dummy_exchange.h" "exchanges/dummy_exchange.cpp")

# TODO: Add tests and install targets if needed.

find_package(CURL CONFIG REQUIRED)
target_link_libraries(CryptoBot PRIVATE CURL::libcurl)