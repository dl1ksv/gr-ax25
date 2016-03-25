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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
include(GrMiscUtils)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND afsk_sources
    afsk1200_impl.cc
    ax25decode_impl.cc
    aprs2inet_impl.cc )

add_library(gnuradio-afsk SHARED ${afsk_sources})
target_link_libraries(gnuradio-afsk ${Boost_LIBRARIES}  ${GNURADIO_RUNTIME_LIBRARIES})
if(APPLE)
  # for some reason on OSX, linking to gnuradio-runtime also requires
  # linking with gnuradio-pmt; this does not seem necessary on
  # Linux.
  target_link_libraries(gnuradio-afsk ${GNURADIO_PMT_LIBRARIES})
  # set install_name self-id correctly
  SET_TARGET_PROPERTIES(gnuradio-afsk PROPERTIES INSTALL_NAME_DIR
				"${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR}"
  )
endif()

set_target_properties(gnuradio-afsk PROPERTIES DEFINE_SYMBOL "gnuradio_afsk_EXPORTS")
GR_LIBRARY_FOO(gnuradio-afsk)

