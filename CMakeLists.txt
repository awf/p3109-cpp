cmake_minimum_required(VERSION 3.20)

project(p3109_cpp LANGUAGES CXX)

if(POLICY CMP0167)
	cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost CONFIG REQUIRED)
find_library(MPFR_LIBRARY mpfr REQUIRED)
find_library(GMP_LIBRARY gmp REQUIRED)

enable_testing()

add_executable(p3109_demo p3109.cpp)
target_include_directories(p3109_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(p3109_demo PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})

add_executable(p3109_tests tests/test_decode.cpp tests/test_utils.cpp)
target_include_directories(p3109_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(p3109_tests PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})

add_test(NAME decode_zero COMMAND p3109_tests zero 8 3 signed)
add_test(NAME decode_subnormal COMMAND p3109_tests subnormal 8 3 signed)
add_test(NAME decode_normal COMMAND p3109_tests normal 8 3 signed)
add_test(NAME decode_reflection COMMAND p3109_tests reflection 8 3 signed)
add_test(NAME decode_special_values COMMAND p3109_tests special_values 8 3 signed)
