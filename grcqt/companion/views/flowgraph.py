# standard modules
import xml.etree.ElementTree as ET
from   ast import literal_eval

# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

# custom modules
from . block import Block


DEFAULT_MAX_X = 1280
DEFAULT_MAX_Y = 1024

class FlowGraph(QtWidgets.QGraphicsView):
    def __init__(self, parent, filename=None):
        super(FlowGraph, self).__init__()
        self.setParent(parent)
        self.setAlignment(Qt.AlignLeft|Qt.AlignTop)

        self.scene = QtWidgets.QGraphicsScene()

        self.setSceneRect(0,0,DEFAULT_MAX_X, DEFAULT_MAX_Y)
        if filename is not None:
            self.readFile(filename)
        else:
            self.initEmpty()

        self.setScene(self.scene);
        self.setBackgroundBrush(QtGui.QBrush(Qt.white))
        self.setInteractive(True)
        self.setDragMode(QtWidgets.QGraphicsView.RubberBandDrag)

        self.isPanning    = False
        self.mousePressed = False

    def readFile(self, filename):
        tree = ET.parse(filename)
        root = tree.getroot()
        blocks = {}

        for xml_block in tree.findall('block'):
            attrib = {}
            params = []
            block_key = xml_block.find('key').text

            for param in xml_block.findall('param'):
                key = param.find('key').text
                value = param.find('value').text
                if key.startswith('_'):
                    attrib[key] = literal_eval(value)
                else:
                    params.append((key, value))

            block = Block(block_key, attrib, params)
            x, y = block._coordinate
            proxy = self.scene.addWidget(block)
            proxy.setPos(x, y)
        bounds = self.scene.itemsBoundingRect()
        self.setSceneRect(bounds)
        self.fitInView(bounds)

    def initEmpty(self):
        self.setSceneRect(0,0,DEFAULT_MAX_X, DEFAULT_MAX_Y)



    def mousePressEvent(self,  event):
        if event.button() == Qt.LeftButton:
            self.mousePressed = True
            if self.isPanning:
                self.setCursor(Qt.ClosedHandCursor)
                self.dragPos = event.pos()
                event.accept()
            else:
                super(FlowGraph, self).mousePressEvent(event)

    def mouseMoveEvent(self, event):
        if self.mousePressed and self.isPanning:
            newPos = event.pos()
            diff = newPos - self.dragPos
            self.dragPos = newPos
            self.horizontalScrollBar().setValue(self.horizontalScrollBar().value() - diff.x())
            self.verticalScrollBar().setValue(self.verticalScrollBar().value() - diff.y())
            event.accept()
        else:
            itemUnderMouse = self.itemAt(event.pos())
            if  itemUnderMouse is not None:
                #~ print itemUnderMouse
                pass

            super(FlowGraph, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            if event.modifiers() & Qt.ControlModifier:
                self.setCursor(Qt.OpenHandCursor)
            else:
                self.isPanning = False
                self.setCursor(Qt.ArrowCursor)
            self.mousePressed = False
        super(FlowGraph, self).mouseReleaseEvent(event)

    def mouseDoubleClickEvent(self, event):
        pass

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Control and not self.mousePressed:
            self.isPanning = True
            self.setCursor(Qt.OpenHandCursor)
        else:
            super(FlowGraph, self).keyPressEvent(event)

    def keyReleaseEvent(self, event):
        if event.key() == Qt.Key_Control:
            if not self.mousePressed:
                self.isPanning = False
                self.setCursor(Qt.ArrowCursor)
        else:
            super(FlowGraph, self).keyPressEvent(event)


    def wheelEvent(self,  event):
        factor = 1.1;
        if event.angleDelta().y() < 0:
            factor = 1.0 / factor
        self.scale(factor, factor)
