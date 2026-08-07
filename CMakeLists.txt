cmake_minimum_required(VERSION 3.16)

project(fstl-view LANGUAGES C)
set(PROJECT_HOME ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Force verbose output" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(SOURCES
	${PROJECT_HOME}/src/main.c
	${PROJECT_HOME}/src/abstrShader.c
	${PROJECT_HOME}/src/render.c
	${PROJECT_HOME}/src/stlRead.c
	${PROJECT_HOME}/src/input.c
)

find_package(glfw3 3.3 REQUIRED)
add_subdirectory(${PROJECT_HOME}/glad)

set(INCLUDES
	${PROJECT_HOME}/include
	${PROJECT_HOME}/cglm/include
	${PROJECT_HOME}/glad/include
)

set(LIBRARIES
	glad
	glfw
    m
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
