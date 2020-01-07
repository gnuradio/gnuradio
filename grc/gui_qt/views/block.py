# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

ARC        = 10  # arc radius for block corners
LONG_VALUE = 20  # maximum length of a param string.
                 # if exceeded, '...' will be displayed

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


class Block(QtWidgets.QWidget):
    def __init__(self, block_key, attrib, params):
        super(Block, self).__init__()

        self.__dict__.update(attrib)

        layout = QtWidgets.QVBoxLayout()
        layout.setSpacing(0)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(BlockTitle(block_key))
        layout.addWidget(BlockParams(params))
        self.setLayout(layout)

        palette = self.palette()
        palette.setColor(self.backgroundRole(), Qt.transparent)
        self.setAutoFillBackground(True)
        self.setPalette(palette)






