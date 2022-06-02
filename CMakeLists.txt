project(emd)
cmake_minimum_required(VERSION 2.8)

add_compile_options(--std=c++14 -O3 -Wall -g)

include_directories(src)

file(GLOB CPP_LIB_SOURCES src/*/*.cpp)
file(GLOB C_LIB_SOURCES src/*/*.c)
set(LIB_SOURCES ${CPP_LIB_SOURCES} ${C_LIB_SOURCES})
message(STATUS ${LIB_SOURCES})

add_library(lib ${LIB_SOURCES})

add_executable(example src/example.cpp ${LIB_SOURCES})
target_link_libraries(example lib)
