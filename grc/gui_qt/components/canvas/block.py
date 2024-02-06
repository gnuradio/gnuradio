import logging

from qtpy.QtGui import QPen, QPainter, QBrush, QFont, QFontMetrics
from qtpy.QtCore import Qt, QPointF, QRectF, QUrl
from qtpy.QtWidgets import QGraphicsItem, QApplication

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


class Block(CoreBlock):
    """
    A block. Accesses its graphical representation with self.gui.
    """
    @classmethod
    def make_cls_with_base(cls, super_cls):
        name = super_cls.__name__
        bases = (super_cls,) + cls.__bases__[:-1]
        namespace = cls.__dict__.copy()
        return type(name, bases, namespace)

    def __init__(self, parent, **n):
        super(self.__class__, self).__init__(parent)

        self.width = 50  # will change immediately after the first paint
        self.block_label = self.key

        self.old_data = None

        if "rotation" not in self.states.keys():
            self.states["rotation"] = 0.0

        self.gui = GUIBlock(self, parent)

    def import_data(self, name, states, parameters, **_):
        super(self.__class__, self).import_data(name, states, parameters, **_)
        self.gui.setPos(*self.states["coordinate"])
        self.gui.setRotation(self.states["rotation"])
        self.rewrite()
        self.gui.create_shapes_and_labels()

    def update_bus_logic(self):
        for direc in {'source', 'sink'}:
            if direc == 'source':
                ports = self.sources
                ports_gui = self.filter_bus_port(self.sources)
            else:
                ports = self.sinks
                ports_gui = self.filter_bus_port(self.sinks)
            if 'bus' in map(lambda a: a.dtype, ports):
                for port in ports_gui:
                    self.parent_flowgraph.gui.removeItem(port.gui)
        super(self.__class__, self).update_bus_logic()


