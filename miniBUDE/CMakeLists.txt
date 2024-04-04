cmake_minimum_required(VERSION 3.17)
project(hpx_bench VERSION 1.0.0)

find_package(HPX REQUIRED)

# set(CMAKE_CXX_FLAGS "-march=native")

set(target "seq")
add_executable(${target} ${target}.cpp)

find_package(Boost REQUIRED)
set(target "simd")
add_executable(${target} ${target}.cpp)
target_link_libraries(${target} PUBLIC Boost::boost)

set(target "omp")
add_executable(${target} ${target}.cpp)
target_compile_options(${target} PUBLIC "-fopenmp")
target_link_options(${target} PUBLIC "-fopenmp")

set(target "omp_simd")
add_executable(${target} ${target}.cpp)
target_compile_options(${target} PUBLIC "-fopenmp")
target_link_options(${target} PUBLIC "-fopenmp")


set(target "hpx")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)

set(target "test")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)


set(target "hpx_fj")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)

set(target "hpx_static_cs")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)

set(target "hpx_simd")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)

