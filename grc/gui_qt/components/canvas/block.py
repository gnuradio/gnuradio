import logging

# third-party modules
from qtpy import QtGui, QtCore, QtWidgets
from qtpy.QtCore import Qt, QUrl

from . import colors
from ... import Constants
from ... import Utils
from ....core.blocks.block import Block as CoreBlock
from ....core.utils import flow_graph_complexity

from ..dialogs import PropsDialog

# Logging
log = logging.getLogger(__name__)

LONG_VALUE = 20  # maximum length of a param string.
# if exceeded, '...' will be displayed


class Block(QtWidgets.QGraphicsItem, CoreBlock):
    @classmethod
    def make_cls_with_base(cls, super_cls):
        name = super_cls.__name__
        bases = (super_cls,) + cls.__bases__[:-1]
        namespace = cls.__dict__.copy()
        return type(name, bases, namespace)

    def __init__(self, parent, **n):
        # super(self.__class__, self).__init__(parent, **n)
        CoreBlock.__init__(self, parent)
        QtWidgets.QGraphicsItem.__init__(self)

        for sink in self.sinks:
            sink.setParentItem(self)
        for source in self.sources:
            source.setParentItem(self)

        self.width = 300  # default shouldnt matter, it will change immedaitely after the first paint
        # self.block_key = block_key
        # self.block_label = block_label
        self.block_label = self.key

        if "coordinate" not in self.states.keys():
            self.states["coordinate"] = (500, 300)
            self.setPos(
                QtCore.QPointF(
                    self.states["coordinate"][0], self.states["coordinate"][1]
                )
            )
        if "rotation" not in self.states.keys():
            self.states["rotation"] = 0.0

        self.create_shapes_and_labels()

        self.moving = False
        self.oldPos = (self.x(), self.y())
        self.newPos = (self.x(), self.y())
        self.states["coordinate"] = (self.x(), self.y())
        self.oldData = None
        self.props_dialog = None

        self.setFlag(QtWidgets.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtWidgets.QGraphicsItem.ItemIsSelectable)
        self.setFlag(QtWidgets.QGraphicsItem.ItemSendsScenePositionChanges)

    def create_shapes_and_labels(self):
        self.force_show_id = self.parent.app.qsettings.value('grc/show_block_ids', type=bool)
        self.hide_variables = self.parent.app.qsettings.value('grc/hide_variables', type=bool)
        self.hide_disabled_blocks = self.parent.app.qsettings.value('grc/hide_disabled_blocks', type=bool)
        self.snap_to_grid = self.parent.app.qsettings.value('grc/snap_to_grid', type=bool)
        self.show_complexity = self.parent.app.qsettings.value('grc/show_complexity', type=bool)
        self.show_block_comments = self.parent.app.qsettings.value('grc/show_block_comments', type=bool)
        self.show_param_expr = self.parent.app.qsettings.value('grc/show_param_expr', type=bool)
        self.show_param_val = self.parent.app.qsettings.value('grc/show_param_val', type=bool)
        self.prepareGeometryChange()
        font = QtGui.QFont("Helvetica", 10)
        font.setBold(True)

        # figure out height of block based on how many params there are
        i = 30

        for key, item in self.params.items():
            value = item.value
            if (value is not None and item.hide == "none") or (item.dtype == 'id' and self.force_show_id):
                i += 20

        self.height = i

        def get_min_height_for_ports(ports):
            min_height = (
                2 * Constants.PORT_BORDER_SEPARATION +
                len(ports) * Constants.PORT_SEPARATION
            )
            # If any of the ports are bus ports - make the min height larger
            if any([p.dtype == "bus" for p in ports]):
                min_height = (
                    2 * Constants.PORT_BORDER_SEPARATION +
                    sum(
                        port.height + Constants.PORT_SPACING
                        for port in ports
                        if port.dtype == "bus"
                    ) - Constants.PORT_SPACING
                )

            else:
                if ports:
                    min_height -= ports[-1].height
            return min_height

        self.height = max(
            self.height,
            get_min_height_for_ports(self.active_sinks),
            get_min_height_for_ports(self.active_sources),
        )
        # figure out width of block based on widest line of text
        fm = QtGui.QFontMetrics(font)
        largest_width = fm.width(self.label)
        for key, item in self.params.items():
            name = item.name
            value = item.value
            if (value is not None and item.hide == "none") or (item.dtype == 'id' and self.force_show_id):
                if len(value) > LONG_VALUE:
                    value = value[:LONG_VALUE - 3] + '...'

                value_label = item.options[value] if value in item.options else value
                full_line = name + ": " + value_label

                if fm.width(full_line) > largest_width:
                    largest_width = fm.width(full_line)
        self.width = largest_width + 15

        bussified = (
            self.current_bus_structure["source"],
            self.current_bus_structure["sink"],
        )
        for ports, has_busses in zip(
            (self.active_sources, self.active_sinks), bussified
        ):
            if not ports:
                continue
            port_separation = (
                Constants.PORT_SEPARATION
                if not has_busses
                else ports[0].height + Constants.PORT_SPACING
            )
            offset = (
                self.height - (len(ports) - 1) * port_separation - ports[0].height
            ) / 2
            for port in ports:
                if port._dir == "sink":
                    port.setPos(-15, offset)
                else:
                    port.setPos(self.width, offset)
                port.create_shapes_and_labels()
                """
                port.coordinate = {
                    0: (+self.width, offset),
                    90: (offset, -port.width),
                    180: (-port.width, offset),
                    270: (offset, +self.width),
                }[port.connector_direction]
                """

                offset += (
                    Constants.PORT_SEPARATION
                    if not has_busses
                    else port.height + Constants.PORT_SPACING
                )

        self._update_colors()
        self.create_port_labels()
        self.setTransformOriginPoint(self.width / 2, self.height / 2)

    def create_port_labels(self):
        for ports in (self.active_sinks, self.active_sources):
            for port in ports:
                port.create_shapes_and_labels()
                # max_width = max(max_width, port.width_with_label)
            # for port in ports:
            #    port.width = max_width

    def _update_colors(self):
        def get_bg():
            """
            Get the background color for this block
            Explicit is better than a chain of if/else expressions,
            so this was extracted into a nested function.
            """
            if self.is_dummy_block:
                return colors.MISSING_BLOCK_BACKGROUND_COLOR
            if self.state == "bypassed":
                return colors.BLOCK_BYPASSED_COLOR
            if self.state == "enabled":
                if self.deprecated:
                    return colors.BLOCK_DEPRECATED_BACKGROUND_COLOR
                return colors.BLOCK_ENABLED_COLOR
            return colors.BLOCK_DISABLED_COLOR

        def get_border():
            """
            Get the border color for this block
            """
            if self.is_dummy_block:
                return colors.MISSING_BLOCK_BORDER_COLOR
            if self.deprecated:
                return colors.BLOCK_DEPRECATED_BORDER_COLOR
            if self.state == "enabled":
                return colors.BORDER_COLOR
            return colors.BORDER_COLOR_DISABLED

        self._bg_color = get_bg()
        # self._font_color[-1] = 1.0 if self.state == 'enabled' else 0.4
        self._border_color = get_border()

    def paint(self, painter, option, widget):
        if (self.hide_variables and (self.is_variable or self.is_import)) or (self.hide_disabled_blocks and not self.enabled):
            return
        self.states["coordinate"] = (self.x(), self.y())
        # Set font
        font = QtGui.QFont("Helvetica", 10)
        # font.setStretch(70) # makes it more condensed
        font.setBold(True)

        painter.setRenderHint(QtGui.QPainter.Antialiasing)

        # Draw main rectangle
        pen = QtGui.QPen(1)
        if self.isSelected():
            pen = QtGui.QPen(colors.HIGHLIGHT_COLOR)
        else:
            pen = QtGui.QPen(self._border_color)

        pen.setWidth(3)
        painter.setPen(pen)

        painter.setBrush(QtGui.QBrush(self._bg_color))

        rect = QtCore.QRectF(0, 0, self.width, self.height)
        painter.drawRect(rect)
        painter.setPen(QtGui.QPen(1))

        # Draw block label text
        painter.setFont(font)
        if self.is_valid():
            painter.setPen(Qt.black)
        else:
            painter.setPen(Qt.red)
        painter.drawText(
            QtCore.QRectF(0, 0 - self.height / 2 + 15, self.width, self.height),
            Qt.AlignCenter,
            self.label,
        )  # NOTE the 3rd/4th arg in  QRectF seems to set the bounding box of the text, so if there is ever any clipping, thats why

        # Draw param text
        y_offset = 30  # params start 30 down from the top of the box
        for key, item in self.params.items():
            name = item.name
            value = item.value
            is_evaluated = item.value != str(item.get_evaluated())

            display_value = ""

            # Include the value defined by the user (after evaluation)
            if not is_evaluated or self.show_param_val or not self.show_param_expr:
                display_value += item.options[value] if value in item.options else value  # TODO: pretty_print

            # Include the expression that was evaluated to get the value
            if is_evaluated and self.show_param_expr:
                expr_string = value  # TODO: Truncate

                if display_value:  # We are already displaying the value
                    display_value = expr_string + "=" + display_value
                else:
                    display_value = expr_string

            if len(display_value) > LONG_VALUE:
                display_value = display_value[:LONG_VALUE - 3] + '...'

            value_label = display_value
            if (value is not None and item.hide == "none") or (item.dtype == 'id' and self.force_show_id):
                if item.is_valid():
                    painter.setPen(QtGui.QPen(1))
                else:
                    painter.setPen(Qt.red)

                font.setBold(True)
                painter.setFont(font)
                painter.drawText(
                    QtCore.QRectF(7.5, 0 + y_offset, self.width, self.height),
                    Qt.AlignLeft,
                    name + ": ",
                )
                fm = QtGui.QFontMetrics(font)
                font.setBold(False)
                painter.setFont(font)
                painter.drawText(
                    QtCore.QRectF(
                        7.5 + fm.width(name + ": "),
                        0 + y_offset,
                        self.width,
                        self.height,
                    ),
                    Qt.AlignLeft,
                    value_label,
                )
                y_offset += 20

        markups = []
        if self.show_complexity and self.key == "options":
            complexity = flow_graph_complexity.calculate(self.parent)
            markups.append('Complexity: {num} bal'.format(
                num=Utils.num_to_str(complexity)))

        if self.show_block_comments and self.comment:
            markups.append(self.comment)

        if markups:  # TODO: Calculate comment box size
            painter.setPen(Qt.gray)
            painter.drawText(
                QtCore.QRectF(0, self.height + 5, self.width, self.height),
                Qt.AlignLeft,
                "\n".join(markups),
            )

    def boundingRect(self):  # required to have
        return QtCore.QRectF(  # TODO: Calculate comment box size
            -2.5, -2.5, self.width + 5, self.height + (5 if not self.comment else 50)
        )  # margin to avoid artifacts

    def setStates(self, states):
        for k, v in states.items():
            self.states[k] = v

        self.setPos(self.states["coordinate"][0], self.states["coordinate"][1])
        self.setRotation(self.states["rotation"])

    def mouseReleaseEvent(self, e):
        super(self.__class__, self).mouseReleaseEvent(e)

    def mousePressEvent(self, e):
        super(self.__class__, self).mousePressEvent(e)
        log.debug(f"{self} clicked")
        url_prefix = str(self.parent.app.platform.config.wiki_block_docs_url_prefix)
        self.parent.app.WikiTab.setURL(QUrl(url_prefix + self.label.replace(" ", "_")))

        self.moveToTop()

    def contextMenuEvent(self, e):
        if not self.isSelected():
            self.parent.clearSelection()
            self.setSelected(True)

        view = self.parent.view
        contextMenu = view._app().MainWindow.menus["edit"]
        contextMenu.exec_(e.screenPos())

    def mouseDoubleClickEvent(self, e):
        self.open_properties()
        super(self.__class__, self).mouseDoubleClickEvent(e)

    def itemChange(self, change, value):
        if change == QtWidgets.QGraphicsItem.ItemPositionChange and self.scene() and self.snap_to_grid:
            grid_size = 10
            value.setX(round(value.x() / grid_size) * grid_size)
            value.setY(round(value.y() / grid_size) * grid_size)
            return value
        else:
            return QtWidgets.QGraphicsItem.itemChange(self, change, value)

    def import_data(self, name, states, parameters, **_):
        CoreBlock.import_data(self, name, states, parameters, **_)
        self.states["coordinate"] = QtCore.QPointF(
            states["coordinate"][0], states["coordinate"][1]
        )
        self.setPos(self.states["coordinate"])
        self.rewrite()
        self.create_shapes_and_labels()

    def rotate(self, rotation):
        log.debug(f"Rotating {self.name}")
        self.setRotation(self.rotation() + rotation)

    def moveToTop(self):
        # TODO: Is there a simpler way to do this?
        self.setZValue(self.parent.getMaxZValue() + 1)

    def center(self):
        return QtCore.QPointF(self.x() + self.width / 2, self.y() + self.height / 2)

    def open_properties(self):
        self.props_dialog = PropsDialog(self, self.force_show_id)
        self.props_dialog.show()

    def update_bus_logic(self):
        ###############################
        # Bus Logic
        ###############################
        for direc in {'source', 'sink'}:
            if direc == 'source':
                ports = self.sources
                ports_gui = self.filter_bus_port(self.sources)
            else:
                ports = self.sinks
                ports_gui = self.filter_bus_port(self.sinks)
            if 'bus' in map(lambda a: a.dtype, ports):
                for port in ports_gui:
                    self.parent_flowgraph.removeItem(port)
        #super(Block, self).update_bus_logic()
        super(self.__class__, self).update_bus_logic()
