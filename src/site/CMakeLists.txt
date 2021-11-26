

set(INCLUDE_PATHS)
IncludeProjects(
	krunchercore
	exexml
	datakruncher
	informationkruncher
	infodata
	infodata/auth
	infodata/records
	infodata/site
)
set(LIBRARY_PATHS)
set(LIBRARIES)
IncludeLibraryPaths(
	krunchercore
	datakruncher
	informationkruncher
	infodata/auth
	infodata/records
	infodata/site
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
		krunchertrace
		site
		auth
)

install(
    TARGETS
	    webkruncher
		    EXPORT exports
		    RUNTIME DESTINATION bin
) 

