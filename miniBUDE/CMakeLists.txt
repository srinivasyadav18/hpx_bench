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

set(target "hpx_hierarchical_2")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)
set(target "hpx_hierarchical_3")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)
set(target "hpx_hierarchical_4")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)
set(target "hpx_hierarchical_5")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)
set(target "hpx_hierarchical_6")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)
set(target "hpx_hierarchical_16")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)
set(target "hpx_hierarchical_32")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)


set(target "hpx_fj")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)

set(target "hpx_fj_simd")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)


set(target "hpx_static_cs")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)

set(target "hpx_simd")
add_hpx_executable(${target} ESSENTIAL SOURCES ${target}.cpp)

# message("*** HPXMP PATH : " ${HPXMP_PATH})
# message("*** HPXC_INCLUDE_PATH : " ${HPXC_INCLUDE_PATH})
# add_definitions(-DHPXC)
# find_library(HPXMP libomp.so
#   PATHS ${HPXMP_PATH}
#   NO_DEFAULT_PATH
# )

# message("*** HPXMP : " ${HPXMP})
# add_executable(hpxmp hpxmp.cpp)
# target_link_libraries(hpxmp ${HPXMP} pthread HPX::hpx HPX::wrap_main m)
# target_include_directories(hpxmp PUBLIC ${HPXC_INCLUDE_PATH})
# target_compile_options(hpxmp PUBLIC -fopenmp)

# add_executable(hpxmp_simd hpxmp_simd.cpp)
# target_link_libraries(hpxmp_simd ${HPXMP} pthread HPX::hpx HPX::wrap_main m)
# target_include_directories(hpxmp_simd PUBLIC ${HPXC_INCLUDE_PATH})
# target_compile_options(hpxmp_simd PUBLIC -fopenmp)
