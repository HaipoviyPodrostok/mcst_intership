include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(graph_test.x
    graph_test.cpp
)

target_include_directories(graph_test.x
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(graph_test.x
    PRIVATE
        graph_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(graph_test.x
    PROPERTIES LABELS "unit"
    TEST_PREFIX "unit:"
)
