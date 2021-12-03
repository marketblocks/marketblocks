# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
set(CMAKE_CXX_STANDARD 20)

cmake_minimum_required (VERSION 3.8)

project ("CryptoBot")

enable_testing()

# Include sub-projects.
add_subdirectory ("exe")
add_subdirectory ("src")
add_subdirectory ("test")