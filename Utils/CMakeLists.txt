cmake_minimum_required(VERSION 3.6)

project(EZUtils LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME})

file(GLOB_RECURSE utils ${PROJECT_SOURCE_DIR}/Src/*.c ${PROJECT_SOURCE_DIR}/Src/*.cpp ${PROJECT_SOURCE_DIR}/Src/*.cc ${PROJECT_SOURCE_DIR}/Src/*.h)
SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} FILES ${utils})

include_directories(${PROJECT_SOURCE_DIR}/../3rd)

target_sources(${PROJECT_NAME}
	PRIVATE
	${utils}
)