class GUIBlock(QGraphicsItem):
    """
    The graphical representation of a block. Accesses the actual block with self.core.
    """
    def __init__(self, core, parent, **n):
        super(GUIBlock, self).__init__()
        self.core = core
        self.parent = self.scene()
        self.font = QFont("Helvetica", 10)

        self.create_shapes_and_labels()

        if "coordinate" not in self.core.states.keys():
            self.core.states["coordinate"] = (500, 300)
            self.setPos(*self.core.states["coordinate"])

        self.old_pos = (self.x(), self.y())
        self.new_pos = (self.x(), self.y())
        self.moving = False

        self.props_dialog = None
        self.right_click_menu = None

        self.setFlag(QGraphicsItem.ItemIsMovable)
        self.setFlag(QGraphicsItem.ItemIsSelectable)
        self.setFlag(QGraphicsItem.ItemSendsScenePositionChanges)

        self.markups_height = 0.0
        self.markups_width = 0.0
        self.markups = []
        self.markup_text = ""

    def create_shapes_and_labels(self):
        qsettings = QApplication.instance().qsettings
        self.force_show_id = qsettings.value('grc/show_block_ids', type=bool)
        self.hide_variables = qsettings.value('grc/hide_variables', type=bool)
        self.hide_disabled_blocks = qsettings.value('grc/hide_disabled_blocks', type=bool)
        self.snap_to_grid = qsettings.value('grc/snap_to_grid', type=bool)
        self.show_complexity = qsettings.value('grc/show_complexity', type=bool)
        self.show_block_comments = qsettings.value('grc/show_block_comments', type=bool)
        self.show_param_expr = qsettings.value('grc/show_param_expr', type=bool)
        self.show_param_val = qsettings.value('grc/show_param_val', type=bool)
        self.prepareGeometryChange()
        self.font.setBold(True)

        # figure out height of block based on how many params there are
        i = 30

        # Check if we need to increase the height to fit all the ports
        for key, item in self.core.params.items():
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
                        port.gui.height + Constants.PORT_SPACING
                        for port in ports
                        if port.dtype == "bus"
                    ) - Constants.PORT_SPACING
                )

            else:
                if ports:
                    min_height -= ports[-1].gui.height
            return min_height

        self.height = max(
            self.height,
            get_min_height_for_ports(self.core.active_sinks),
            get_min_height_for_ports(self.core.active_sources),
        )

        # Figure out width of block based on widest line of text
        fm = QFontMetrics(self.font)
        largest_width = fm.width(self.core.label)
        for key, item in self.core.params.items():
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

        self.markups = []
        self.markup_text = ""
        self.markups_width = 0.0

        if self.show_complexity and self.key == "options":
            complexity = flow_graph_complexity.calculate(self.parent)
            self.markups.append('Complexity: {num} bal'.format(
                num=Utils.num_to_str(complexity)))

        if self.show_block_comments and self.core.comment:
            self.markups.append(self.core.comment)

        self.markup_text = "\n".join(self.markups).strip()

        self.markups_height = fm.height() * (self.markup_text.count("\n") + 1)
        for line in self.markup_text.split("\n"):
            if fm.width(line) > self.markups_width:
                self.markups_width = fm.width(line)

        # Update the position and size of all the ports
        bussified = (
            self.core.current_bus_structure["source"],
            self.core.current_bus_structure["sink"],
        )
        for ports, has_busses in zip(
            (self.core.active_sources, self.core.active_sinks), bussified
        ):
            if not ports:
                continue
            port_separation = (
                Constants.PORT_SEPARATION
                if not has_busses
                else ports[0].gui.height + Constants.PORT_SPACING
            )
            offset = (
                self.height - (len(ports) - 1) * port_separation - ports[0].gui.height
            ) / 2
            for port in ports:
                if port._dir == "sink":
                    port.gui.setPos(-15, offset)
                else:
                    port.gui.setPos(self.width, offset)
                port.gui.create_shapes_and_labels()

                offset += (
                    Constants.PORT_SEPARATION
                    if not has_busses
                    else port.gui.height + Constants.PORT_SPACING
                )

        self._update_colors()
        self.create_port_labels()
        self.setTransformOriginPoint(self.width / 2, self.height / 2)

    def create_port_labels(self):
        for ports in (self.core.active_sinks, self.core.active_sources):
            for port in ports:
                port.gui.create_shapes_and_labels()

    def _update_colors(self):
        def get_bg():
            """
            Get the background color for this block
            Explicit is better than a chain of if/else expressions,
            so this was extracted into a nested function.
            """
            if self.core.is_dummy_block:
                return colors.MISSING_BLOCK_BACKGROUND_COLOR
            if self.core.state == "bypassed":
                return colors.BLOCK_BYPASSED_COLOR
            if self.core.state == "enabled":
                if self.core.deprecated:
                    return colors.BLOCK_DEPRECATED_BACKGROUND_COLOR
                return colors.BLOCK_ENABLED_COLOR
            return colors.BLOCK_DISABLED_COLOR

        self._bg_color = get_bg()

    def move(self, x, y):
        self.moveBy(x, y)
        self.core.states["coordinate"] = (self.x(), self.y())

    def paint(self, painter, option, widget):
        if (self.hide_variables and (self.is_variable or self.is_import)) or (self.hide_disabled_blocks and not self.enabled):
            return

        painter.setRenderHint(QPainter.Antialiasing)
        self.font.setBold(True)

        # TODO: Make sure this is correct
        border_color = colors.BORDER_COLOR

        if self.isSelected():
            border_color = colors.HIGHLIGHT_COLOR
        else:
            if self.core.is_dummy_block:
                border_color = colors.MISSING_BLOCK_BORDER_COLOR
            elif self.core.deprecated:
                border_color = colors.BLOCK_DEPRECATED_BORDER_COLOR
            elif self.core.state == "disabled":
                border_color = colors.BORDER_COLOR_DISABLED

        pen = QPen(1)
        pen = QPen(border_color)

        pen.setWidth(3)
        painter.setPen(pen)
        painter.setBrush(QBrush(self._bg_color))
        rect = QRectF(0, 0, self.width, self.height)

        painter.drawRect(rect)
        painter.setPen(QPen(1))

        # Draw block label text
        painter.setFont(self.font)
        if self.core.is_valid():
            painter.setPen(Qt.black)
        else:
            painter.setPen(Qt.red)
        painter.drawText(
            QRectF(0, 0 - self.height / 2 + 15, self.width, self.height),
            Qt.AlignCenter,
            self.core.label,
        )

        # Draw param text
        y_offset = 30  # params start 30 down from the top of the box
        for key, item in self.core.params.items():
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
                    painter.setPen(QPen(1))
                else:
                    painter.setPen(Qt.red)

                self.font.setBold(True)
                painter.setFont(self.font)
                painter.drawText(
                    QRectF(7.5, 0 + y_offset, self.width, self.height),
                    Qt.AlignLeft,
                    name + ": ",
                )
                fm = QFontMetrics(self.font)
                self.font.setBold(False)
                painter.setFont(self.font)
                painter.drawText(
                    QRectF(
                        7.5 + fm.width(name + ": "),
                        0 + y_offset,
                        self.width,
                        self.height,
                    ),
                    Qt.AlignLeft,
                    value_label,
                )
                y_offset += 20

        if self.markup_text:
            painter.setPen(Qt.gray)
            painter.drawText(
                QRectF(0, self.height + 5, self.markups_width, self.markups_height),
                Qt.AlignLeft,
                self.markup_text,
            )

    def boundingRect(self):
        # TODO: Comments should be a separate QGraphicsItem
        return QRectF(
            -2.5, -2.5, self.width + 5 + self.markups_width, self.height + 5 + self.markups_height
        )

    def set_states(self, states):
        for k, v in states.items():
            self.core.states[k] = v

        self.setPos(*self.core.states["coordinate"])
        self.setRotation(self.core.states["rotation"])

    def mousePressEvent(self, e):
        super(self.__class__, self).mousePressEvent(e)
        log.debug(f"{self} clicked")
        url_prefix = str(self.core.parent_platform.config.wiki_block_docs_url_prefix)
        QApplication.instance().WikiTab.setURL(QUrl(url_prefix + self.core.label.replace(" ", "_")))

        self.moveToTop()

    def contextMenuEvent(self, e):
        if not self.isSelected():
            self.scene().clearSelection()
            self.setSelected(True)

        self.right_click_menu = self.scene()._app().MainWindow.menus["edit"]
        self.right_click_menu.exec_(e.screenPos())

    def mouseDoubleClickEvent(self, e):
        self.open_properties()
        super(self.__class__, self).mouseDoubleClickEvent(e)

    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionChange and self.scene() and self.snap_to_grid:
            grid_size = 10
            value.setX(round(value.x() / grid_size) * grid_size)
            value.setY(round(value.y() / grid_size) * grid_size)
            return value
        else:
            return QGraphicsItem.itemChange(self, change, value)

    def rotate(self, rotation):
        log.debug(f"Rotating {self.core.name}")
        new_rotation = self.rotation() + rotation
        self.setRotation(new_rotation)
        self.core.states["rotation"] = new_rotation
        self.create_shapes_and_labels()
        for con in self.core.connections():
            con.gui.update()

    def moveToTop(self):
        # TODO: Is there a simpler way to do this?
        self.setZValue(self.scene().getMaxZValue() + 1)

    def center(self):
        return QPointF(self.x() + self.width / 2, self.y() + self.height / 2)

    def open_properties(self):
        self.props_dialog = PropsDialog(self.core, self.force_show_id)
        self.props_dialog.show()
