project(ogs-filesystem)

add_library(${PROJECT_NAME} MODULE)

target_sources(${PROJECT_NAME}
	PRIVATE FileSystem.cpp
)

if(WIN32)
	target_sources(${PROJECT_NAME}
		PRIVATE FileSystemWin.cpp
	)
elseif(UNIX)
	target_sources(${PROJECT_NAME}
		PRIVATE FileSystemPosix.cpp
	)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "filesystem_stdio"
)

target_link_libraries(${PROJECT_NAME} ogs-tier1)