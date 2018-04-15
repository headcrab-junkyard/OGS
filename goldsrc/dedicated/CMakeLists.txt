project(gskiller-dedicated)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "dedicated"
)

target_link_libraries(${PROJECT_NAME} ogs-interface)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} -ldl)
endif()