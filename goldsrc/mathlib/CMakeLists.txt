project(ogs-mathlib)

# Sources list
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Build as a shared library
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})