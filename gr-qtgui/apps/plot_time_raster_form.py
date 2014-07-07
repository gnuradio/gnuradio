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

import sys, math
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

class plot_time_raster_form(plot_form):
    def __init__(self, top_block, title='', scale=1):
        plot_form.__init__(self, top_block, title, scale)

        self.right_col_layout = QtGui.QVBoxLayout()
        self.right_col_form = QtGui.QFormLayout()
        self.right_col_layout.addLayout(self.right_col_form)
        self.layout.addLayout(self.right_col_layout, 1,4,1,1)

        self.auto_scale = QtGui.QCheckBox("Auto Scale", self)
        if(self.top_block._auto_scale):
            self.auto_scale.setChecked(self.top_block._auto_scale)
        self.connect(self.auto_scale, QtCore.SIGNAL("stateChanged(int)"),
                     self.set_auto_scale)
        self.right_col_layout.addWidget(self.auto_scale)

        self.ncols_edit = QtGui.QLineEdit(self)
        self.ncols_edit.setMinimumWidth(100)
        self.ncols_edit.setMaximumWidth(100)
        self.ncols_edit.setText(QtCore.QString("%1").arg(top_block._ncols))
        self.right_col_form.addRow("Num. Cols.", self.ncols_edit)
        self.connect(self.ncols_edit, QtCore.SIGNAL("returnPressed()"),
                     self.ncols_update)

        self.nrows_edit = QtGui.QLineEdit(self)
        self.nrows_edit.setMinimumWidth(100)
        self.nrows_edit.setMaximumWidth(100)
        self.nrows_edit.setText(QtCore.QString("%1").arg(top_block._nrows))
        self.right_col_form.addRow("Num. Rows.", self.nrows_edit)
        self.connect(self.nrows_edit, QtCore.SIGNAL("returnPressed()"),
                     self.nrows_update)


        self.add_raster_control(self.right_col_layout)

    def add_raster_control(self, layout):
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
            self._color_edit[-1].setCurrentIndex(1)
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

    def set_auto_scale(self, state):
        if(state):
            self.top_block.auto_scale(True)
        else:
            self.top_block.auto_scale(False)

    def update_samp_rate(self):
        sr = self.samp_rate_edit.text().toDouble()[0]
        self.top_block.gui_snk.set_samp_rate(sr)

        nsamps = int(math.ceil((self.top_block._nrows+1)*self.top_block._ncols))
        self.top_block.reset(self._start, nsamps)

    def ncols_update(self):
        n = self.ncols_edit.text().toDouble()[0]
        self.top_block.gui_snk.set_num_cols(n)
        self.top_block._ncols = n

        nsamps = int(math.ceil((self.top_block._nrows+1)*n))
        self.top_block.reset(self._start, nsamps)

    def nrows_update(self):
        n = self.nrows_edit.text().toInt()[0]
        self.top_block.gui_snk.set_num_rows(n)
        self.top_block._nrows = n

        nsamps = int(math.ceil(self.top_block._ncols*(n+1)))
        self.top_block.reset(self._start, nsamps)


