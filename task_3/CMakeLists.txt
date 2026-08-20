cmake_minimum_required(VERSION 3.14)
project(LLVM_sccp_pass)

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

add_llvm_pass_plugin(SccpPass
    src/sccp_pass.cpp
)

target_include_directories(SccpPass PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include    
    ${LLVM_INCLUDE_DIRS}
)