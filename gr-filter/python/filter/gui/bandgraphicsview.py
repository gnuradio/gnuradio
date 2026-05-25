# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from qtpy.QtCore import Qt
from qtpy.QtWidgets import QGraphicsView


class BandGraphicsView(QGraphicsView):
    def resizeEvent(self, event):
        self.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.fitInView(self.scene().itemsBoundingRect(),
                       Qt.AspectRatioMode.KeepAspectRatio)
        self.scale(1.3, 1.3)
        self.setViewportMargins(10, 10, 10, 10)
