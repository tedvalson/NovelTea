cmake_minimum_required(VERSION 2.8.11)

project("NovelTea")

set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
set(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Choose whether SFML is linked statically or not.")
option(BUILD_DOCS "Build doxygen documentation" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_EDITOR "Build NovelTea Qt5 editor" ON)
option(BUILD_ANDROID "Build Android app" OFF)

if (NOT ANDROID)
	# Find SFML
	find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

	# Output an error if SFML wasn't found
	if(NOT SFML_FOUND)
		message("\n-> SFML libraries were not found. Set SFML_DIR to SFML's CMake Config path (generally located in \"lib/cmake/SFML\").")
		message("-> Make sure the SFML libraries with the same configuration (Release/Debug, Static/Dynamic) exist.\n")
	endif()
else()
    # we install libs in a subdirectory named after the ABI (lib/mips/*.so)
    set(LIB_SUFFIX "/${ANDROID_ABI}")
	set(CMAKE_INSTALL_PREFIX ${ANDROID_NDK}/sources/noveltea)
	add_definitions("-D_GLIBCXX_USE_C99 -D_GLIBCXX_HAVE_WCSTOF")
	include_directories(
		${ANDROID_NDK}/sources/third-party/sfml/include
	)
	install(DIRECTORY include
		DESTINATION .
		COMPONENT devel
		FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
	
	# install Android.mk so the NDK knows how to set up SFML
	install(FILES src/Android.mk DESTINATION .)
	install(FILES external/duktape.h DESTINATION ./include)
	install(FILES external/duk_config.h DESTINATION ./include)
	install(DIRECTORY external/dukglue DESTINATION ./include)
	install(DIRECTORY external/TweenEngine/include/TweenEngine DESTINATION ./include)
endif()

include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/external/TweenEngine/include
	${PROJECT_SOURCE_DIR}/external
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
