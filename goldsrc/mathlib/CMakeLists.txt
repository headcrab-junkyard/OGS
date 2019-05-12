project(ogs-mathlib)

# Build as a shared library
add_library(${PROJECT_NAME} STATIC)

# Sources list
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)