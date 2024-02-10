# Copyright 2011,2012,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrPython)

gr_python_check_module(
    DESC "Mako >= ${GR_MAKO_MIN_VERSION}"
    MODULE mako
    CHECK "LooseVersion(mako.__version__) >= LooseVersion('${GR_MAKO_MIN_VERSION}')"
    VAR MAKO_FOUND)

gr_python_check_module(
    DESC "click"
    MODULE click
    VAR CLICK_FOUND
)

gr_python_check_module(
    DESC "click-plugins"
    MODULE click_plugins
    VAR CLICK_PLUGINS_FOUND
)

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(utils_python_deps MAKO_FOUND)

    set(utils_modtool_deps CLICK_FOUND)

endif(NOT CMAKE_CROSSCOMPILING)

gr_register_component(
    NAME "gr-utils"
    VAR ENABLE_GR_UTILS
    REQUIRED_COMPONENTS
        gnuradio-runtime
    REQUIRED_TPLS
        ENABLE_PYTHON
        ${utils_python_deps}
)

gr_register_component(
    NAME "gr_modtool"
    VAR ENABLE_GR_MODTOOL
    REQUIRED_COMPONENTS
        gnuradio-runtime
    REQUIRED_TPLS
        ENABLE_PYTHON
        ${utils_modtool_deps}
)

gr_register_component(
    NAME "gr_blocktool"
    VAR ENABLE_GR_BLOCKTOOL
    REQUIRED_COMPONENTS
        gnuradio-runtime
    REQUIRED_TPLS
        ENABLE_PYTHON
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_UTILS)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(plot_tools)
    add_subdirectory(read_file_metadata)

    if(ENABLE_GR_BLOCKTOOL)
        add_subdirectory(blocktool)
    endif(ENABLE_GR_BLOCKTOOL)

    if(ENABLE_GR_MODTOOL)
        add_subdirectory(bindtool)
        add_subdirectory(modtool)
    endif(ENABLE_GR_MODTOOL)

endif(ENABLE_GR_UTILS)
