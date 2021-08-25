
set(CMAKE_CXX_FLAGS " -std=c++11 -pthread " )

get_filename_component(INCLUDES_PATH ../include/ REALPATH) 

add_executable(
	testsite
		main.cpp
)

#add_library( kruncher::testsite  ALIAS testsite  )

target_include_directories(
    testsite
	    PRIVATE
		${INCLUDE_PATHS}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	testsite
		PUBLIC
			${LIBRARY_PATHS}
)

target_link_libraries(
    testsite
	    PUBLIC
			${LIBRARIES}
)

install(
    TARGETS
	    testsite
		    EXPORT exports
		    RUNTIME DESTINATION bin
) 

