cmake_minimum_required(VERSION 3.6)

set(DST_MODULE_TYPE "Dynamic")
set(MODULE_NAME Framework)
set(DST_MODULE_NAME ${MODULE_NAME})
include(${QT_PROJ_TEMPLATE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DFRAMEWORK_EXPORT_API)

file(GLOB_RECURSE Src ${PROJECT_SOURCE_DIR}/*.c ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.cc ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*.qml ${PROJECT_SOURCE_DIR}/*.rc)
SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} FILES ${Src})

target_sources(${PROJECT_NAME}
	PRIVATE
	${Src}
	Framework.qrc
)

target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/bin)
target_link_libraries(${PROJECT_NAME} PUBLIC GUIKit)
target_link_libraries(${PROJECT_NAME} PUBLIC EZLog)
include_directories(${PROJECT_SOURCE_DIR}/../GUIKit)
include_directories(${PROJECT_SOURCE_DIR}/../../Log/Src)

target_link_libraries(${PROJECT_NAME} PUBLIC Core)
include_directories(${PROJECT_SOURCE_DIR}/../../Core/Interface)

#version
set(TARGETFILENAME ${PROJECT_NAME})
configure_file(
	${NOICO_RC_TEMPLETE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/Version/version.rc
)



