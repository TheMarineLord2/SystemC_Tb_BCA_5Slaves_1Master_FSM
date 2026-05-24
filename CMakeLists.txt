cmake_minimum_required(VERSION 4.2)
project(Systemy_Wbudowane)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(Systemy_Wbudowane STATIC src/library.cpp)


set(SYSTEMC_HOME "/home/soph/systemc-3.0.2")
include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib-linux64)

add_executable(SystemC_Project ./src/EclipsePr.cpp)
target_link_libraries(SystemC_Project systemc m)