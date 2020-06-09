# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.


########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.13)

########################################################################
#
# First look in the install directory for package config of gnuradio
#
########################################################################
set( ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

cmake_minimum_required(VERSION 3.8)
project(gr-afsk CXX C)
#install to PyBOMBS target prefix if defined
if(DEFINED ENV{PYBOMBS_PREFIX})
    set(CMAKE_INSTALL_PREFIX $ENV{PYBOMBS_PREFIX})
    message(STATUS "PyBOMBS installed GNU Radio. Setting CMAKE_INSTALL_PREFIX to $ENV{PYBOMBS_PREFIX}")
endif()

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
#
# setup version info
#
########################################################################

set(VERSION_MAJOR 3)
set(VERSION_API   8)
set(VERSION_ABI 0)
set(VERSION_PATCH 0)
cmake_policy(SET CMP0011 NEW)
########################################################################
# Compiler specific setup
########################################################################
#if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
#    #http://gcc.gnu.org/wiki/Visibility
#    add_definitions(-fvisibility=hidden)
#  endif()
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    add_definitions(-fvisibility=hidden)
endif()

########################################################################
# Find gnuradio build dependencies
########################################################################
find_package(Gnuradio "3.8" REQUIRED runtime )
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake/Modules")
if(Gnuradio_VERSION VERSION_LESS "3.9")
    add_definitions(-DBOOST_SHARED_PTR)
endif()
include(GrVersion)
#set(GR_BOOST_MIN_VERSION "1.53")
########################################################################
# Install directories
########################################################################
include(GrPlatform) #define LIB_SUFFIX
set(GR_RUNTIME_DIR      bin)
set(GR_LIBRARY_DIR      lib${LIB_SUFFIX})
set(GR_INCLUDE_DIR      include/afsk)
set(GR_DATA_DIR         share)
set(GR_PKG_DATA_DIR     ${CMAKE_INSTALL_PREFIX}/${GR_DATA_DIR}/${CMAKE_PROJECT_NAME})
set(GR_DOC_DIR          ${GR_DATA_DIR}/doc)
set(GR_PKG_DOC_DIR      ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME})
set(GR_CONF_DIR         etc)
set(GR_PKG_CONF_DIR     ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d)
set(GR_LIBEXEC_DIR      libexec)
set(GR_PKG_LIBEXEC_DIR  ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})
set(GRC_BLOCKS_DIR      ${CMAKE_INSTALL_PREFIX}/${GR_DATA_DIR}/gnuradio/grc/blocks)
SET(GR_CMAKE_DIR        ${CMAKE_MODULES_DIR}/gr-display)
find_package(PythonLibs 3)
find_package(SWIG)

if(NOT SWIG_FOUND)
  message(FATAL_ERROR "Swig required to build fcdproplus")
else()    
  message(STATUS "Swig version ${SWIG_VERSION} found")
endif()
find_package(Doxygen)

########################################################################
# On Apple only, set install name and use rpath correctly, if not already set
########################################################################
if(APPLE)
   if(NOT CMAKE_INSTALL_NAME_DIR)
       set(CMAKE_INSTALL_NAME_DIR
           ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
           PATH "Library Install Name Destination Directory" FORCE)
   endif(NOT CMAKE_INSTALL_NAME_DIR)
   if(NOT CMAKE_INSTALL_RPATH)
       set(CMAKE_INSTALL_RPATH
           ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
           PATH "Library Install RPath" FORCE)
   endif(NOT CMAKE_INSTALL_RPATH)
   if(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
       set(CMAKE_BUILD_WITH_INSTALL_RPATH ON CACHE
           BOOL "Do Build Using Library Install RPath" FORCE)
   endif(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
endif(APPLE)
########################################################################
# Find gnuradio build dependencies
########################################################################
find_package(Doxygen)

########################################################################
# Setup doxygen option
########################################################################
if(DOXYGEN_FOUND)
        option(ENABLE_DOXYGEN "Build docs using Doxygen" ON)
else(DOXYGEN_FOUND)
        option(ENABLE_DOXYGEN "Build docs using Doxygen" OFF)
endif(DOXYGEN_FOUND)


########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)


########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/afsk)
add_subdirectory(lib)
add_subdirectory(swig)
add_subdirectory(python)
add_subdirectory(grc)
add_subdirectory(apps)
add_subdirectory(docs)
########################################################################
# Create Pkg Config File
########################################################################
# fake gnuradio cpack behavior as long as we don't use it directly
set(CPACK_PACKAGE_NAME "gnuradio-ax25")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GNU Radio block to  decode ax25 packets ")
set(CPACK_PACKAGE_VERSION ${VERSION})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-ax25.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-ax25.pc @ONLY)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-ax25.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "ax25"
 )

########################################################################
# Print Summary
########################################################################
MESSAGE(STATUS "================ Build Summary =========================")
MESSAGE(STATUS "Building gr-ax25        : ${LIBVER} for ${CMAKE_SYSTEM_NAME} ")
MESSAGE(STATUS "Building for gnuradio   : ${Gnuradio_VERSION}")
MESSAGE(STATUS "Using CMAKE Module path : ${CMAKE_MODULE_PATH}" )
MESSAGE(STATUS "gr-display INCLUDES IN  : ${GR_INCLUDE_DIR}")
MESSAGE(STATUS "Using install prefix    : ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Installing grc files to : ${GRC_BLOCKS_DIR}")
MESSAGE(STATUS "=========================================================")

