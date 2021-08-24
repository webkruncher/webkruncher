cmake_minimum_required(VERSION 3.10)

project("webkruncher")

#find_library(infokruncher REQUIRED )

message( SYSTEM: ${CMAKE_SYSTEM_NAME} ) 

IF(CMAKE_SYSTEM_NAME STREQUAL OpenBSD)
	message( "THIS IS UNIX" )
	add_definitions( -DUNIX )
	set(
		INCLUDE_PATHS
			/usr/local/include/infokruncher/
			/usr/include/uuid/
	)
	set(
		LIBRARY_PATHS
			/usr/local/lib/infokruncher/
	)
	set(
		LIBRARIES
			informationkruncher
			ssl	
			crypto	
	)
ENDIF()

IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
	message( "THIS IS Linux" )
	add_definitions( -DLINUX )
ENDIF()


add_subdirectory( site )
