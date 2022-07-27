#
# Copyright 2011, 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Provides a GUI interface using the QT backend.
'''

# The presence of this file turns this directory into a Python package
import os


try:
    from .qtgui_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .qtgui_python import *

try:
    import matplotlib.pyplot as plt
    from .distanceradar import DistanceRadar
    from .azelplot import AzElPlot
except ImportError:
    from gnuradio import gr
    gr.log.warn("Matplotlib is a required dependency to use DistanceRadar and AzElPlot."
                "  Please install matplotlib to use these blocks (https://matplotlib.org/)")

from .range import Range, RangeWidget, GrRangeWidget
from . import util

from .compass import GrCompass
from .togglebutton import ToggleButton
from .msgpushbutton import MsgPushButton
from .msgcheckbox import MsgCheckBox
from .digitalnumbercontrol import MsgDigitalNumberControl
from .dialcontrol import GrDialControl
from .ledindicator import GrLEDIndicator
from .graphicitem import GrGraphicItem
from .levelgauge import GrLevelGauge
from .dialgauge import GrDialGauge
from .toggleswitch import GrToggleSwitch
from .graphicoverlay import GrGraphicOverlay
from .auto_correlator_sink import AutoCorrelatorSink
from .auto_correlator_sink import AutoCorrelator
from .auto_correlator_sink import Normalize
