# Copyright 2011 Free Software Foundation, Inc.
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
find_package(Doxygen)
find_package(Sphinx)

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("doxygen" ENABLE_DOXYGEN DOXYGEN_FOUND)
GR_REGISTER_COMPONENT("sphinx"  ENABLE_SPHINX  SPHINX_FOUND)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_DOXYGEN)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_COMPONENT("docs"
    DISPLAY_NAME "Documentation"
    DESCRIPTION  "Doxygen generated documentation"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(doxygen)
add_subdirectory(exploring-gnuradio)

endif(ENABLE_DOXYGEN)


########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_SPHINX)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_COMPONENT("docs"
    DISPLAY_NAME "Documentation"
    DESCRIPTION  "Sphinx generated documentation"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(sphinx)

endif(ENABLE_SPHINX)
