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

# Demo executable
add_executable(p3109_demo p3109.cpp)
target_include_directories(p3109_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(p3109_demo PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})

# Tests
add_executable(test_decode tests/test_decode.cpp tests/test_utils.cpp)
target_include_directories(test_decode PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_decode PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME decode COMMAND test_decode)

add_executable(test_encode tests/test_encode.cpp tests/test_utils.cpp)
target_include_directories(test_encode PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_encode PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME encode COMMAND test_encode)

add_executable(test_project_ops tests/test_project_ops.cpp tests/test_utils.cpp)
target_include_directories(test_project_ops PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_project_ops PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME project_ops COMMAND test_project_ops)

add_executable(test_sqrt tests/test_sqrt.cpp tests/test_utils.cpp)
target_include_directories(test_sqrt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_sqrt PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME sqrt COMMAND test_sqrt)

add_executable(test_abs tests/test_abs.cpp tests/test_utils.cpp)
target_include_directories(test_abs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_abs PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME abs COMMAND test_abs)

add_executable(test_add tests/test_add.cpp tests/test_utils.cpp)
target_include_directories(test_add PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_add PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME add COMMAND test_add)

add_executable(test_fma tests/test_fma.cpp tests/test_utils.cpp)
target_include_directories(test_fma PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_fma PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME fma COMMAND test_fma)

add_executable(test_compare_equal tests/test_compare_equal.cpp tests/test_utils.cpp)
target_include_directories(test_compare_equal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_compare_equal PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME compare_equal COMMAND test_compare_equal)

add_executable(test_is_zero tests/test_is_zero.cpp tests/test_utils.cpp)
target_include_directories(test_is_zero PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_is_zero PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME is_zero COMMAND test_is_zero)

add_executable(test_format tests/test_format.cpp tests/test_utils.cpp)
target_include_directories(test_format PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_format PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})
add_test(NAME format COMMAND test_format)
