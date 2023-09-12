import logging

# third-party modules
from qtpy import QtGui, QtCore, QtWidgets
from qtpy.QtCore import Qt, QUrl

from . import colors
from ... import Constants
from ....core.blocks.block import Block as CoreBlock

# Logging
log = logging.getLogger(__name__)

LONG_VALUE = 20  # maximum length of a param string.
# if exceeded, '...' will be displayed
"""
class ParameterEdit(QtWidgets.QWidget):
    def __init__(self, label, value):
        super().__init__()
        self.layout = QtWidgets.QHBoxLayout(self)
        self.layout.addWidget(QtWidgets.QLabel(label))
        edit = QtWidgets.QLineEdit(value)
        self.layout.addWidget(edit)
"""


# TODO: Move this to a separate file
class PropsDialog(QtWidgets.QDialog):
    def __init__(self, parent_block):
        super().__init__()
        self.setMinimumSize(600, 400)
        self._block = parent_block
        self.setModal(True)

        self.setWindowTitle(f"Properties: {self._block.label}")

        categories = (p.category for p in self._block.params.values())

        def unique_categories():
            seen = {Constants.DEFAULT_PARAM_TAB}
            yield Constants.DEFAULT_PARAM_TAB
            for cat in categories:
                if cat in seen:
                    continue
                yield cat
                seen.add(cat)

        self.edit_params = []

        self.tabs = QtWidgets.QTabWidget()
        for cat in unique_categories():
            qvb = QtWidgets.QGridLayout()
            qvb.setAlignment(QtCore.Qt.AlignTop)
            qvb.setVerticalSpacing(5)
            qvb.setHorizontalSpacing(20)
            i = 0
            for param in self._block.params.values():
                if param.category == cat and param.hide != "all":
                    qvb.addWidget(QtWidgets.QLabel(param.name), i, 0)
                    if param.dtype == "enum" or param.options:
                        dropdown = QtWidgets.QComboBox()
                        for opt in param.options.values():
                            dropdown.addItem(opt)
                        dropdown.param_values = list(param.options)
                        dropdown.param = param
                        qvb.addWidget(dropdown, i, 1)
                        self.edit_params.append(dropdown)
                        if param.dtype == "enum":
                            dropdown.setCurrentIndex(
                                dropdown.param_values.index(param.get_value())
                            )
                        else:
                            dropdown.setEditable(True)
                            value_label = (
                                param.options[param.value]
                                if param.value in param.options
                                else param.value
                            )
                            dropdown.setCurrentText(value_label)
                    else:
                        line_edit = QtWidgets.QLineEdit(param.value)
                        line_edit.param = param
                        if f"dtype_{param.dtype}" in colors.LIGHT_THEME_STYLES:
                            line_edit.setStyleSheet(
                                colors.LIGHT_THEME_STYLES[f"dtype_{param.dtype}"]
                            )
                        qvb.addWidget(line_edit, i, 1)
                        self.edit_params.append(line_edit)
                    qvb.addWidget(QtWidgets.QLabel("unit"), i, 2)
                i += 1
            tab = QtWidgets.QWidget()
            tab.setLayout(qvb)
            self.tabs.addTab(tab, cat)

        buttons = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
        self.buttonBox = QtWidgets.QDialogButtonBox(buttons)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.tabs)
        self.layout.addWidget(self.buttonBox)

        self.setLayout(self.layout)

    # Move to Block?
    def accept(self):
        super().accept()
        self._block.oldData = self._block.export_data()
        for par in self.edit_params:
            if isinstance(par, QtWidgets.QLineEdit):
                par.param.set_value(par.text())
            else:  # Dropdown/ComboBox
                for key, val in par.param.options.items():
                    if val == par.currentText():
                        par.param.set_value(key)
        self._block.rewrite()
        self._block.validate()
        self._block.create_shapes_and_labels()
        self._block.parent.blockPropsChange.emit(self._block)


class Block(QtWidgets.QGraphicsItem, CoreBlock):
    @classmethod
    def make_cls_with_base(cls, super_cls):
        name = super_cls.__name__
        bases = (super_cls,) + cls.__bases__[:-1]
        namespace = cls.__dict__.copy()
        return type(name, bases, namespace)

    def create_shapes_and_labels(self):
        self.prepareGeometryChange()
        font = QtGui.QFont("Helvetica", 10)
        font.setBold(True)

        # figure out height of block based on how many params there are
        i = 30

        for key, item in self.params.items():
            value = item.value
            if value is not None and item.hide == "none":
                i += 20

        self.height = i

        def get_min_height_for_ports(ports):
            min_height = (
                2 * Constants.PORT_BORDER_SEPARATION
                + len(ports) * Constants.PORT_SEPARATION
            )
            # If any of the ports are bus ports - make the min height larger
            if any([p.dtype == "bus" for p in ports]):
                min_height = (
                    2 * Constants.PORT_BORDER_SEPARATION
                    + sum(
                        port.height + Constants.PORT_SPACING
                        for port in ports
                        if port.dtype == "bus"
                    )
                    - Constants.PORT_SPACING
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
            value_label = item.options[value] if value in item.options else value
            if value is not None and item.hide == "none":
                full_line = name + ": " + value_label
                """
                if len(value) > LONG_VALUE:
                    value = value[:LONG_VALUE-3] + '...'
                """
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
            max_width = 0
            for port in ports:
                port.create_shapes_and_labels()
                # max_width = max(max_width, port.width_with_label)
            # for port in ports:
            #    port.width = max_width

    # def __init__(self, block_key, block_label, attrib, params, parent):
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
        x, y = (self.x(), self.y())
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
            value_label = item.options[value] if value in item.options else value
            if value is not None and item.hide == "none":
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

    def boundingRect(self):  # required to have
        return QtCore.QRectF(
            -2.5, -2.5, self.width + 5, self.height + 5
        )  # margin to avoid artifacts

    def registerMoveStarting(self):
        self.moving = True
        self.oldPos = self.states["coordinate"]

    def setStates(self, states):
        for k, v in states.items():
            self.states[k] = v

        self.setPos(self.states["coordinate"][0], self.states["coordinate"][1])
        self.setRotation(self.states["rotation"])

    def mouseReleaseEvent(self, e):
        super(self.__class__, self).mouseReleaseEvent(e)

    def mousePressEvent(self, e):
        log.debug(f"{self} clicked")
        try:
            # self.parent.app.DocumentationTab.setText(self.documentation[self.key])
            prefix = str(self.parent.app.platform.Config.wiki_block_docs_url_prefix)
            self.parent.app.WikiTab.setURL(QUrl(prefix + self.label.replace(" ", "_")))
        except KeyError:
            pass

        self.moveToTop()
        super(self.__class__, self).mousePressEvent(e)

    def mouseDoubleClickEvent(self, e):
        log.debug(f"Detected double click on block {self.name}, opening PropsDialog")
        super(self.__class__, self).mouseDoubleClickEvent(e)
        self.props_dialog = PropsDialog(self)
        self.props_dialog.show()
        """
        if props.exec():
            log.debug(f"Pressed Ok on block {self.name}'s PropsDialog")
        else:
            log.debug(f"Pressed Cancel on block {self.name}'s PropsDialog")
        """

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
