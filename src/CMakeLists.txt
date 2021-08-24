cmake_minimum_required(VERSION 3.10)

project("webkruncher")

#find_library(infokruncher REQUIRED )

IF(LINUX)
	message( THIS IS LINUX )
ENDIF(LINUX)

IF(UNIX)
	message( THIS IS UNIX )
ENDIF(UNIX)


add_subdirectory( site )
