
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
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	webkruncher
		PUBLIC
			/usr/local/lib/infokruncher/
)

target_link_libraries(
    webkruncher
	    PUBLIC
		ssl	
		crypto	
		informationkruncher
)

install(
    TARGETS
	    webkruncher
		    EXPORT exports
		    RUNTIME DESTINATION bin
) 

