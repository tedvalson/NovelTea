cmake_minimum_required(VERSION 3.21)

project("NovelTea")
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
set(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Choose whether SFML is linked statically or not.")
option(BUILD_DOCS "Build doxygen documentation" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_EDITOR "Build NovelTea Qt5 editor" ON)
option(BUILD_ANDROID "Build Android app" OFF)
option(ENABLE_COVERAGE "Enabled Coverage Flags" OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if (NOT ANDROID)
	# Find SFML
	find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

	# Output an error if SFML wasn't found
	if(NOT SFML_FOUND)
		message("\n-> SFML libraries were not found. Set SFML_DIR to SFML's CMake Config path (generally located in \"lib/cmake/SFML\").")
		message("-> Make sure the SFML libraries with the same configuration (Release/Debug, Static/Dynamic) exist.\n")
	endif()
else()
    # let the user switch ABIs
    set(ANDROID_ABI "armeabi-v7a" CACHE STRING "Look at the NDK docs for currently supported ABIs")
    # we install libs in a subdirectory named after the ABI (lib/mips/*.so)
    set(LIB_SUFFIX "/${ANDROID_ABI}")
	set(CMAKE_INSTALL_PREFIX ${CMAKE_ANDROID_NDK}/sources/noveltea)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	add_definitions(-DANDROID)
	include_directories(
		${CMAKE_ANDROID_NDK}/sources/third_party/sfml/include
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
set(CMAKE_CXX_STANDARD 14)

# rpath stuff
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# set the output directory for libraries and binaries
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# compile flags
set(NT_COVERAGE_FLAGS "--coverage")
set(NT_EDITOR_FLAGS "-g -O3")

add_subdirectory(src)

if(BUILD_DOCS)
	add_subdirectory(doc)
endif()
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
