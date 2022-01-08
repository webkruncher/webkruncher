
Banner()


Headers(
	krunchercore
	krunchercore::tracer
	exexml
	datakruncher
	informationkruncher
	infodata
	infodata::db
	infodata::db/auth
	infodata::db/records
	infodata::db/site
)


LibraryPaths(
	krunchercore::global/tracer/
	krunchercore::global/
	krunchercore::ut/Directory/
	krunchercore::ut/
	datakruncher::data/
	informationkruncher::socks/
	infodata::db/auth/
	infodata::db/records/
	infodata::db/site/
)


Libraries(
	krunchertrace
	krunchercore
	datafactory
	datakruncher
	informationkruncher
	auth
	records
	site
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
		ssl
		crypto
		/home/jmt/libdb/build_unix/libdb_cxx.a
		${LIBRARIES}
)

install(
    TARGETS
	    webkruncher
		    EXPORT exports
		    RUNTIME DESTINATION bin
) 

