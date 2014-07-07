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

class plot_psd_form(plot_form):
    def __init__(self, top_block, title=''):
        plot_form.__init__(self, top_block, title)

        self.right_col_layout = QtGui.QVBoxLayout()
        self.right_col_form = QtGui.QFormLayout()
        self.right_col_layout.addLayout(self.right_col_form)
        self.layout.addLayout(self.right_col_layout, 1,4,1,1)

        self.psd_size_val = QtGui.QIntValidator(0, 2**18, self)
        self.psd_size_edit = QtGui.QLineEdit(self)
        self.psd_size_edit.setMinimumWidth(50)
        self.psd_size_edit.setMaximumWidth(100)
        self.psd_size_edit.setText(QtCore.QString("%1").arg(top_block._psd_size))
        self.psd_size_edit.setValidator(self.psd_size_val)
        self.right_col_form.addRow("FFT:", self.psd_size_edit)
        self.connect(self.psd_size_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_psd_size)

        self.psd_win_combo = QtGui.QComboBox(self)
        self.psd_win_combo.addItems(["None", "Hamming", "Hann", "Blackman",
                                     "Rectangular", "Kaiser", "Blackman-harris"])
        self.psd_win_combo.setCurrentIndex(self.top_block.gui_snk.fft_window()+1)
        self.right_col_form.addRow("Window:", self.psd_win_combo)
        self.connect(self.psd_win_combo,
                     QtCore.SIGNAL("currentIndexChanged(int)"),
                     self.update_psd_win)

        self.psd_avg_val = QtGui.QDoubleValidator(0, 1.0, 4, self)
        self.psd_avg_edit = QtGui.QLineEdit(self)
        self.psd_avg_edit.setMinimumWidth(50)
        self.psd_avg_edit.setMaximumWidth(100)
        self.psd_avg_edit.setText(QtCore.QString("%1").arg(top_block._avg))
        self.psd_avg_edit.setValidator(self.psd_avg_val)
        self.right_col_form.addRow("Average:", self.psd_avg_edit)
        self.connect(self.psd_avg_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_psd_avg)

        self.add_line_control(self.right_col_layout)

    def update_psd_size(self):
        newpsdsize = self.psd_size_edit.text().toInt()[0]
        if(newpsdsize != self.top_block._psd_size):
            self.top_block.gui_snk.set_fft_size(newpsdsize)
            self.top_block._psd_size = newpsdsize
            self.top_block.reset(self.top_block._start,
                                 self.top_block._nsamps)

    def update_psd_win(self, index):
        self.top_block.gui_snk.set_fft_window(index-1)
        self.top_block.reset(self.top_block._start,
                             self.top_block._nsamps)

    def update_psd_avg(self):
        newpsdavg = self.psd_avg_edit.text().toDouble()[0]
        if(newpsdavg != self.top_block._avg):
            self.top_block.gui_snk.set_fft_average(newpsdavg)
            self.top_block._avg = newpsdavg
            self.top_block.reset(self.top_block._start,
                                 self.top_block._nsamps)

