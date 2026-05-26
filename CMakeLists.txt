cmake_minimum_required(VERSION 3.1)
project(Systemy_Wbudowane)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(Systemy_Wbudowane STATIC src/library.cpp)


# Define SystemC location (can be overridden with -DSYSTEMC_HOME=...)
if(NOT DEFINED SYSTEMC_HOME)
    set(SYSTEMC_HOME "/home/soph/systemc-3.0.2")
endif()

include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib-linux64)

# Create library
add_library(Systemy_Wbudowane STATIC src/library.cpp)

# Create executable and link both the library AND SystemC
add_executable(SystemC_Project src/EclipsePr.cpp)
target_link_libraries(SystemC_Project 
    PRIVATE Systemy_Wbudowane systemc m
)