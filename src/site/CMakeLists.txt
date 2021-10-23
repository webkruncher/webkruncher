


set(INCLUDE_PATHS)
IncludeProjects(
	krunchercore/src/include
	exexml/src/include
	datakruncher/src/include
	informationkruncher/src/include
	informationkruncher/src/include/db
)
set(LIBRARY_PATHS)
set(LIBRARIES)
IncludeLibraryPaths(
	krunchercore
	informationkruncher
)


add_executable(
	webkruncher
		main.cpp
		webkruncher.cpp
)


target_include_directories(
    webkruncher
	    PRIVATE
		${INCLUDE_PATHS}
		../include/
		/home/jmt/libdb/build_unix
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
		ssl
		crypto
		/home/jmt/libdb/build_unix/libdb_cxx.a
)

install(
    TARGETS
	    webkruncher
		    EXPORT exports
		    RUNTIME DESTINATION bin
) 

