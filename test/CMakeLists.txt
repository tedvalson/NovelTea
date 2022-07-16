include(FetchContent)
FetchContent_Declare(
  gtest
  QUIET
  URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

# we cannot analyse results without gcov
find_program(GCOV_PATH gcov)
if(NOT GCOV_PATH)
  message(FATAL_ERROR "Code coverage analysis requires gcov!")
endif()

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCROOT ${PROJECT_SOURCE_DIR}/test)

set(SRCTESTS
    ${SRCROOT}/main.cpp
    ${SRCROOT}/BBCodeParser.cpp
    ${SRCROOT}/Cutscene.cpp
    ${SRCROOT}/Dialogue.cpp
)

add_executable(tests ${SRCTESTS})
target_link_libraries(tests GTest::gtest_main NovelTea sfml-graphics sfml-window sfml-system sfml-audio pthread)

include(GoogleTest)
gtest_discover_tests(tests
  PROPERTIES
    LABELS "unit"
  DISCOVERY_TIMEOUT  # how long to wait (in seconds) before crashing
    240
  )
