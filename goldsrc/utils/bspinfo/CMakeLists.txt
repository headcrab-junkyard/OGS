project(ogs-bspinfo)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bspinfo.c
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common/cmdlib.c
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common/bspfile.c
)

if(WIN32)
target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32)
endif()

# TODO: define _DEBUG for debug builds
# TODO: define NDEBUG for release builds

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "bspinfo"
)