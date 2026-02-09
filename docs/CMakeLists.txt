# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
gr_find_package(Doxygen)
gr_find_package(MathJax2)

########################################################################
# Register components
########################################################################
include(GrComponent)
gr_register_component(
    NAME "doxygen-documentation"
    VAR ENABLE_DOXYGEN
    REQUIRED_TPLS DOXYGEN_FOUND
)
gr_register_component(
    NAME "man-pages"
    VAR ENABLE_MANPAGES
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_DOXYGEN)
    add_subdirectory(doxygen)
endif(ENABLE_DOXYGEN)

if(ENABLE_MANPAGES)
    add_subdirectory(man)
endif(ENABLE_MANPAGES)
