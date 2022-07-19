# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
set(CMAKE_CXX_STANDARD 23)

cmake_minimum_required (VERSION 3.8)

project (marketblocks VERSION "4.0.0")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/project_settings.h.in ${CMAKE_BINARY_DIR}/generated/project_settings.h)

enable_testing()

# Include sub-projects.
add_subdirectory ("src")
add_subdirectory ("test")