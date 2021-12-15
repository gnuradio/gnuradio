import logging

# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

from . import colors
from ... import Constants
from ....core.blocks.block import Block as CoreBlock

# Logging
log = logging.getLogger(__name__)

ARC        = 10  # arc radius for block corners
LONG_VALUE = 20  # maximum length of a param string.
                 # if exceeded, '...' will be displayed

class ParameterEdit(QtWidgets.QWidget):
    def __init__(self, label, value):
        super().__init__()
        self.layout = QtWidgets.QHBoxLayout(self)
        self.layout.addWidget(QtWidgets.QLabel(label))
        edit = QtWidgets.QLineEdit(value)
        self.layout.addWidget(edit)

#TODO: Move this to a separate file
class PropsDialog(QtWidgets.QDialog):
    def __init__(self, parent_block):
        super().__init__()
        self.setMinimumSize(600, 400)
        self._block = parent_block

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
            qvb.setVerticalSpacing(5.0)
            qvb.setHorizontalSpacing(20.0)
            i = 0
            for param in self._block.params.values():
                if param.category == cat and param.hide != 'all':
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
                            dropdown.setCurrentIndex(dropdown.param_values.index(param.get_value()))
                        else:
                            dropdown.setEditable(True)
                            value_label = param.options[param.value] if param.value in param.options else param.value
                            dropdown.setCurrentText(value_label)
                    else:
                        line_edit = QtWidgets.QLineEdit(param.value)
                        line_edit.param = param
                        if f'dtype_{param.dtype}' in colors.LIGHT_THEME_STYLES:
                            line_edit.setStyleSheet(colors.LIGHT_THEME_STYLES[f'dtype_{param.dtype}'])
                        qvb.addWidget(line_edit, i, 1)
                        self.edit_params.append(line_edit)
                    qvb.addWidget(QtWidgets.QLabel("unit"), i, 2)
                i+=1
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

    def accept(self):
        super().accept()
        for par in self.edit_params:
            if isinstance(par, QtWidgets.QLineEdit):
                par.param.set_value(par.text())
            else: # Dropdown/ComboBox
                for key, val in par.param.options.items():
                    if val == par.currentText():
                        par.param.set_value(key)
        self._block.rewrite()
        self._block.validate()
        self._block.create_shapes_and_labels()


'''
class BlockTitle(QtWidgets.QLabel):
    def __init__(self, block_key):
        super(BlockTitle, self).__init__()

        title = block_key.replace('_', ' ').title()
        self.setText(title)
        title_font = QtGui.QFont("Sans Serif", 9, QtGui.QFont.Bold)
        self.setAlignment(Qt.AlignCenter)
        self.setFont(title_font)
        self.setContentsMargins(5, 5, 5, 5)
        self.title_only = False
        self.color = QtGui.QColor(0xD0, 0xD0, 0xFF)

    def paintEvent(self, event):
        # handle paintEvent just enough to provide a (partial)
        # rounded rectangle around the title

        painter = QtGui.QPainter()
        painter.begin(self)
        painter.setPen(QtGui.QPen(2))
        painter.setBrush(QtGui.QBrush(self.color))

        # set rounding to include bottom of title
        # otherwise, push the bottom of the rounded rectangle
        # outside of the widget window
        height = self.height()-1 if self.title_only else self.height()+ARC
        painter.drawRoundedRect(0, 0, self.width()-1, height, ARC, ARC);

        painter.end()

        # use the default handler for everything else
        super(BlockTitle, self).paintEvent(event)

    def boundingRect(self): # required to have
        return QRectF(self.x, self.y, self.current_width, 150) # same as the rectangle we draw

    def mouseReleaseEvent(self, e):
        super(Block, self).mouseReleaseEvent(e)

    def mouseDoubleClickEvent(self, e):
        print("DETECTED DOUBLE CLICK!")
        super(Block, self).mouseDoubleClickEvent(e)

class BlockParams(QtWidgets.QWidget):
    def __init__(self, params):
        super(BlockParams, self).__init__()
        self.params_only = False

        label_font = QtGui.QFont("Sans Serif",  8, QtGui.QFont.Bold)
        value_font = QtGui.QFont("Sans Serif",  8, QtGui.QFont.Normal)

        layout = QtWidgets.QGridLayout()
        layout.setSpacing(2)
        layout.setVerticalSpacing(0)
        layout.setContentsMargins(ARC, 3, ARC, ARC)
        for row, (key, value) in enumerate(params):
            if value is not None:
                param_label = QtWidgets.QLabel(key+': ')
                param_label.setAlignment(Qt.AlignRight)
                param_label.setFont(label_font)

                if len(value) > LONG_VALUE:
                    value = value[:LONG_VALUE-3] + '...'
                param_value = QtWidgets.QLabel(value)
                param_value.setFont(value_font)

                layout.addWidget(param_label, row, 0)
                layout.addWidget(param_value, row, 1)

        self.setLayout(layout)

        self.setContentsMargins(0, 0, 0, 5)
        self.color = QtGui.QColor(0xFA, 0xF8, 0xE0)

    def paintEvent(self, event):
        # handle paintEvent just enough to provide a (partial)
        # rounded rectangle around the title

        painter = QtGui.QPainter()
        painter.begin(self)
        painter.setPen(QtGui.QPen(2))
        painter.setBrush(QtGui.QBrush(self.color))

        # set rounding to include top of title
        # otherwise, push the top of the rounded rectangle
        # outside of the widget window
        top = 1 if self.params_only else -ARC
        painter.drawRoundedRect(0, top, self.width()-1, self.height()-1, ARC, ARC);

        painter.end()

        # use the default handler for everything else
        super(BlockParams, self).paintEvent(event)
'''

