from qtpy.QtGui import QPen, QPainter, QBrush, QFont, QFontMetrics
from qtpy.QtCore import Qt, QPointF, QRectF
from qtpy.QtWidgets import QGraphicsItem

from . import colors
from ... import Constants
from ....core.ports import Port as CorePort


class Port(CorePort):
    @classmethod
    def make_cls_with_base(cls, super_cls):
        name = super_cls.__name__
        bases = (super_cls,) + cls.__bases__[:-1]
        namespace = cls.__dict__.copy()
        return type(name, bases, namespace)

    def __init__(self, parent, direction, **n):
        super(self.__class__, self).__init__(parent, direction, **n)
        self.gui = GUIPort(self, direction)

    def remove_clone(self, port):
        self.gui.scene().removeItem(port.gui)
        super(self.__class__, self).remove_clone(port)


class GUIPort(QGraphicsItem):
    """
    The graphical port. Interfaces with its underlying Port object using self.core.
    The GUIPort is always instantiated in the Port constructor.

    Note that this constructor is called before its parent GUIBlock is instantiated,
    which is why we call setParentItem() in create_shapes_and_labels().
    """

    def __init__(self, core, direction, **n):
        self.core = core
        QGraphicsItem.__init__(self)

        self.y_offset = 0
        self.height = 3 * 15.0 if self.core.dtype == 'bus' else 15.0
        self.width = 15.0

        if self.core._dir == "sink":
            self.connection_point = self.mapToScene(QPointF(0.0, self.height / 2.0))
            self.ctrl_point = self.mapToScene(QPointF(0.0, self.height / 2.0) - QPointF(5.0, 0.0))
        else:
            self.connection_point = self.mapToScene(QPointF(self.width, self.height / 2.0))
            self.ctrl_point = self.mapToScene(QPointF(self.width, self.height / 2.0) + QPointF(5.0, 0.0))

        self.setFlag(QGraphicsItem.ItemSendsScenePositionChanges)

        self._border_color = self._bg_color = colors.BLOCK_ENABLED_COLOR

        self.setFlag(QGraphicsItem.ItemStacksBehindParent)
        self.setFlag(QGraphicsItem.ItemIsSelectable)
        self.setAcceptHoverEvents(True)

        self._hovering = False
        self.auto_hide_port_labels = False

        self.font = QFont('Helvetica', 8)
        self.fm = QFontMetrics(self.font)

        # TODO: Move somewhere else? Not necessarily
        self.core.parent_flowgraph.gui.addItem(self)

    def update_connections(self):
        if self.core._dir == "sink":
            self.connection_point = self.mapToScene(QPointF(-10.0, self.height / 2.0))
            self.ctrl_point = self.mapToScene(QPointF(-10.0, self.height / 2.0) - QPointF(100.0, 0.0))
        else:
            self.connection_point = self.mapToScene(QPointF(self.width, self.height / 2.0))
            self.ctrl_point = self.mapToScene(QPointF(self.width, self.height / 2.0) + QPointF(100.0, 0.0))

        for conn in self.core.connections():
            conn.gui.set_rotation(self.parentItem().rotation())
            conn.gui.update()

    def itemChange(self, change, value):
        self.update_connections()
        return QGraphicsItem.itemChange(self, change, value)

    def create_shapes_and_labels(self):
        self.auto_hide_port_labels = self.core.parent.parent.gui.app.qsettings.value(
            'grc/auto_hide_port_labels', type=bool)
        if not self.parentItem():
            self.setParentItem(self.core.parent_block.gui)

        self.width = max(15, self.fm.width(self.core.name) * 1.5)
        self._update_colors()
        self.update_connections()

    @property
    def _show_label(self) -> bool:
        return self._hovering or not self.auto_hide_port_labels

    def _update_colors(self):
        """
        Get the color that represents this port's type.
        TODO: Codes differ for ports where the vec length is 1 or greater than 1.
        """
        if not self.core.parent.enabled:
            color = colors.BLOCK_DISABLED_COLOR
        elif self.core.domain == Constants.GR_MESSAGE_DOMAIN:
            color = colors.PORT_TYPE_TO_COLOR.get('message')
        else:
            color = colors.PORT_TYPE_TO_COLOR.get(self.core.dtype) or colors.PORT_TYPE_TO_COLOR.get('')
        self._bg_color = color
        self._border_color = color

    def paint(self, painter, option, widget):
        if self.core.hidden:
            return
        painter.setRenderHint(QPainter.Antialiasing)

        pen = QPen(self._border_color)
        painter.setPen(pen)
        painter.setBrush(QBrush(self._bg_color))

        # TODO: should boundingRect() be used here ?
        if self.core._dir == "sink":
            rect = QRectF(-max(0, self.width - 15), 0, self.width, self.height)
        else:
            rect = QRectF(0, 0, self.width, self.height)
        painter.drawRect(rect)

        # TODO: Adjustments for finer rotation values if required (eg. 15, 30)
        block_rotation = self.parentItem().rotation()

        if self._show_label:
            painter.setPen(QPen(1))
            painter.setFont(self.font)

            # Adjust the painter if parent block is 180 degrees rotated
            if block_rotation == 180:
                painter.translate(self.width, self.height)
                painter.rotate(180)

            if self.core._dir == "sink":
                if block_rotation == 180:
                    painter.drawText(QRectF(max(0, self.width - 15), 0, self.width,
                                     self.height), Qt.AlignCenter, self.core.name)
                else:
                    painter.drawText(QRectF(-max(0, self.width - 15), 0, self.width,
                                     self.height), Qt.AlignCenter, self.core.name)
            else:
                painter.drawText(QRectF(0, 0, self.width, self.height), Qt.AlignCenter, self.core.name)

    def center(self):
        return QPointF(self.x() + self.width / 2, self.y() + self.height / 2)

    def boundingRect(self):
        if self.core._dir == "sink":
            return QRectF(-max(0, self.width - 15), 0, self.width, self.height)
        else:
            return QRectF(0, 0, self.width, self.height)

    def hoverEnterEvent(self, event):
        self._hovering = True
        return QGraphicsItem.hoverEnterEvent(self, event)

    def hoverLeaveEvent(self, event):
        self._hovering = False
        return QGraphicsItem.hoverLeaveEvent(self, event)
