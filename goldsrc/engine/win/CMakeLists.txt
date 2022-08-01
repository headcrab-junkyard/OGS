target_sources(${PROJECT_NAME}
	PRIVATE cd_win.cpp
	PRIVATE in_win.c
	PRIVATE snd_win.c
)

if(NOT OGS_USE_SDL)
	target_sources(${PROJECT_NAME}
		PRIVATE sys_mainwind.cpp
	)
endif()

# NOTE: looks like wsock32 is loaded dynamically
# ws2_32
target_link_libraries(${PROJECT_NAME} wsock32 winmm dxguid)