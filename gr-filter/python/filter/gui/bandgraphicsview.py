# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from PyQt5 import QtWidgets, QtCore, Qt


class BandGraphicsView(QtWidgets.QGraphicsView):
    def resizeEvent(self, event):
        self.setAlignment(Qt.Qt.AlignCenter)
        self.fitInView(self.scene().itemsBoundingRect(),
                       QtCore.Qt.KeepAspectRatio)
        self.scale(1.3, 1.3)
        self.setViewportMargins(10, 10, 10, 10)
