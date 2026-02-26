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

add_executable(p3109_tests tests/p3109_test.cpp tests/test_decode.cpp tests/test_encode.cpp tests/test_project_ops.cpp tests/test_sqrt.cpp tests/test_utils.cpp)
target_include_directories(p3109_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(p3109_tests PRIVATE Boost::headers ${MPFR_LIBRARY} ${GMP_LIBRARY})

add_test(NAME decode_zero COMMAND p3109_tests decode_zero 8 3 signed)
add_test(NAME decode_subnormal COMMAND p3109_tests decode_subnormal 8 3 signed)
add_test(NAME decode_normal COMMAND p3109_tests decode_normal 8 3 signed)
add_test(NAME decode_reflection COMMAND p3109_tests decode_reflection 8 3 signed)
add_test(NAME decode_special_values COMMAND p3109_tests decode_special_values 8 3 signed)

add_test(NAME encode_zero COMMAND p3109_tests encode_zero 8 3 signed)
add_test(NAME encode_subnormal COMMAND p3109_tests encode_subnormal 8 3 signed)
add_test(NAME encode_normal COMMAND p3109_tests encode_normal 8 3 signed)
add_test(NAME encode_reflection COMMAND p3109_tests encode_reflection 8 3 signed)
add_test(NAME encode_special_values COMMAND p3109_tests encode_special_values 8 3 signed)
add_test(NAME encode_roundtrip_sample COMMAND p3109_tests encode_roundtrip_sample 8 3 signed)

add_test(NAME round_ties_to_even COMMAND p3109_tests round_ties_to_even 8 3 signed)
add_test(NAME round_ties_to_away COMMAND p3109_tests round_ties_to_away 8 3 signed)
add_test(NAME saturate_finite COMMAND p3109_tests saturate_finite 8 3 signed)
add_test(NAME saturate_propagate COMMAND p3109_tests saturate_propagate 8 3 signed)
add_test(NAME saturate_ovf_inf_toward_zero COMMAND p3109_tests saturate_ovf_inf_toward_zero 8 3 signed)
add_test(NAME saturate_ovf_inf_nearest COMMAND p3109_tests saturate_ovf_inf_nearest 8 3 signed)
add_test(NAME project_nan COMMAND p3109_tests project_nan 8 3 signed)
add_test(NAME project_ovf_inf_nearest COMMAND p3109_tests project_ovf_inf_nearest 8 3 signed)
add_test(NAME project_ovf_inf_toward_zero COMMAND p3109_tests project_ovf_inf_toward_zero 8 3 signed)
add_test(NAME project_finite_requires_satfinite COMMAND p3109_tests project_finite_requires_satfinite 8 3 signed)

add_test(NAME sqrt_nan COMMAND p3109_tests sqrt_nan 8 3 signed)
add_test(NAME sqrt_neginf COMMAND p3109_tests sqrt_neginf 8 3 signed)
add_test(NAME sqrt_negative COMMAND p3109_tests sqrt_negative 8 3 signed)
add_test(NAME sqrt_posinf COMMAND p3109_tests sqrt_posinf 8 3 signed)
add_test(NAME sqrt_positive COMMAND p3109_tests sqrt_positive 8 3 signed)
