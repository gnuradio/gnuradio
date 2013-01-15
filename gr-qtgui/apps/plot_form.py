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
    def __init__(self, top_block, title=''):
        QtGui.QWidget.__init__(self, None)

        self._start = 0
        self._end = 0

        self.top_block = top_block
        
        self.setWindowTitle(title)

        self.size_val = QtGui.QIntValidator(0, top_block._max_nsamps, self)

        self.start_form = QtGui.QFormLayout()
        self.start_edit = QtGui.QLineEdit(self)
        self.start_edit.setMinimumWidth(100)
        self.start_edit.setText(QtCore.QString("%1").arg(top_block._start))
        self.start_edit.setValidator(self.size_val)
        self.start_form.addRow("Start:", self.start_edit)
        self.connect(self.start_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_points)

        end = top_block._start + top_block._nsamps
        self.end_form = QtGui.QFormLayout()
        self.end_edit = QtGui.QLineEdit(self)
        self.end_edit.setMinimumWidth(100)
        self.end_edit.setText(QtCore.QString("%1").arg(end))
        self.end_edit.setValidator(self.size_val)
        self.end_form.addRow("End:", self.end_edit)
        self.connect(self.end_edit, QtCore.SIGNAL("returnPressed()"),
                     self.update_points)

        self.auto_scale = QtGui.QCheckBox("Auto Scale", self)
        if(self.top_block._auto_scale):
            self.auto_scale.setChecked(self.top_block._auto_scale)
        self.connect(self.auto_scale, QtCore.SIGNAL("stateChanged(int)"),
                     self.set_auto_scale)

        self.layout = QtGui.QGridLayout(self)
        self.layout.addWidget(top_block.get_gui(), 1,0,1,2)
        self.layout.addLayout(self.start_form, 2,0,1,1)
        self.layout.addLayout(self.end_form, 2,1,1,1)
        self.layout.addWidget(self.auto_scale, 1,3,1,1)

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

        self.layout.addWidget(self.menu, 0,0,1,2)
        
        self.resize(800, 500)

    def update_points(self):
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
