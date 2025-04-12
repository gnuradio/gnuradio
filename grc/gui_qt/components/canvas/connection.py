from qtpy.QtGui import QPainterPath, QPainter, QPen, QTransform
from qtpy.QtWidgets import QGraphicsPathItem, QApplication
from qtpy.QtCore import QPointF

from enum import Enum

from ....core.Connection import Connection as CoreConnection
from . import colors
from ...Constants import (
    CONNECTOR_ARROW_BASE,
    CONNECTOR_ARROW_HEIGHT
)


class StyledConnection():
    """
    Styled connection; current styles:
    - CURVED: cubic spline
    - ANGLED: rectangular connection, one step in the horizontal middle
    """
    class ConnectionStyle(Enum):
        CURVED = 0
        ANGLED = 1

    def __init__(self):
        self._qsettings = QApplication.instance().qsettings
        self._conn_style_str = self._qsettings.value("appearance/connection_style", "CURVED", type=str)
        self._conn_style = self.ConnectionStyle[self._conn_style_str]
        self._line_width_factor = self._qsettings.value("appearance/line_width_factor", 1.0, type=float)


class DummyConnection(QGraphicsPathItem, StyledConnection):
    """
    Dummy connection used for when the user drags a connection
    between two ports.
    """

    def __init__(self, parent, start_point, end_point):
        super(DummyConnection, self).__init__()
        super(StyledConnection, self).__init__()

        self.start_point = start_point
        self.end_point = end_point

        self._line = QPainterPath()
        self._arrowhead = QPainterPath()
        self._path = QPainterPath()
        self.update(end_point)

        self._color1 = self._color2 = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None
        self.setFlag(QGraphicsPathItem.ItemIsSelectable)

    def update(self, end_point):
        """User moved the mouse, redraw with new end point"""
        self.end_point = end_point
        self._line.clear()
        self._line.moveTo(self.start_point)
        if self._conn_style == StyledConnection.ConnectionStyle.CURVED:
            c1 = self.start_point + QPointF(200, 0)
            c2 = self.end_point - QPointF(200, 0)
            self._line.cubicTo(c1, c2, self.end_point)
        elif self._conn_style == StyledConnection.ConnectionStyle.ANGLED:
            y_start = self.start_point.y()
            y_end = self.end_point.y()
            delta_x = self.end_point.y() - self.start_point.x()
            x_step = self.start_point.x() + delta_x / 2
            self._line.lineTo(QPointF(x_step, y_start))
            self._line.lineTo(QPointF(x_step, y_end))
            self._line.lineTo(self.end_point)

        self._arrowhead.clear()
        self._arrowhead.moveTo(self.end_point)
        self._arrowhead.lineTo(self.end_point + QPointF(-CONNECTOR_ARROW_HEIGHT, - CONNECTOR_ARROW_BASE / 2))
        self._arrowhead.lineTo(self.end_point + QPointF(-CONNECTOR_ARROW_HEIGHT, CONNECTOR_ARROW_BASE / 2))
        self._arrowhead.lineTo(self.end_point)

        self._path.clear()
        self._path.addPath(self._line)
        self._path.addPath(self._arrowhead)
        self.setPath(self._path)

    def paint(self, painter, option, widget):
        painter.setRenderHint(QPainter.Antialiasing)

        color = colors.BORDER_COLOR

        pen = QPen(color)

        pen.setWidthF(2 * self._line_width_factor)
        painter.setPen(pen)
        painter.drawPath(self._line)
        painter.setBrush(color)
        painter.drawPath(self._arrowhead)


class Connection(CoreConnection):
    def __init__(self, parent, source, sink):
        super(Connection, self).__init__(parent, source, sink)
        self.gui = GUIConnection(self, parent, source, sink)


class GUIConnection(QGraphicsPathItem, StyledConnection):
    def __init__(self, core, parent, source, sink):
        super(StyledConnection, self).__init__()
        self.core = core
        super(QGraphicsPathItem, self).__init__()

        self.source = source
        self.sink = sink

        self._line = QPainterPath()
        self._arrowhead = QPainterPath()
        self._path = QPainterPath()

        self._color1 = self._color2 = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None
        self.setFlag(QGraphicsPathItem.ItemIsSelectable)
        self.update()

    def update(self):
        """
        Source and sink moved in relation to each other, redraw with new end points
        """
        self._line.clear()
        self._line.moveTo(self.source.gui.connection_point)

        if self._conn_style == StyledConnection.ConnectionStyle.CURVED:
            c1 = self.source.gui.ctrl_point
            c2 = self.sink.gui.ctrl_point
            self._line.cubicTo(c1, c2, self.sink.gui.connection_point)
        elif self._conn_style == StyledConnection.ConnectionStyle.ANGLED:
            start = self.source.gui.connection_point
            end = self.sink.gui.connection_point
            y_start = start.y()
            y_end = end.y()
            delta_x = end.x() - start.x()
            x_step = start.x() + (delta_x / 2)
            self._line.lineTo(QPointF(x_step, y_start))
            self._line.lineTo(QPointF(x_step, y_end))
            self._line.lineTo(end)
        self._create_arrowhead()

        self._path.clear()
        self._path.addPath(self._line)
        self._path.addPath(self._arrowhead)
        self.setPath(self._path)

    def set_rotation(self, angle):
        self._arrow_rotation = angle

    def _create_arrowhead(self):
        self._arrowhead.clear()
        end_point = self.sink.gui.connection_point

        # Create a default arrow head path
        default_arrowhead = QPainterPath()
        default_arrowhead.moveTo(QPointF(10.0, 0))
        default_arrowhead.lineTo(QPointF(10.0, 0) + QPointF(-CONNECTOR_ARROW_HEIGHT, -CONNECTOR_ARROW_BASE / 2))
        default_arrowhead.lineTo(QPointF(10.0, 0) + QPointF(-CONNECTOR_ARROW_HEIGHT, CONNECTOR_ARROW_BASE / 2))
        default_arrowhead.lineTo(QPointF(10.0, 0))

        # Rotate the default arrow head path based on _arrow_rotation
        transform = QTransform()
        transform.translate(end_point.x(), end_point.y())
        transform.rotate(self._arrow_rotation)
        self._arrowhead = transform.map(default_arrowhead)

    def paint(self, painter, option, widget):
        painter.setRenderHint(QPainter.Antialiasing)

        color = colors.CONNECTION_ENABLED_COLOR
        if self.isSelected():
            color = colors.HIGHLIGHT_COLOR
        elif not self.core.enabled:
            color = colors.CONNECTION_DISABLED_COLOR
        elif not self.core.is_valid():
            color = colors.CONNECTION_ERROR_COLOR

        pen = QPen(color)

        pen.setWidthF(2 * self._line_width_factor)
        painter.setPen(pen)
        painter.drawPath(self._line)
        painter.setBrush(color)
        painter.drawPath(self._arrowhead)

    def mouseDoubleClickEvent(self, e):
        connection = self.core
        self.core.parent.gui.set_saved(False)
        connection.parent.connections.remove(connection)
        self.scene().removeItem(self)
