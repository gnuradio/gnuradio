# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

from ....core.Connection import Connection as CoreConnection

class Connection(CoreConnection, QtWidgets.QGraphicsPathItem):

    def __init__(self, parent, source, sink):
        CoreConnection.__init__(self, parent, source, sink)
        QtWidgets.QGraphicsItem.__init__(self)
        parent.addItem(self)

        self.source = source
        self.sink = sink

        #self._line = QtCore.QLineF(source.scenePos(), sink.scenePos())
        self._line = QtGui.QPainterPath()
        self._line.moveTo(source.connection_point)
        c1 = source.connection_point + QtCore.QPointF(200, 0)
        c2 = sink.connection_point - QtCore.QPointF(200, 0)
        self._line.cubicTo(c1, c2, sink.connection_point)
        self.setPath(self._line)

        self._line_width_factor = 1.0
        self._color1 = self._color2 = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None
        self.setFlag(QtWidgets.QGraphicsItem.ItemIsSelectable)

    def updateLine(self):
        self._line.clear()
        self._line.moveTo(self.source.connection_point)
        c1 = self.source.connection_point + QtCore.QPointF(200, 0)
        c2 = self.sink.connection_point - QtCore.QPointF(200, 0)
        self._line.cubicTo(c1, c2, self.sink.connection_point)
        self.setPath(self._line)

    def paint(self, painter, option, widget):
        painter.setRenderHint(QtGui.QPainter.Antialiasing)

        pen = QtGui.QPen(1)
        if self.isSelected():
            pen = QtGui.QPen(QtGui.QColor(0x00, 0x00, 0xFF))
        else:
            pen = QtGui.QPen(QtGui.QColor(0x61, 0x61, 0x61))
        painter.setPen(pen)
        painter.drawPath(self._line)


