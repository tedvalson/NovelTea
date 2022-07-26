if (ENABLE_COVERAGE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NT_COVERAGE_FLAGS}")
endif()

add_subdirectory(core)

if(BUILD_EDITOR)
	add_subdirectory(editor)
endif()
