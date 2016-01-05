cmake_minimum_required(VERSION 2.8)
project(kodtest)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_CXX_FLAGS "-std=c++14")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(
	${PROJECT_NAME}
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	${SOURCES}
	${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
	curl
)
