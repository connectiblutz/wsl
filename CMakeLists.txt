cmake_minimum_required(VERSION 3.8)


add_library(wsl
  wslutil.cc
  wslutil.h
  WslApiLoader.cpp
  WslApiLoader.h
)
target_include_directories(wsl PUBLIC
  .
)
target_link_libraries(wsl PRIVATE
  common
)


target_link_directories(wsl PUBLIC
  .
)