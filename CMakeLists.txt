cmake_minimum_required(VERSION 2.8.11)

# Project name without spaces or special characters
project("NovelTea")

option(BUILD_DOCS "Build doxygen documentation" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_EDITOR "Build NovelTea Qt5 editor" ON)
option(BUILD_ANDROID "Build Android app" OFF)

include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/external/TweenEngine/include
)

# C++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# disable the rpath stuff
set(CMAKE_SKIP_BUILD_RPATH TRUE)

# set the output directory for libraries and binaries
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# compile flags
set(NT_TEST_FLAGS "-g -O3 -coverage")
set(NT_EDITOR_FLAGS "-g -O3")

add_subdirectory(src)

if(BUILD_DOCS)
	add_subdirectory(doc)
endif()
if(BUILD_TESTS)
	add_subdirectory(test)
endif()
