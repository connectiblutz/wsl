cmake_minimum_required(VERSION 3.13)
project(wsl VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if (WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

add_library(wsl)
target_sources(wsl
  PRIVATE
    src/wslutil.cc
    src/WslApiLoader.cpp
    src/WslApiLoader.h
  PUBLIC
    include/wsl/wslutil.h
)
target_include_directories(wsl PUBLIC
  include/
)
target_link_libraries(wsl PRIVATE
  bcl
)

if (WIN32)
target_compile_definitions(wsl PRIVATE
  UNICODE
)
endif()