# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

from ....core.Connection import Connection as CoreConnection

class Connection(CoreConnection, QtWidgets.QGraphicsLineItem):

    def __init__(self, parent, source, sink):
        CoreConnection.__init__(self, parent, source, sink)
        QtWidgets.QGraphicsItem.__init__(self)
        parent.addItem(self)

        self.source = source
        self.sink = sink

        self._line = QtCore.QLineF(source.scenePos(), sink.scenePos())
        self._line_width_factor = 1.0
        self._color1 = self._color2 = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None

    def updateLine(self):
        self._line.setP1(self.source.scenePos())
        self._line.setP2(self.sink.scenePos())
        self.setLine(self._line)
