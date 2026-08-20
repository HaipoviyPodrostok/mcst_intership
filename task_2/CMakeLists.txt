cmake_minimum_required(VERSION 3.13.4)
project(MyPass)

set(LT_LLVM_INSTALL_DIR "/usr/lib/llvm-11/" CACHE PATH "LLVM installation directory")

find_package(LLVM REQUIRED CONFIG)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_library(MyPass SHARED ../src/MyPass.cpp)

target_link_libraries(MyPass
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")