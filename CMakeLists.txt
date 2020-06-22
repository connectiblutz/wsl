cmake_minimum_required(VERSION 3.13)
project(wsl VERSION 0.5.0 LANGUAGES CXX)

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