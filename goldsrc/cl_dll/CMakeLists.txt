project(ogs-cl_dll)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "client"
)

target_link_libraries(${PROJECT_NAME} ogs-interface ogs-pm_shared ogs-mathlib)