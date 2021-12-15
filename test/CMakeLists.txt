include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        release-1.11.0)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(CryptoBotTest "unittest/utils/stringutils_test.cpp" "unittest/utils/mathutils_test.cpp" "unittest/utils/financeutils_test.cpp" "unittest/strategies/tri_arb_test.cpp" "unittest/mocks.h" "unittest/exchanges/paper_trader/paper_trader_test.cpp" "unittest/common/trading/fee_schedule_test.cpp" "unittest/common/security/hash_test.cpp")

target_link_libraries(CryptoBotTest LINK_PUBLIC CryptoBot_lib)
target_link_libraries(CryptoBotTest PRIVATE gtest_main gmock_main)

include(GoogleTest)
gtest_discover_tests(CryptoBotTest)