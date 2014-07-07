#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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
#

import sys
from gnuradio import filter

try:
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Error: Program requires PyQt4."
    sys.exit(1)

try:
    from gnuradio.qtgui.plot_from import plot_form
except ImportError:
    from plot_form import plot_form

class plot_constellation_form(plot_form):
    def __init__(self, top_block, title='', scale=1):
        plot_form.__init__(self, top_block, title, scale)

        self.right_col_layout = QtGui.QVBoxLayout()
        self.right_col_form = QtGui.QFormLayout()
        self.right_col_layout.addLayout(self.right_col_form)
        self.layout.addLayout(self.right_col_layout, 1,4,1,1)

        # Constellation resizing scales x and y together.
        # Set the bar to go from 0.001 to max
        self.ybar.setMinimum(1)
        self.ybar.setMaximum(self._pos_scale*self.top_block._y_max)
        self.ybar.setSingleStep(self._pos_scale*(max(self.top_block._y_range/10, 0.010)))
        self.ybar.setPageStep(self._pos_scale*(max(self.top_block._y_range/2, 0.010)))

        self.auto_scale = QtGui.QCheckBox("Auto Scale", self)
        if(self.top_block._auto_scale):
            self.auto_scale.setChecked(self.top_block._auto_scale)
        self.set_auto_scale(self.top_block._auto_scale)
        self.connect(self.auto_scale, QtCore.SIGNAL("stateChanged(int)"),
                     self.set_auto_scale)
        self.right_col_layout.addWidget(self.auto_scale)

        self.ybar.setValue(1000*self.top_block._y_value)

        self.add_line_control(self.right_col_layout)

    def set_auto_scale(self, state):
        if(state):
            self.top_block.auto_scale(True)
        else:
            self.top_block.auto_scale(False)

    def update_samp_rate(self):
        pass

