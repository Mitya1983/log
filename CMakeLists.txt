cmake_minimum_required(VERSION 3.17)

project(Log LANGUAGES CXX DESCRIPTION "Simple logging lib in C++")
include(CustomFunctions.cmake)

set(CMAKE_CXX_STANDARD 17)

set_project_properties()

prepare_source_files(SRC_FILES)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        /home/mitya/MyLibs/DateTime/${CMAKE_BUILD_TYPE}/inc
        )

target_link_directories(${PROJECT_NAME} PRIVATE
        /home/mitya/MyLibs/DateTime/${CMAKE_BUILD_TYPE}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        -lDateTime
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Log")

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS ${CMAKE_BUILD_TYPE}
        DESTINATION ${CMAKE_BUILD_TYPE}/lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/inc DESTINATION ${CMAKE_BUILD_TYPE})
