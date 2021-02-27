project(ogs-engine)

option(OGS_ENGINE_HW "Build the engine module with hardware render (OpenGL) support, it will be using software render otherwise" ON)

add_subdirectory(render)

add_library(${PROJECT_NAME} MODULE)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

if(OGS_USE_SDL)
	set(PLATFORM_NAME sdl2)
	# TODO: cd and sound impls still should be taken from the platform-specific sources
else()
	if(WIN32)
		set(PLATFORM_NAME win)
	elseif(UNIX)
		set(PLATFORM_NAME linux)
	else()
		set(PLATFORM_NAME null)
	endif()
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME} PLATFORM_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PLATFORM_SOURCES}
)

if(OGS_USE_IMGUI)
	target_sources(${PROJECT_NAME}
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl2.cpp
	)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(OGS_USE_SDL)
	target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

if(OGS_USE_IMGUI)
	target_include_directories(${PROJECT_NAME} PRIVATE ${OGS_IMGUI_PATH})
	target_include_directories(${PROJECT_NAME} PRIVATE ${OGS_IMGUI_PATH}/examples)
	target_link_directories(${PROJECT_NAME} PRIVATE ${OGS_IMGUI_PATH}/build)
endif()

if(OGS_ENGINE_HW)
	target_compile_definitions(${PROJECT_NAME} PRIVATE GLQUAKE) # TODO
endif()

if(OGS_DEDICATED)
	target_compile_definitions(${PROJECT_NAME} PRIVATE SWDS)
endif()

target_link_libraries(${PROJECT_NAME} ogs-tier1 ogs-mathlib)

if(OGS_USE_IMGUI)
	target_link_libraries(${PROJECT_NAME} imgui)
	if(WIN32)
		target_link_libraries(${PROJECT_NAME} XInput1_3)
	endif()
endif()

if(WIN32)
	# NOTE: looks like wsock32 is loaded dynamically
	# ws2_32
	target_link_libraries(${PROJECT_NAME} wsock32 winmm dxguid)
endif()

if(OGS_USE_SDL)
	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
endif()

if(NOT OGS_DEDICATED)
	if(OGS_ENGINE_HW)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "hw"
		)
		
		target_link_libraries(${PROJECT_NAME} ogs-render-gl)
	else()
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "sw"
		)
		
		target_link_libraries(${PROJECT_NAME} ogs-render-soft)
	endif()
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "swds"
	)
	
	target_link_libraries(${PROJECT_NAME} ogs-render-soft)
endif()