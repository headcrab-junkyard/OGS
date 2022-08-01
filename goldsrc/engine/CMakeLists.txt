project(ogs-engine)

option(OGS_ENGINE_HW "Build the engine module with hardware renderer (OpenGL) support, it will be using software renderer otherwise" ON)

add_subdirectory(render) # TODO: renderer

add_library(${PROJECT_NAME} MODULE)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

if(OGS_USE_SDL)
	add_subdirectory(sdl2)
	
	# TODO: input and sound impls still should be taken from the platform-specific sources
else()
	if(WIN32)
		add_subdirectory(win)
	elseif(UNIX)
		add_subdirectory(linux)
	else()
		add_subdirectory(null)
	endif()
endif()

if(OGS_USE_IMGUI)
	add_subdirectory(imgui)
endif()

target_link_libraries(${PROJECT_NAME} ogs-tier1 ogs-mathlib)

if(NOT OGS_DEDICATED)
	if(OGS_ENGINE_HW)
		target_compile_definitions(${PROJECT_NAME} PRIVATE GLQUAKE) # TODO: make it required only for gl renderer
		
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "hw"
		)
		
		target_link_libraries(${PROJECT_NAME} ogs-renderer-gl)
	else()
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "sw"
		)
		
		target_link_libraries(${PROJECT_NAME} ogs-renderer-soft)
	endif()
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE SWDS)
	
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "swds"
	)
	
	target_link_libraries(${PROJECT_NAME} ogs-renderer-soft)
endif()