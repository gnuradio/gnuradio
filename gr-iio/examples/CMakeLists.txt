# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

INSTALL(
    FILES
    attr-sink-source.grc
    attr-source.grc
    cyclic-sine.grc
    cyclic-sine_pluto.grc
    fmradio_pluto.grc
    fm-transmitter_pluto.grc
    DESTINATION ${GR_PKG_IIO_EXAMPLES_DIR}
    COMPONENT "iio_python"
)
