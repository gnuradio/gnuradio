# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrPython)

GR_PYTHON_CHECK_MODULE("Mako >= ${GR_MAKO_MIN_VERSION}" mako "mako.__version__ >= '${GR_MAKO_MIN_VERSION}'" MAKO_FOUND)

GR_PYTHON_CHECK_MODULE_RAW(
  "click"
  "import click"
  CLICK_FOUND
  )

GR_PYTHON_CHECK_MODULE_RAW(
  "click-plugins"
  "import click_plugins"
  CLICK_PLUGINS_FOUND
  )

 GR_PYTHON_CHECK_MODULE_RAW(
   "pygccxml"
   "import pygccxml"
   PYGCCXML_FOUND
   )

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(utils_python_deps
        MAKO_FOUND
    )

    set(utils_modtool_deps
      CLICK_FOUND
      CLICK_PLUGINS_FOUND
    )

    set(utils_blocktool_deps
        PYGCCXML_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT("gr-utils" ENABLE_GR_UTILS
    ENABLE_GNURADIO_RUNTIME
    ENABLE_PYTHON
    ${utils_python_deps}
)

GR_REGISTER_COMPONENT("gr_modtool" ENABLE_GR_MODTOOL
    ENABLE_GNURADIO_RUNTIME
    ENABLE_PYTHON
    ${utils_modtool_deps}
)

 GR_REGISTER_COMPONENT("gr_blocktool" ENABLE_GR_BLOCKTOOL
     ENABLE_GNURADIO_RUNTIME
     ENABLE_PYTHON
     ${utils_blocktool_deps}
 )


########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_UTILS)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(python/utils)

if(ENABLE_GR_BLOCKTOOL)
add_subdirectory(python/blocktool)
endif(ENABLE_GR_BLOCKTOOL)

if(ENABLE_GR_MODTOOL)
add_subdirectory(python/modtool)
endif(ENABLE_GR_MODTOOL)

endif(ENABLE_GR_UTILS)
