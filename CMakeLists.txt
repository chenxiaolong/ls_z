cmake_minimum_required(VERSION 3.6)

project(ls_z)

add_executable(ls_z ls_z.cpp)

set_target_properties(
	ls_z
	PROPERTIES
	LINK_FLAGS "-static"
	LINK_SEARCH_START_STATIC ON
)

target_compile_options(ls_z PRIVATE -std=c++11)

#if(NOT MSVC)
#    set_target_properties(
#        ls_z
#        PROPERTIES
#        CXX_STANDARD 11
#        CXX_STANDARD_REQUIRED 1
#    )
#endif()
