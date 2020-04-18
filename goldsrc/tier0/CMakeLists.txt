project(ogs-tier0)

# Build as a shared library
add_library(${PROJECT_NAME} MODULE) # TODO: SHARED?

# Sources list
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "tier0"
)

#target_link_libraries(${PROJECT_NAME} ogs-tier1)