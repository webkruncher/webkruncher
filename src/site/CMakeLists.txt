
set(CMAKE_CXX_FLAGS " -std=c++11 -pthread " )

get_filename_component(INCLUDES_PATH ../include/ REALPATH) 

add_executable(
	webkruncher
		site.cpp
)

#add_library( kruncher::webkruncher  ALIAS webkruncher  )

target_include_directories(
    webkruncher
	    PRIVATE
		/usr/local/include/infokruncher/
		/usr/include/uuid/
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	webkruncher
		PUBLIC
			${LIBRARY_PATHS}
)

target_link_libraries(
    webkruncher
	    PUBLIC
			${LIBRARIES}
)

install(
    TARGETS
	    webkruncher
		    EXPORT exports
		    RUNTIME DESTINATION bin
) 

