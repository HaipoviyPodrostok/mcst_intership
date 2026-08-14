cmake_minimum_required(VERSION 3.14)
project("mcst_intership")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GRAPH_SRCS
  src/graph.cpp
  src/node.cpp
  src/graph_algo.cpp
)

add_library(graph_lib STATIC ${GRAPH_SRCS})
target_include_directories(graph_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(a.out main.cpp)
target_link_libraries(a.out PRIVATE graph_lib)

option(ENABLE_DUMP "Enable graphviz dumps " OFF)
if (ENABLE_DUMP)
    target_compile_definitions(graph_lib PUBLIC ENABLE_DUMP)
    target_compile_definitions(a.out PRIVATE ENABLE_DUMP)
endif()

option(BUILD_TESTING "Enable testing" OFF)
if (BUILD_TESTING)
    enable_testing()
    include(CTest)

    add_subdirectory(tests/unit)

    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    file(GLOB E2E_TEST_FILES "${CMAKE_SOURCE_DIR}/tests/end2end/tests/*.txt")
    foreach(TEST_FILE ${E2E_TEST_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_test(NAME e2e:${TEST_NAME}
                 COMMAND ${Python3_EXECUTABLE}
                         ${CMAKE_SOURCE_DIR}/tests/end2end/run.py
                         -b $<TARGET_FILE:a.out>
                         -m ${TEST_NAME}
                         -t ${CMAKE_SOURCE_DIR}/tests/end2end/tests
                         -k ${CMAKE_SOURCE_DIR}/tests/end2end/keys
        )
        set_tests_properties(e2e:${TEST_NAME} PROPERTIES LABELS "e2e")
    endforeach()
endif()

