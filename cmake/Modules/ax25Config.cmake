INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_AX25 ax25)

FIND_PATH(
    AX25_INCLUDE_DIRS
    NAMES ax25/api.h
    HINTS $ENV{AX25_DIR}/include
        ${PC_AX25_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    AX25_LIBRARIES
    NAMES gnuradio-ax25
    HINTS $ENV{AX25_DIR}/lib
        ${PC_AX25_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/ax25Target.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AX25 DEFAULT_MSG AX25_LIBRARIES AX25_INCLUDE_DIRS)
MARK_AS_ADVANCED(AX25_LIBRARIES AX25_INCLUDE_DIRS)
