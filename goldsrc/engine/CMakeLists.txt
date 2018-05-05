project(gskiller-engine)

add_definitions(-DGLQUAKE) # TODO

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/null/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/null/*.cpp
)

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ogs-interface)

# TODO:
# * Evol/Vanilla presets support
# * Render DLL for Evol/Render Static Lib for Vanilla

# if(OGS_EVOL)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "engine"
	)
# else()
	# if(NOT OGS_DEDICATED)
		# if(OGS_ENGINE_HW)
			# set_target_properties(${PROJECT_NAME} PROPERTIES
				# OUTPUT_NAME "hw"
			# )
			
			# target_link_libraries(${PROJECT_NAME} gskiller-render-gl)
		# else()
			# set_target_properties(${PROJECT_NAME} PROPERTIES
				# OUTPUT_NAME "sw"
			# )
			
			# target_link_libraries(${PROJECT_NAME} gskiller-render-soft)
		# endif()
	# else()
		# set_target_properties(${PROJECT_NAME} PROPERTIES
			# OUTPUT_NAME "swds"
		# )
		
		# target_link_libraries(${PROJECT_NAME} gskiller-render-soft)
	# endif()
# endif()