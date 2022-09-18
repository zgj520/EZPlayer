if(NOT DEFINED DST_MODULE_NAME)
	message(FATAL_ERROR "please defined DST_MODULE_NAME")
endif()

project(${DST_MODULE_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 必须在生成target前
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(${DST_MODULE_TYPE} STREQUAL "Dynamic")
	add_library(${PROJECT_NAME} SHARED)
	set(PRODUCT_FULL_NAME ${DST_MODULE_NAME}.dll)
elseif(${DST_MODULE_TYPE} STREQUAL "Static")
	add_library(${PROJECT_NAME} STATIC)
	set(PRODUCT_FULL_NAME ${DST_MODULE_NAME}.lib)
elseif(${DST_MODULE_TYPE} STREQUAL "Executable")
	add_executable(${PROJECT_NAME})
	set(PRODUCT_FULL_NAME ${DST_MODULE_NAME}.exe)
endif()

#======================= Qt setting ==============================
if(NOT DEFINED CMAKE_PREFIX_PATH)
	message(FATAL_ERROR "please input CMAKE_PREFIX_PATH point qt path")
endif()
find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED
	COMPONENTS
	Core
	Gui
	Widgets
	Quick
	Qml
)
message(STATUS "QT_VERSION_MAJOR:${QT_VERSION_MAJOR}")
target_link_libraries(${DST_MODULE_NAME} 
	PUBLIC
	Qt6Widgets
	Qt6Core
	Qt6Gui
	Qt6Quick
	Qt6Qml
)
set(QtPath ${CMAKE_PREFIX_PATH})
target_link_directories(${DST_MODULE_NAME} PUBLIC ${QtPath}/lib)
include_directories(${QtPath}/include ${QtPath}/include/QtCore ${QtPath}/include/QtGui ${QtPath}/include/QtWidgets ${QtPath}/include/QtQml)

set(WINQT_DEPLOY_TOOL "${QtPath}\\bin\\windeployqt.exe")
set(QT_DEPLOT_BASE_PARAMS "--no-compiler-runtime --no-translations --plugindir=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\\${CMAKE_BUILD_TYPE}\\plugins\"")

set(QT_DEPLOT_PARAMS ${QT_DEPLOT_BASE_PARAMS})
list(APPEND QT_DEPLOT_PARAMS "--qmldir=\"${PROJECT_SOURCE_DIR}\"")

add_custom_command(
	TARGET ${MODULE_NAME} POST_BUILD
	COMMENT "instll qt dependens"
	COMMAND ${WINQT_DEPLOY_TOOL} ${QT_DEPLOT_PARAMS} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\\${CMAKE_BUILD_TYPE}\\${PRODUCT_FULL_NAME}"
	)
