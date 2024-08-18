# Copyright 2018,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

GR_PYTHON_INSTALL(FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/modtool
)

set(GR_PKG_MODTOOL_DATA_DIR ${GR_PKG_DATA_DIR}/modtool/templates)


########################################################################
# Create and install the modtool conf file
########################################################################
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${GR_PKG_MODTOOL_DATA_DIR}/gr-newmod newmoddir)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/modtool.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/modtool.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/modtool.conf
    DESTINATION ${GR_PREFSDIR}
)


########################################################################
# Install shell completion scripts
########################################################################
if(ENABLE_BASH_COMPLETIONS)
    install(
        FILES complete_bash.in
        RENAME gr_modtool
        DESTINATION ${GR_BASH_COMPLETIONS_DIR}
    )
endif(ENABLE_BASH_COMPLETIONS)

if(ENABLE_ZSH_COMPLETIONS)
    install(
        FILES complete_zsh.in
        RENAME _gr_modtool
        DESTINATION ${GR_ZSH_COMPLETIONS_DIR}
    )
endif(ENABLE_ZSH_COMPLETIONS)

if(ENABLE_FISH_COMPLETIONS)
    install(
        FILES complete_fish.in
        RENAME gr_modtool.fish
        DESTINATION ${GR_FISH_COMPLETIONS_DIR}
    )
endif(ENABLE_FISH_COMPLETIONS)


GR_PYTHON_INSTALL(
    PROGRAMS    gr_modtool
    DESTINATION ${GR_RUNTIME_DIR}
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(core)
add_subdirectory(cli)
add_subdirectory(tools)
add_subdirectory(templates)
add_subdirectory(tests)
