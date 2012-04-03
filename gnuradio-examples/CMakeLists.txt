# Copyright 2012 Free Software Foundation, Inc.
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

include(GrPython) #used for code generation

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gnuradio-examples" ENABLE_GR_EXAMPLES
    ENABLE_GRUEL
    ENABLE_VOLK
    ENABLE_GR_CORE
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_EXAMPLES)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_EXAMPLE_DESCRIPTION "GNU Radio Examples")

CPACK_COMPONENT("gnuradio_examples"
    GROUP        "Examples"
    DISPLAY_NAME "GNU Radio Examples"
    DESCRIPTION  "GNU Radio Examples"
    DEPENDS      "core_runtime,core_python,gruel_runtime,gruel_python"
)

########################################################################
# Add subdirectories
########################################################################
if(ENABLE_PYTHON)
    add_subdirectory(grc)
    add_subdirectory(python)
endif(ENABLE_PYTHON)

endif(ENABLE_GR_EXAMPLES)
