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

class plot_spectrogram_form(plot_form):
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
        self.right_col_form.addRow("FFT Size:", self.psd_size_edit)
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

        self.autoscale_button = QtGui.QPushButton("Auto Scale", self)
        self.autoscale_button.setMaximumWidth(100)
        self.right_col_layout.addWidget(self.autoscale_button)
        self.connect(self.autoscale_button, QtCore.SIGNAL("clicked()"),
                     self.spectrogram_auto_scale)

        self.add_spectrogram_control(self.right_col_layout)

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

    def add_spectrogram_control(self, layout):
        self._line_tabs = QtGui.QTabWidget()

        self._line_pages = []
        self._line_forms = []
        self._label_edit = []
        self._size_edit = []
        self._color_edit = []
        self._style_edit = []
        self._marker_edit = []
        self._alpha_edit = []
        for n in xrange(self.top_block._nsigs):
            self._line_pages.append(QtGui.QDialog())
            self._line_forms.append(QtGui.QFormLayout(self._line_pages[-1]))

            label = self.top_block.gui_snk.line_label(n)
            self._label_edit.append(QtGui.QLineEdit(self))
            self._label_edit[-1].setMinimumWidth(125)
            self._label_edit[-1].setMaximumWidth(125)
            self._label_edit[-1].setText(QtCore.QString("%1").arg(label))
            self._line_forms[-1].addRow("Label:", self._label_edit[-1])
            self.connect(self._label_edit[-1], QtCore.SIGNAL("returnPressed()"),
                         self.update_line_label)

            self._qtcolormaps = ["Multi Color", "White Hot",
                                 "Black Hot", "Incandescent"]
            self._color_edit.append(QtGui.QComboBox(self))
            self._color_edit[-1].addItems(self._qtcolormaps)
            self._line_forms[-1].addRow("Color Map:", self._color_edit[-1])
            self.connect(self._color_edit[-1],
                         QtCore.SIGNAL("currentIndexChanged(int)"),
                         self.update_color_map)

            alpha_val = QtGui.QDoubleValidator(0, 1.0, 2, self)
            alpha_val.setTop(1.0)
            alpha = self.top_block.gui_snk.line_alpha(n)
            self._alpha_edit.append(QtGui.QLineEdit(self))
            self._alpha_edit[-1].setMinimumWidth(50)
            self._alpha_edit[-1].setMaximumWidth(100)
            self._alpha_edit[-1].setText(QtCore.QString("%1").arg(alpha))
            self._alpha_edit[-1].setValidator(alpha_val)
            self._line_forms[-1].addRow("Alpha:", self._alpha_edit[-1])
            self.connect(self._alpha_edit[-1], QtCore.SIGNAL("returnPressed()"),
                         self.update_line_alpha)

            self._line_tabs.addTab(self._line_pages[-1], "{0}".format(label))

        layout.addWidget(self._line_tabs)

    def update_color_map(self, c_index):
        index = self._line_tabs.currentIndex()
        self.top_block.gui_snk.set_color_map(index, c_index)
        self.update_line_alpha()

    def spectrogram_auto_scale(self):
        self.top_block.gui_snk.auto_scale()
        _min = self.top_block.gui_snk.min_intensity(0)
        _max = self.top_block.gui_snk.max_intensity(0)
        if(self.gui_y_axis):
            self.gui_y_axis(_min, _max)