class Block(QtWidgets.QGraphicsItem, CoreBlock):

    @classmethod
    def make_cls_with_base(cls, super_cls):
        name = super_cls.__name__
        bases = (super_cls,) + cls.__bases__[:-1]
        namespace = cls.__dict__.copy()
        return type(name, bases, namespace)

    def create_shapes_and_labels(self):
        self.prepareGeometryChange()

        # figure out height of block based on how many params there are
        i = 30

        for key, item in self.params.items():
            value = item.value
            if value is not None and item.hide == 'none':
                i+= 20

        self.height = i

        def get_min_height_for_ports(ports):
            min_height = 2 * Constants.PORT_BORDER_SEPARATION + len(ports) * Constants.PORT_SEPARATION
            # If any of the ports are bus ports - make the min height larger
            if any([p.dtype == 'bus' for p in ports]):
                min_height = 2 * Constants.PORT_BORDER_SEPARATION + sum(
                    port.height + Constants.PORT_SPACING for port in ports if port.dtype == 'bus'
                    ) - Constants.PORT_SPACING

            else:
                if ports:
                    min_height -= ports[-1].height
            return min_height

        self.height = max(self.height,
                     get_min_height_for_ports(self.active_sinks),
                     get_min_height_for_ports(self.active_sources))
        # figure out width of block based on widest line of text
        fm = QtGui.QFontMetrics(QtGui.QFont('Helvetica Bold', 10))
        largest_width = fm.width(self.label)
        for key, item in self.params.items():
            name = item.name
            value = item.value
            value_label = item.options[value] if value in item.options else value
            if value is not None and item.hide == 'none':
                full_line = name + ": " + value_label
                '''
                if len(value) > LONG_VALUE:
                    value = value[:LONG_VALUE-3] + '...'
                '''
                if fm.width(full_line) > largest_width:
                    largest_width = fm.width(full_line)
        self.width = largest_width + 15

        bussified = self.current_bus_structure['source'], self.current_bus_structure['sink']
        for ports, has_busses in zip((self.active_sources, self.active_sinks), bussified):
            if not ports:
                continue
            port_separation = Constants.PORT_SEPARATION if not has_busses else ports[0].height + Constants.PORT_SPACING
            offset = (self.height - (len(ports) - 1) * port_separation - ports[0].height) / 2
            for port in ports:
                if port._dir == "sink":
                    port.setPos(-15, offset)
                else:
                    port.setPos(self.width, offset)
                port.create_shapes_and_labels()
                '''
                port.coordinate = {
                    0: (+self.width, offset),
                    90: (offset, -port.width),
                    180: (-port.width, offset),
                    270: (offset, +self.width),
                }[port.connector_direction]
                '''

                offset += Constants.PORT_SEPARATION if not has_busses else port.height + Constants.PORT_SPACING

        self._update_colors()
        self.create_port_labels()
        self.setTransformOriginPoint(self.width / 2, self.height / 2)

    def create_port_labels(self):
        for ports in (self.active_sinks, self.active_sources):
            max_width = 0
            for port in ports:
                port.create_shapes_and_labels()
                #max_width = max(max_width, port.width_with_label)
            #for port in ports:
            #    port.width = max_width


    #def __init__(self, block_key, block_label, attrib, params, parent):
    def __init__(self, parent, **n):
        #super(self.__class__, self).__init__(parent, **n)
        CoreBlock.__init__(self, parent)
        QtWidgets.QGraphicsItem.__init__(self)

        for sink in self.sinks:
            sink.setParentItem(self)
        for source in self.sources:
            source.setParentItem(self)


        #self.__dict__.update(attrib)
        #self.params = params
        #self.x = attrib['_coordinate'][0]
        #self.y = attrib['_coordinate'][1]
        #self.x = 500
        #self.y = 300
        try:
            self.coordinate = tuple(self.states['coordinate'])
        except KeyError:
            self.coordinate = (500,300)
        self.width = 300 # default shouldnt matter, it will change immedaitely after the first paint
        #self.block_key = block_key
        #self.block_label = block_label
        self.block_label = self.key


        x,y = self.coordinate
        self.setPos(x, y)

        self.create_shapes_and_labels()

        self.moving = False
        self.movingFrom = None
        self.movingTo = None

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
            if self.state == 'bypassed':
                return colors.BLOCK_BYPASSED_COLOR
            if self.state == 'enabled':
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
            if self.state == 'enabled':
                return colors.BORDER_COLOR
            return colors.BORDER_COLOR_DISABLED

        self._bg_color = get_bg()
        #self._font_color[-1] = 1.0 if self.state == 'enabled' else 0.4
        self._border_color = get_border()

    def paint(self, painter, option, widget):
        x,y = (self.x(), self.y())
        self.states['coordinate'] = (x,y)
        # Set font
        font = QtGui.QFont('Helvetica', 10)
        #font.setStretch(70) # makes it more condensed
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

        ARC = 10
        painter.drawRoundedRect(0, 0, self.width, self.height, ARC, ARC);
        painter.setPen(QtGui.QPen(1))

        # Draw block label text
        painter.setFont(font)
        if self.is_valid():
            painter.setPen(QtGui.QPen(1))
        else:
            painter.setPen(Qt.red)
        painter.drawText(QtCore.QRectF(0, 0 - self.height/2 + 10, self.width, self.height), Qt.AlignCenter, self.label)  # NOTE the 3rd/4th arg in  QRectF seems to set the bounding box of the text, so if there is ever any clipping, thats why

        # Draw param text
        y_offset = 30 # params start 30 down from the top of the box
        for key, item in self.params.items():
            name = item.name
            value = item.value
            value_label = item.options[value] if value in item.options else value
            if value is not None and item.hide == 'none':
                if item.is_valid():
                    painter.setPen(QtGui.QPen(1))
                else:
                    painter.setPen(Qt.red)

                font.setBold(True)
                painter.setFont(font)
                painter.drawText(QtCore.QRectF(7.5, 0 + y_offset, self.width, self.height), Qt.AlignLeft, name + ': ')
                font.setBold(False)
                painter.setFont(font)
                fm = QtGui.QFontMetrics(QtGui.QFont('Helvetica Bold', 10))
                painter.drawText(QtCore.QRectF(7.5 + fm.width(name + ": "), 0 + y_offset, self.width, self.height), Qt.AlignLeft, value_label)
                y_offset += 20

    def boundingRect(self): # required to have
        x,y = (self.x(), self.y())
        self.states['coordinate'] = (x,y)
        return QtCore.QRectF(-2.5, -2.5, self.width+5, self.height+5) # margin to avoid artifacts

    def registerMoveStarting(self):
        print(f"{self} moving")
        self.moving = True
        self.movingFrom = self.pos()

    def registerMoveEnding(self):
        print(f"{self} moved")
        self.moving = False
        self.movingTo = self.pos()

    def mouseReleaseEvent(self, e):
        if not self.movingFrom == self.pos():
            self.parent.registerMoveCommand(self)
        super(self.__class__, self).mouseReleaseEvent(e)

    def mousePressEvent(self, e):
        print(f"{self} clicked")
        self.parent.registerBlockMovement(self)
        try:
            self.parent.app.DocumentationTab.setText(self.documentation[self.key])
        except KeyError:
            pass

        self.moveToTop()
        super(self.__class__, self).mousePressEvent(e)

    def mouseDoubleClickEvent(self, e):
        log.debug(f"Detected double click on block {self.name}, opening PropsDialog")
        super(self.__class__, self).mouseDoubleClickEvent(e)
        props = PropsDialog(self)
        if props.exec():
            log.debug(f"Pressed Ok on block {self.name}'s PropsDialog")
        else:
            log.debug(f"Pressed Cancel on block {self.name}'s PropsDialog")

    def import_data(self, name, states, parameters, **_):
        CoreBlock.import_data(self, name, states, parameters, **_)
        self.rewrite()
        self.create_shapes_and_labels()
        x,y = tuple(states['coordinate'])
        self.setPos(x, y)

    def rotate(self, rotation):
        log.debug(f"Rotating {self.name}")
        self.setRotation(self.rotation() + rotation)

    def moveToTop(self):
        # TODO: Is there a simpler way to do this?
        self.setZValue(self.parent.getMaxZValue() + 1)

