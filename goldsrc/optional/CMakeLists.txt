if(NOT MINGW) # Only buildable by MSVC compiler
	add_subdirectory(vguiwrap)
endif()