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
# Setup dependencies
########################################################################
include(GrPython)

GR_PYTHON_CHECK_MODULE("Mako >= ${GR_MAKO_MIN_VERSION}" mako "mako.__version__ >= '${GR_MAKO_MIN_VERSION}'" MAKO_FOUND)

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(utils_python_deps
        MAKO_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT("gr-utils" ENABLE_GR_UTILS
    ENABLE_GNURADIO_RUNTIME
    ENABLE_PYTHON
    ${utils_python_deps}
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_UTILS)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(python/utils)
add_subdirectory(python/modtool)

endif(ENABLE_GR_UTILS)
