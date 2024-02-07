cmake_minimum_required(VERSION 3.5)

project(CaliLite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build/install)

add_library(
	${PROJECT_NAME}
	STATIC
	"mylib.cpp"
)

install(TARGETS ${PROJECT_NAME} DESTINATION .)

