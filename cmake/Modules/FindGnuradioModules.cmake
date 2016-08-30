# - Try to find the cmake/Modules directory of an installed gnuradio
# - This might be not the standard location
# - Look for GrVersion.cmake

find_path(GNURADIO_MODULE_DIRECTORY
	NAMES GrVersion.cmake
	HINTS ${CMAKE_INSTALL_PREFIX}/lib/cmake/gnuradio ${CMAKE_INSTALL_PREFIX}/lib64/cmake/gnuradio
	PATHS
          /usr/lib64/cmake/gnuradio
          /usr/lib/cmake/gnuradio
	  /usr/lib/arm-linux-gnueabihf/cmake/gnuradio
)
