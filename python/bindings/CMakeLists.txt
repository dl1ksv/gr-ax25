# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND ax25_python_files
    afsk1200_python.cc
    aprs2inet_python.cc
    ax25decode_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(ax25 
   ../..
   gr::ax25
   "${ax25_python_files}")

install(TARGETS ax25_python DESTINATION ${GR_PYTHON_DIR}/ax25 COMPONENT pythonapi)
