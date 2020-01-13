project(ogs-voice)

# Build as a shared library
add_library(${PROJECT_NAME} MODULE)

# Sources list
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "voice_speex"
)

target_link_libraries(${PROJECT_NAME} ogs-tier1)