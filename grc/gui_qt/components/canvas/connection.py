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
        self._line.moveTo(source.scenePos())
        c1 = source.scenePos() + QtCore.QPointF(10, 0)
        c2 = sink.scenePos() - QtCore.QPointF(10, 0)
        self._line.cubicTo(c1, c2, sink.scenePos())
        self.setPath(self._line)

        self._line_width_factor = 1.0
        self._color1 = self._color2 = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None

    def updateLine(self):
        self._line.clear()
        self._line.moveTo(self.source.scenePos())
        c1 = self.source.scenePos() + QtCore.QPointF(200, 0)
        c2 = self.sink.scenePos() - QtCore.QPointF(200, 0)
        self._line.cubicTo(c1, c2, self.sink.scenePos())
        self.setPath(self._line)

    def paint(self, painter, option, widget):
        painter.setRenderHint(QtGui.QPainter.Antialiasing)
        super(self.__class__, self).paint(painter, option, widget)


