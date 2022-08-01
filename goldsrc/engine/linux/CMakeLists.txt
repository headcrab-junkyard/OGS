target_sources(${PROJECT_NAME}
	PRIVATE in_linux.c
	PRIVATE snd_linux.c
)

if(NOT OGS_USE_SDL)
	target_sources(${PROJECT_NAME}
		PRIVATE sys_linuxwind.cpp
	)
endif()