cmake_minimum_required(VERSION 3.6)

project(Gui LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(QT_PROJ_TEMPLATE ${PROJECT_SOURCE_DIR}/qt_project_template.cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/Framework Framework)
add_subdirectory(${PROJECT_SOURCE_DIR}/GUIKit GUIKit)

set_property(TARGET Framework PROPERTY FOLDER "Gui")
set_property(TARGET GUIKit PROPERTY FOLDER "Gui")


