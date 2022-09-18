cmake_minimum_required(VERSION 3.6)

project(EZPlayer LANGUAGES CXX)

option(DISABLE_OPTION OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

IF(DISABLE_OPTION)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Od")
ENDIF()

set(SOFT_NAME EZPlayer)

message(STATUS "=============== ${PROJECT_NAME} toolchain ====================")
message(STATUS "C compiler:${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler:${CMAKE_CXX_COMPILER}")
message(STATUS "C++ linker:${CMAKE_LINKER}")
message(STATUS "C++ ar:${CMAKE_CXX_COMPILER_AR}")
message(STATUS "C++ ranlib:${CMAKE_CXX_COMPILER_RANLIB}")
message(STATUS "Using std=c${CMAKE_C_STANDARD}")
message(STATUS "Using std=c++${CMAKE_CXX_STANDARD}")
message(STATUS "cxxflags:${CMAKE_CXX_LINK_FLAGS}")
message(STATUS "=============== ${PROJECT_NAME} build config ====================")
message(STATUS "Build type:${CMAKE_BUILD_TYPE}")
message(STATUS "PROJECT_SOURCE_DIR:${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_PREFIX_PATH:${CMAKE_PREFIX_PATH}")


IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_definitions(-D__windows__)
	include(config_version.cmake)
ELSEIF(${CMAKE_SYSTEM_NAME} STREQUAL "Drawin")
	add_definitions(-D__mac__)
ENDIF()

add_subdirectory(${PROJECT_SOURCE_DIR}/../../Launcher Launcher)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../Utils Utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../Log Log)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../Gui Gui)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../Core Core)

if (MSVC)
	# Set the startup project .
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${SOFT_NAME})
endif (MSVC)


