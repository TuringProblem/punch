cmake_minimum_required(VERSION 3.20)
project(punch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

find_package(Threads REQUIRED)

# Header-only game rules. Everything except main.cpp lives here.
add_library(punch_core INTERFACE)
target_include_directories(punch_core INTERFACE include)
target_link_libraries(punch_core INTERFACE Threads::Threads)
target_compile_features(punch_core INTERFACE cxx_std_23)

if(MSVC)
  # Source files hold UTF-8 card glyphs; MSVC assumes the local codepage.
  target_compile_options(punch_core INTERFACE /utf-8)
else()
  target_compile_options(punch_core INTERFACE -Wall -Wextra)
endif()

add_executable(punch src/main.cpp)
target_include_directories(punch PRIVATE src)
target_link_libraries(punch PRIVATE punch_core)

include(CTest)
if(BUILD_TESTING)
  foreach(test deck_test eval_test tie_test punch_test engine_test game_test bot_test)
    add_executable(${test} tests/${test}.cpp)
    target_link_libraries(${test} PRIVATE punch_core)
    add_test(NAME ${test} COMMAND ${test})
  endforeach()
endif()
