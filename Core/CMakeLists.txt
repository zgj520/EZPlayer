cmake_minimum_required(VERSION 3.6)

project(Core LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} SHARED)
add_definitions(-DCORE_EXPORT_API)

file(GLOB_RECURSE Src ${PROJECT_SOURCE_DIR}/*.c ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.cc ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*.rc)
SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} FILES ${Src})

target_sources(${PROJECT_NAME}
	PRIVATE
	${Src}
)

#version
set(TARGETFILENAME ${PROJECT_NAME})
configure_file(
	${NOICO_RC_TEMPLETE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/Version/version.rc
)

add_definitions(-DEGL_EGLEXT_PROTOTYPES)
add_definitions(-DGL_GLEXT_PROTOTYPES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src/Render/Shader)


#====================== angle setting ============================
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../3rd/angle/libs/${CMAKE_BUILD_TYPE})
target_link_libraries(${PROJECT_NAME} PUBLIC libEGL.dll.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC libGLESv2.dll.lib)
include_directories(${PROJECT_SOURCE_DIR}/../3rd/angle/include)
include_directories(${PROJECT_SOURCE_DIR}/Src)


#====================== ffmpeg setting ============================
target_link_libraries(${PROJECT_NAME} 
	PUBLIC
	avcodec
	avformat
	avutil
	swscale
)
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../3rd/ffmpeg/x64/lib)
include_directories(${PROJECT_SOURCE_DIR}/../3rd/ffmpeg/x64/include)

#====================== ezlog setting ============================
include_directories(${PROJECT_SOURCE_DIR}/../EZlog/Src)
target_link_libraries(${PROJECT_NAME} PUBLIC EZLog)


#====================== ezutils setting ============================
target_link_libraries(${PROJECT_NAME} PUBLIC EZUtils)
include_directories(${PROJECT_SOURCE_DIR}/../Utils/Src)

#====================== DirectXTK setting ============================
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../3rd/DirectXTK/lib/Release/x64)
target_link_libraries(${PROJECT_NAME} PUBLIC DirectXTK)
include_directories(${PROJECT_SOURCE_DIR}/../3rd/DirectXTK/include)