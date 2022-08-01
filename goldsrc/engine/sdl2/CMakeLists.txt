aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PLATFORM_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PLATFORM_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})