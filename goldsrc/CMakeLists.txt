set(OGS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/../ogs-sdk)

add_subdirectory(filesystem_stdcrabio)

#add_subdirectory(r_gl)
#add_subdirectory(r_soft)

add_subdirectory(engine)

add_subdirectory(dedicated)
add_subdirectory(launcher)