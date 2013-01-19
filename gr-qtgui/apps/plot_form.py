#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

try:
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Error: Program requires PyQt4."
    sys.exit(1)

class dialog_box(QtGui.QWidget):
    def __init__(self, top_block, title='', doautoscale=False):
        QtGui.QWidget.__init__(self, None)

        self._do_autoscale = doautoscale
        self._start = 0
        self._end = 0
        self._y_min = 0
        self._y_max = 0

        self.top_block = top_block
        self.top_block.gui_y_axis = self.gui_y_axis
        
        self.setWindowTitle(title)

        self.layout = QtGui.QGridLayout(self)
        self.layout.addWidget(top_block.get_gui(), 1,2,1,2)

        if(self._do_autoscale):
            self.auto_scale = QtGui.QCheckBox("Auto Scale", self)
            if(self.top_block._auto_scale):
                self.auto_scale.setChecked(self.top_block._auto_scale)
            self.connect(self.auto_scale, QtCore.SIGNAL("stateChanged(int)"),
                         self.set_auto_scale)
            self.layout.addWidget(self.auto_scale, 1,4,1,1)

        # Create a save action
        self.save_act = QtGui.QAction("Save", self)
        self.save_act.setShortcut(QtGui.QKeySequence.Save)
        self.connect(self.save_act, QtCore.SIGNAL("triggered()"),
                     self.save_figure)

        # Create an exit action
        self.exit_act = QtGui.QAction("Exit", self)
        self.exit_act.setShortcut(QtGui.QKeySequence.Close)
        self.connect(self.exit_act, QtCore.SIGNAL("triggered()"),
                     self.close)

        # Create a menu for the window
        self.menu = QtGui.QToolBar("Menu", self)
        self.menu.addAction(self.save_act)
        self.menu.addAction(self.exit_act)

        self.layout.addWidget(self.menu, 0,0,1,4)

        self.left_col_form = QtGui.QFormLayout()
        self.layout.addLayout(self.left_col_form, 1,0,1,1)
        self.layout.setColumnStretch(0, 0)
        self.layout.setColumnStretch(2, 1)

        # Create Edit boxes for X-axis start/stop
        self.size_val = QtGui.QIntValidator(0, top_block._max_nsamps, self)

        self.start_edit = QtGui.QLineEdit(self)
        self.start_edit.setMinimumWidth(50)
        self.start_edit.setMaximumWidth(100)
        self.start_edit.setText(QtCore.QString("%1").arg(top_block._start))
        self.start_edit.setValidator(self.size_val)
        self.left_col_form.addRow("Start:", self.start_edit)
        self.connect(self.start_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_xaxis_pos)

        end = top_block._start + top_block._nsamps
        self.end_edit = QtGui.QLineEdit(self)
        self.end_edit.setMinimumWidth(50)
        self.end_edit.setMaximumWidth(100)
        self.end_edit.setText(QtCore.QString("%1").arg(end))
        self.end_edit.setValidator(self.size_val)
        self.left_col_form.addRow("End:", self.end_edit)
        self.connect(self.end_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_xaxis_pos)

        # Create a slider to move the position in the file
        self.posbar = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        self.posbar.setMaximum(self.top_block._max_nsamps)
        self.posbar.setPageStep(self.top_block._nsamps)
        self.connect(self.posbar, QtCore.SIGNAL("valueChanged(int)"),
                     self.update_xaxis_slider)
        self.layout.addWidget(self.posbar, 2,2,1,1)

        # Create Edit boxes for Y-axis min/max
        self.y_range_val = QtGui.QDoubleValidator(top_block._y_min,
                                                  top_block._y_max,
                                                  4, self)

        self.y_max_edit = QtGui.QLineEdit(self)
        self.y_max_edit.setValidator(self.y_range_val)
        self.y_max_edit.setMinimumWidth(50)
        self.y_max_edit.setMaximumWidth(100)
        self.left_col_form.addRow("Y Max:", self.y_max_edit)
        self.connect(self.y_max_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_yaxis_pos)

        self.y_min_edit = QtGui.QLineEdit(self)
        self.y_min_edit.setValidator(self.y_range_val)
        self.y_min_edit.setMinimumWidth(50)
        self.y_min_edit.setMaximumWidth(100)
        self.left_col_form.addRow("Y Min:", self.y_min_edit)
        self.connect(self.y_min_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_yaxis_pos)

        self.gui_y_axis(top_block._y_value-top_block._y_range, top_block._y_value)

        # Create a slider to move the plot's y-axis offset
        self.ybar = QtGui.QSlider(QtCore.Qt.Vertical, self)
        self.ybar.setMinimum(self.top_block._y_min)
        self.ybar.setMaximum(self.top_block._y_max)
        self.ybar.setPageStep(self.top_block._y_range)
        self.ybar.setValue(self.top_block._y_value)
        self.connect(self.ybar, QtCore.SIGNAL("valueChanged(int)"),
                     self.update_yaxis_slider)
        self.layout.addWidget(self.ybar, 1,1,1,1)

        self.resize(800, 500)

    def update_xaxis_pos(self):
        newstart = self.start_edit.text().toUInt()[0]
        newend = self.end_edit.text().toUInt()[0]
        if(newstart != self._start or newend != self._end):
            if(newend < newstart):
                QtGui.QMessageBox.information(
                    self, "Warning",
                    "End sample is less than start sample.",
                    QtGui.QMessageBox.Ok);
            else:
                newnsamps = newend - newstart
                self.top_block.reset(newstart, newnsamps)
        self._start = newstart
        self._end = newend
        self.posbar.setPageStep(self.top_block._nsamps)
        self.posbar.setValue(self._start)

    def update_xaxis_slider(self, value):
        self._start = value
        self._end = value + self.posbar.pageStep()

        self.start_edit.setText("{0}".format(self._start))
        self.end_edit.setText("{0}".format(self._end))

        if(value != self.top_block._max_nsamps):
            self.top_block.reset(self._start, self.posbar.pageStep())

    def update_yaxis_pos(self):
        newmin = self.y_min_edit.text().toDouble()[0]
        newmax = self.y_max_edit.text().toDouble()[0]
        if(newmin != self._y_min or newmax != self._y_max):
            self._y_min = newmin
            self._y_max = newmax
            self.top_block._y_range = newmax - newmin
            self.ybar.setValue(self._y_max)
            self.top_block.set_y_axis(self._y_min, self._y_max)

    def update_yaxis_slider(self, value):
        self.top_block._y_value = value
        self._y_min = value - self.top_block._y_range
        self._y_max = value

        self._y_min, self._y_max = self.top_block.set_y_axis(self._y_min, self._y_max)
        
        self.gui_y_axis(self._y_min, self._y_max)

    def gui_y_axis(self, ymin, ymax):
        self.y_min_edit.setText("{0:.4f}".format(ymin))
        self.y_max_edit.setText("{0:.4f}".format(ymax))

    def save_figure(self):
        qpix = QtGui.QPixmap.grabWidget(self.top_block.pyWin)
        types = "JPEG file (*.jpg);;" + \
            "Portable Network Graphics file (*.png);;" + \
            "Bitmap file (*.bmp);;" + \
            "TIFF file (*.tiff)"
        filebox = QtGui.QFileDialog(self, "Save Image", "./", types)
        filebox.setViewMode(QtGui.QFileDialog.Detail)
        if(filebox.exec_()):
            filename = filebox.selectedFiles()[0]
            filetype = filebox.selectedNameFilter()
        else:
            return

        if(filetype.contains(".jpg")):
            qpix.save(filename, "JPEG");
        elif(filetype.contains(".png")):
            qpix.save(filename, "PNG");
        elif(filetype.contains(".bmp")):
            qpix.save(filename, "BMP");
        elif(filetype.contains(".tiff")):
            qpix.save(filename, "TIFF");
        else:
            qpix.save(filename, "JPEG");

    def set_auto_scale(self, state):
        if(state):
            self.top_block.auto_scale(True)
        else:
            self.top_block.auto_scale(False)
