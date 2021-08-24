cmake_minimum_required(VERSION 3.10)

project("webkruncher")

#find_library(infokruncher REQUIRED )

#IF(DEFINED ENV{LINUX})
#	message( THIS IS LINUX )
#ENDIF()
#
#
#IF(DEFINED ENV{LINUX})
#	message( THIS IS LINUX )
#ENDIF()
#

message( SYSTEM: ${CMAKE_SYSTEM_NAME} ) 

IF(CMAKE_SYSTEM_NAME STREQUAL OpenBSD)
	message( THIS IS UNIX )
ENDIF()

IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
	message( THIS IS LINUX )
ENDIF()


add_subdirectory( site )
