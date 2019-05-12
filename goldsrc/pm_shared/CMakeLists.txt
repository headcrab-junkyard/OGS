project(ogs-pm_shared)

add_library(${PROJECT_NAME} STATIC)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)