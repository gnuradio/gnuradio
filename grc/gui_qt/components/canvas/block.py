# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

from ....core.blocks.block import Block as CoreBlock

ARC        = 10  # arc radius for block corners
LONG_VALUE = 20  # maximum length of a param string.
                 # if exceeded, '...' will be displayed
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

    #def __init__(self, block_key, block_label, attrib, params, parent):
    def __init__(self, parent, **n):
        #super(self.__class__, self).__init__(parent, **n)
        CoreBlock.__init__(self, parent)
        QtWidgets.QGraphicsItem.__init__(self)
        
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

        # figure out height of block based on how many params there are
        i = 30
        
        for key, item in self.params.items():
            value = item.value
            if value is not None and item.hide == 'none':
                i+= 20
        
        self.height = i

        
        # figure out width of block based on widest line of text
        fm = QtGui.QFontMetrics(QtGui.QFont('Helvetica', 10))
        largest_width = fm.width(self.label)/1.5
        for key, item in self.params.items():
            value = item.value
            if value is not None:
                if len(value) > LONG_VALUE:
                    value = value[:LONG_VALUE-3] + '...'
                if fm.width(value) > largest_width:
                    largest_width = fm.width(value)
                if fm.width(key + ': ') > largest_width:
                    largest_width = fm.width(key + ': ') # the keys need a little more margin
        self.width = largest_width*2 + 15 # the *2 is because we only measured half the width, the + 15 is margin


        self.setFlag(QtWidgets.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtWidgets.QGraphicsItem.ItemIsSelectable)


        '''
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
        '''

    def paint(self, painter, option, widget):
        x, y = tuple(self.states['coordinate'])
        # Set font
        font = QtGui.QFont('Helvetica', 10)
        #font.setStretch(70) # makes it more condensed
        font.setBold(True)

        # Draw main rectangle
        if self.isSelected():
            painter.setPen(QtGui.QPen(QtGui.QColor(0x42, 0xD4, 0xF5)))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(0x00, 0x00, 0x00)))
        painter.setBrush(QtGui.QBrush(QtGui.QColor(0xFA, 0xF8, 0xE0)))

        pen = QtGui.QPen(1)
        if self.isSelected():
            pen = QtGui.QPen(QtGui.QColor(0x00, 0x00, 0xFF))
        else:
            pen =QtGui.QPen(QtGui.QColor(0x61, 0x61, 0x61))

        pen.setWidth(3)
        painter.setPen(pen)

        if self.state == 'enabled':
            painter.setBrush(QtGui.QBrush(QtGui.QColor(0xF1, 0xEC, 0xFF)))
        elif self.state == 'bypassed':
            painter.setBrush(QtGui.QBrush(QtGui.QColor(0xF4, 0xFF, 0x81)))
        else: # disabled
            painter.setBrush(QtGui.QBrush(QtGui.QColor(0xCC, 0xCC, 0xCC)))

        ARC = 10
        painter.drawRoundedRect(x, y, self.width-1, self.height, ARC, ARC);
        painter.setPen(QtGui.QPen(1))

        # Draw block label text
        painter.setFont(font)
        painter.drawText(QtCore.QRectF(x, y - self.height/2 + 10, self.width, self.height), Qt.AlignCenter, self.label)  # NOTE the 3rd/4th arg in  QRectF seems to set the bounding box of the text, so if there is ever any clipping, thats why
        
        # Draw param text
        y_offset = 30 # params start 30 down from the top of the box
        for key, item in self.params.items():
            value = item.value
            if value is not None and item.hide == 'none':
                if len(value) > LONG_VALUE:
                    value = value[:LONG_VALUE-3] + '...'
                font.setBold(True)
                painter.setFont(font)
                painter.drawText(QtCore.QRectF(x - self.width/2, y + y_offset, self.width, self.height), Qt.AlignRight, key + ': ')
                font.setBold(False)
                painter.setFont(font)
                painter.drawText(QtCore.QRectF(x + self.width/2, y + y_offset, self.width, self.height), Qt.AlignLeft, value)
                y_offset += 20

    def boundingRect(self): # required to have
        x, y = tuple(self.states['coordinate'])
        return QtCore.QRectF(x, y, self.width, self.height) # same as the rectangle we draw

    def mouseReleaseEvent(self, e):
        super(self.__class__, self).mouseReleaseEvent(e)

    def mouseDoubleClickEvent(self, e):
        print("DETECTED DOUBLE CLICK!")
        super(self.__class__, self).mouseDoubleClickEvent(e)




