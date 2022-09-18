cmake_minimum_required(VERSION 3.6)

project(Launcher LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EXEC_NAME ${SOFT_NAME})

add_executable(${EXEC_NAME})

if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${VCMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:wmainCRTStartup")
	add_definitions(-DUNICODE -D_UNICODE)
endif()

target_link_directories(${EXEC_NAME} PUBLIC ${PROJECT_BINARY_DIR}/bin)
target_link_libraries(${EXEC_NAME} PUBLIC EZLog)
target_link_libraries(${EXEC_NAME} PUBLIC EZUtils)
include_directories(${PROJECT_SOURCE_DIR}/../Utils/Src)
include_directories(${PROJECT_SOURCE_DIR}/../EZlog/Src)

set(All_Src 
${PROJECT_SOURCE_DIR}/Src/main.cpp
)

SOURCE_GROUP("Src" FILES ${All_Src})

target_sources(${EXEC_NAME}
	PRIVATE
	${All_Src}
	Version/version.rc
)

#version
set(TARGETFILENAME ${EXEC_NAME})
configure_file(
	${WITHICO_RC_TEMPLETE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/Version/version.rc
)




