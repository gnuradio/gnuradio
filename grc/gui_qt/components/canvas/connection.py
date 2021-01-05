# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

from ....core.Connection import Connection as CoreConnection

class Connection(CoreConnection, QtWidgets.QGraphicsItem):

    def __init__(self, parent, source , sink):
        CoreConnection.__init__(self, parent, source, sink)
        QtWidgets.QGraphicsItem.__init__(self)
        parent.addItem(self)
        
        self.source = source
        self.sink = sink

        self._line = []
        self._line_width_factor = 1.0
        self._color1 = self._color2 = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None

    def paint(self, painter, option, widget):
        pen = QtGui.QPen(1)
        pen.setWidth(2)
        painter.setPen(pen)

        x0, y0 = tuple(self.source.parent.states['coordinate'])
        x1, y1 = tuple(self.sink.parent.states['coordinate'])
        painter.drawLine(x0, y0, x1, y1)


    def boundingRect(self): # required to have
        x0, y0 = tuple(self.source.parent.states['coordinate'])
        x1, y1 = tuple(self.sink.parent.states['coordinate'])
        return QtCore.QRectF(x0,y0,x1,y1)
