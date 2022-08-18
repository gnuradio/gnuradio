# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Man pages
########################################################################

set(MAN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MAN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

list(
    APPEND
    manpages
    dial_tone.1
    display_qt.1
    gnuradio-companion.1
    gnuradio-config-info.1
    grcc.1
    gr-ctrlport-monitor.1
    gr_filter_design.1
    gr_modtool.1
    gr-perf-monitorx.1
    gr_plot_const.1
    gr_plot_fft.1
    gr_plot_iq.1
    gr_plot_psd.1
    gr_plot_qt.1
    gr_plot_time.1
    gr_read_file_metadata.1
    polar_channel_construction.1
    tags_demo.1
    uhd_fft.1
    uhd_rx_cfile.1
    uhd_rx_nogui.1
    uhd_siggen.1
    uhd_siggen_gui.1)

foreach(manpage ${manpages})
    configure_file(${MAN_SRC_DIR}/${manpage}.in ${MAN_BUILD_DIR}/${manpage} @ONLY)
    install(FILES ${MAN_BUILD_DIR}/${manpage} DESTINATION ${GR_MAN_DIR}/man1)
endforeach(manpage)
