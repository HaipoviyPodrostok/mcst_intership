cmake_minimum_required(VERSION 3.14)
project("mcst_intership")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
  main.cpp
  src/graph.cpp

)

add_executable(a.out ${SOURCES})

target_include_directories(a.out PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)



