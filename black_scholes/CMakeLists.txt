cmake_minimum_required(VERSION 3.17)
project(hpx_bench VERSION 1.0.0)


# set(CMAKE_CXX_FLAGS "-march=native")

find_package(OpenMP REQUIRED)
set(target "omp")
add_executable(${target} omp.cpp)
target_link_libraries(${target} PUBLIC OpenMP::OpenMP_CXX)

list(APPEND CMAKE_MODULE_PATH "/home/srinivasyadav/Documents/work/thesis/tbb_install/lib/cmake")

find_package(TBB REQUIRED)
set(target "tbb_bench")
add_executable(${target} ${target}.cpp)
target_link_libraries(${target} PUBLIC tbb)



find_package(HPX REQUIRED)
set(target "hpx")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)
