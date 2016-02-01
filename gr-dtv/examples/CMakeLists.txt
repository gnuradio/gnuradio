# Copyright 2014-2015 Free Software Foundation, Inc.
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

include(GrPython)

GR_PYTHON_INSTALL(
    PROGRAMS
    atsc_ctrlport_monitor.py
    DESTINATION ${GR_PKG_DTV_EXAMPLES_DIR}
    COMPONENT "dtv_python"
)

install(
    FILES
    README.dvbs
    README.dvbs2
    README.dvbt
    README.dvbt2
    README.catv
    dvbs2_tx.grc
    dvbs_tx.grc
    dvbt_rx_8k.grc
    dvbt_tx_2k.grc
    dvbt_tx_8k.grc
    file_atsc_rx.grc
    file_atsc_tx.grc
    uhd_atsc_capture.grc
    uhd_rx_atsc.grc
    vv003-cr23.grc
    vv009-4kfft.grc
    vv018-miso.grc
    catv_tx_64qam.grc
    DESTINATION ${GR_PKG_DTV_EXAMPLES_DIR}
    COMPONENT "dtv_python"
)
