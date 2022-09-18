cmake_minimum_required(VERSION 3.6)

project(EZLog LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} SHARED)
add_definitions(-DEZLOG_EXPORT_API)

file(GLOB_RECURSE Src ${PROJECT_SOURCE_DIR}/*.c ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.cc ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*.rc)
SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} FILES ${Src})

include_directories(${PROJECT_SOURCE_DIR}/../3rd)

target_sources(${PROJECT_NAME}
	PRIVATE
	${Src}
)

#version
set(TARGETFILENAME ${EXEC_NAME})
configure_file(
	${NOICO_RC_TEMPLETE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/Version/version.rc
)



