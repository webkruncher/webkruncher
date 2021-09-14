cmake_minimum_required(VERSION 3.10)

project("webkruncher")


add_compile_options(-ggdb)

set(
	NIX_INCLUDE_PATHS
		/usr/local/include/infokruncher/
		/usr/local/include/infokruncher/db/
		/usr/local/include/infokruncher/db/auth/
		/usr/local/include/exexml/
		/usr/include/uuid/
		../include
)

set(
	NIX_LIBRARY_PATHS
		/usr/local/lib/infokruncher/
)

set(
	NIX_LIBRARIES
			informationkruncher
			auth
			records
			site
			ssl	
			crypto	
)

IF(CMAKE_SYSTEM_NAME STREQUAL OpenBSD)
	add_definitions( -DUNIX )
	set(
		INCLUDE_PATHS
			${NIX_INCLUDE_PATHS}
	)
	set(
		LIBRARY_PATHS
			${NIX_LIBRARY_PATHS}
	)
	set(
		LIBRARIES
			${NIX_LIBRARIES}
	)
ENDIF()

IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
	add_definitions( -DLINUX )
	set(
		INCLUDE_PATHS
			${NIX_INCLUDE_PATHS}
	)
	set(
		LIBRARY_PATHS
			${NIX_LIBRARY_PATHS}
			/usr/lib/x86_64-linux-gnu/
	)
	set(
		LIBRARIES
			${NIX_LIBRARIES}
			uuid
	)
ENDIF()


set(CMAKE_CXX_COMPILER "g++")
add_subdirectory( include )
add_subdirectory( site )


