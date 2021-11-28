# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required (VERSION 3.8)

project ("CryptoBot")

# Include sub-projects.
add_subdirectory ("src")