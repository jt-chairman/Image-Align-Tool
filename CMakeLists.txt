cmake_minimum_required(VERSION 3.5)

project(CaliLite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)


set(PUBLIC_HEADERS
	mylib.h
)

set(SOURCE
	mylib.cpp
)

add_library(
	${PROJECT_NAME}
	STATIC
	${SOURCE}
	${PUBLIC_HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

install(
	TARGETS
	${PROJECT_NAME}
	DESTINATION ./lib
	PUBLIC_HEADER DESTINATION ./include
)

