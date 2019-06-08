project(ogs-vgui2)

#find_package(CEGUI)

# hard coding CEGUI links for now
#set(CEGUI_INCLUDE_DIR "C:/CEGUI-SDK/include")
#set(CEGUI_LIBRARIES "C:/CEGUI-SDK/lib/libCEGUICommonDialogs-0_Static_d.a;C:/CEGUI-SDK/lib/libCEGUICoreWindowRendererSet_Static_d.a;C:/CEGUI-SDK/lib/libCEGUIBase-0_Static_d.a")
#set(CEGUI_OpenGLRenderer_LIBRARIES "C:/CEGUI-SDK/lib/libCEGUIOpenGLRenderer-0_Static_d.a")

#if(${CEGUI_FOUND})
	#include_directories(${CEGUI_INCLUDE_DIR})
#endif()

# Build as dynamic library
add_library(${PROJECT_NAME} MODULE)

# Build a list of sources
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "vgui2"
)

target_link_libraries(${PROJECT_NAME} ogs-tier1)
#target_link_libraries(${PROJECT_NAME} ${CEGUI_LIBRARIES} ${CEGUI_OpenGLRenderer_LIBRARIES} -lopengl32)