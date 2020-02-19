# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
find_package(Doxygen)

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("doxygen" ENABLE_DOXYGEN DOXYGEN_FOUND)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_DOXYGEN)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(doxygen)

endif(ENABLE_DOXYGEN)
