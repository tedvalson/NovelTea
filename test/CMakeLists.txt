enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${GTEST_INCLUDE_DIRS}
)

set(COREROOT ${PROJECT_SOURCE_DIR}/src/core)
set(SRCROOT ${PROJECT_SOURCE_DIR}/test)

set(SRCTESTS
    ${SRCROOT}/main.cpp
)

add_executable(tests ${SRCTESTS})
target_link_libraries(tests ${GTEST_BOTH_LIBRARIES} NovelTea sfml-graphics sfml-window sfml-system sfml-audio pthread)
set_target_properties(tests PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${NT_TEST_FLAGS} -std=c++11")
set_target_properties(tests PROPERTIES COMPILE_DEFINITIONS "EMULATION;TEST")
set_target_properties(tests PROPERTIES LINK_FLAGS "${CMAKE_CXX_FLAGS} ${NT_TEST_FLAGS}")
add_test(AllTests tests)
