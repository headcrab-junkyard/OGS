project(ogs-qbsp)

add_executable(${PROJECT_NAME} bspinfo.c)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "qbsp"
)