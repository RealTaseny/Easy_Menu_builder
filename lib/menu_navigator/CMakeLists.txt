cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(menu_navigator_lib
        src/menu_navigator.cpp
        src/menu_c_wrapper.cpp
)

target_include_directories(menu_navigator_lib   INTERFACE
        include/menu_navigator.h
        include/menu_c_wrapper.h
)
