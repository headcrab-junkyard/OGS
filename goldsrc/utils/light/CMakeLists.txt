project(ogs-light)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../trilib.c
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bspfile.c
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../mathlib.c
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cmdlib.c
)

if(WIN32)
target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32)
endif()

# TODO: define _DEBUG for debug builds
# TODO: define NDEBUG for release builds

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "light"
)