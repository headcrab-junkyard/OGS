target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_qgl.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${OGS_IMGUI_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${OGS_IMGUI_PATH}/examples)
target_link_directories(${PROJECT_NAME} PRIVATE ${OGS_IMGUI_PATH}/build)

target_link_libraries(${PROJECT_NAME} imgui)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} XInput1_3)
endif()