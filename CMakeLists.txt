cmake_minimum_required(VERSION 3.16)
project(Vanadium)

if(NOT CMAKE_BUILD_TYPE)
	set(default_build_type "Debug")
	message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
	set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Chooce the build type." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
	"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Vanadium Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for Vanadium
set(Vanadium_Version_Major 0)
set(Vanadium_Version_Minor 1)
set(Vanadium_Version_Patch 0)
set(Vanadium_Version_Status "-dev")
set(PROJECT_VERSION
	"${Vanadium_Version_Major}.${Vanadium_Version_Minor}.${Vanadium_Version_Patch}${Vanadium_Version_Status}"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(
	vanadium
	${SOURCES}
)

target_include_directories(
	vanadium
	PUBLIC ${PROJECT_SOURCE_DIR}/include
)
