include(FetchContent)
FetchContent_Declare(
  gtest
  URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz
  URL_HASH MD5=ecd1fa65e7de707cd5c00bdac56022cd
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SRCROOT ${PROJECT_SOURCE_DIR}/test)

set(SRCTESTS
	${SRCROOT}/main.cpp
	${SRCROOT}/BBCodeParser.cpp
)

add_executable(tests ${SRCTESTS})
target_link_libraries(tests gtest NovelTea)

if (ENABLE_COVERAGE)
	set_target_properties(tests PROPERTIES LINK_FLAGS "${CMAKE_CXX_FLAGS} ${NT_COVERAGE_FLAGS}")
endif()

if (WIN32)
	add_custom_command(
		TARGET tests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
		COMMAND_EXPAND_LISTS
	)
endif()

include(GoogleTest)
gtest_discover_tests(tests
  PROPERTIES
    LABELS "unit"
  DISCOVERY_TIMEOUT  # how long to wait (in seconds) before crashing
    240
  )
