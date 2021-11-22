cmake_minimum_required(VERSION 3.17)

project(Log LANGUAGES CXX DESCRIPTION "Simple logging lib in C++")
include(CustomFunctions.cmake)

set(CMAKE_CXX_STANDARD 17)

set_project_properties()

prepare_source_files(SRC_FILES)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
add_library(${PROJECT_NAME}_Shared SHARED ${SRC_FILES})

set(LIB_VERSION 1.0)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${LIB_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_Shared PROPERTIES VERSION ${LIB_VERSION})
set_target_properties(${PROJECT_NAME}_Shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS ${CMAKE_BUILD_TYPE}
        DESTINATION ${CMAKE_BUILD_TYPE}/lib/static)
install(TARGETS ${PROJECT_NAME}_Shared
        CONFIGURATIONS ${CMAKE_BUILD_TYPE}
        DESTINATION ${CMAKE_BUILD_TYPE}/lib/shared)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/inc DESTINATION ${CMAKE_BUILD_TYPE})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src DESTINATION ${CMAKE_BUILD_TYPE})
