project(ogs-gameui-steamera)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

add_library(${PROJECT_NAME} MODULE)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "gameui"
)

target_link_libraries(${PROJECT_NAME} ogs-tier1)