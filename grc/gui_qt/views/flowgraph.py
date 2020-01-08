import six

# standard modules
import xml.etree.ElementTree as ET
from   ast import literal_eval

# third-party modules
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

# custom modules
from . block import Block

from .. import base


DEFAULT_MAX_X = 1280
DEFAULT_MAX_Y = 1024

# TODO: Combine the scene and view? Maybe the scene should be the controller?
class FlowgraphScene(QtWidgets.QGraphicsScene, base.Controller):
    def __init__(self, *args, **kwargs):
        super(FlowgraphScene, self).__init__()
        self.isPanning    = False
        self.mousePressed = False
        
    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls:
            event.setDropAction(Qt.CopyAction)
            event.accept()
        else:
            event.ignore()

    def dragMoveEvent(self, event):
        if event.mimeData().hasUrls:
            event.setDropAction(Qt.CopyAction)
            event.accept()
        else:
            event.ignore()

    def decode_data(self, bytearray):
        data = []
        item = {}
        ds = QtCore.QDataStream(bytearray)
        while not ds.atEnd():
            row = ds.readInt32()
            column = ds.readInt32()
            map_items = ds.readInt32()
            for i in range(map_items):
                key = ds.readInt32()
                value = QtCore.QVariant()
                ds >> value
                item[Qt.ItemDataRole(key)] = value
            data.append(item)
        return data

    def dropEvent(self, event):
        QtWidgets.QGraphicsScene.dropEvent(self, event)
        if event.mimeData().hasUrls:
            data = event.mimeData()
            if data.hasFormat('application/x-qabstractitemmodeldatalist'):
                bytearray = data.data('application/x-qabstractitemmodeldatalist')
                data_items = self.decode_data(bytearray)

                # Find block in tree so that we can pull out label
                block_key = data_items[0][QtCore.Qt.UserRole].value()
                block = self.platform.blocks[block_key]

                # Add block of this key at the cursor position
                cursor_pos = event.scenePos()

                # Pull out its params (keep in mind we still havent added the dialog box that lets you change param values so this is more for show)
                params = []
                for p in block.parameters_data: # block.parameters_data is a list of dicts, one per param
                    if 'label' in p: # for now let's just show it as long as it has a label
                        key = p['label']
                        value = p.get('default', '') # just show default value for now
                        params.append((key, value))
                    
                # Tell the block where to show up on the canvas
                attrib = {'_coordinate':(cursor_pos.x(), cursor_pos.y())}
                
                new_block = Block(block_key, block.label, attrib, params)
                self.addItem(new_block)

                event.setDropAction(Qt.CopyAction)
                event.accept()
            else:
                return QtGui.QStandardItemModel.dropMimeData(self, data, action, row, column, parent)
        else:
            event.ignore()
    
    def mousePressEvent(self,  event):
        if event.button() == Qt.LeftButton:
            self.mousePressed = True
            if self.isPanning:
                #self.setCursor(Qt.ClosedHandCursor)
                self.dragPos = event.pos()
                event.accept()
            else:
                super(FlowgraphScene, self).mousePressEvent(event)

    def mouseMoveEvent(self, event):
        if self.mousePressed and self.isPanning:
            newPos = event.pos()
            diff = newPos - self.dragPos
            self.dragPos = newPos
            self.horizontalScrollBar().setValue(self.horizontalScrollBar().value() - diff.x())
            self.verticalScrollBar().setValue(self.verticalScrollBar().value() - diff.y())
            event.accept()
        else:
            itemUnderMouse = self.itemAt(event.pos(), QtGui.QTransform()) # the 2nd arg lets you transform some items and ignore others
            if  itemUnderMouse is not None:
                #~ print itemUnderMouse
                pass

            super(FlowgraphScene, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            if event.modifiers() & Qt.ControlModifier:
                #self.setCursor(Qt.OpenHandCursor)
                pass
            else:
                self.isPanning = False
                #self.setCursor(Qt.ArrowCursor)
            self.mousePressed = False
        super(FlowgraphScene, self).mouseReleaseEvent(event)

    def mouseDoubleClickEvent(self, event): # Will be used to open up dialog box of a block
        print("You double clicked on a block")


        
        
        

class Flowgraph(QtWidgets.QGraphicsView, base.Controller): # added base.Controller so it can see platform
    def __init__(self, parent, filename=None):
        super(Flowgraph, self).__init__()
        self.setParent(parent)
        self.setAlignment(Qt.AlignLeft|Qt.AlignTop)

        self.scene = FlowgraphScene()

        self.setSceneRect(0,0,DEFAULT_MAX_X, DEFAULT_MAX_Y)
        if filename is not None:
            self.readFile(filename)
        else:
            self.initEmpty()

        self.setScene(self.scene)
        self.setBackgroundBrush(QtGui.QBrush(Qt.white))
        
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

            # Find block in tree so that we can pull out label
            block = self.platform.blocks[block_key]
                
            new_block = Block(block_key, block.label, attrib, params)
            self.scene.addItem(new_block)

        # This part no longer works now that we are using a Scene with GraphicsItems, but I'm sure there's still some way to do it
        #bounds = self.scene.itemsBoundingRect()
        #self.setSceneRect(bounds)
        #self.fitInView(bounds)

    def initEmpty(self):
        self.setSceneRect(0,0,DEFAULT_MAX_X, DEFAULT_MAX_Y)

    def wheelEvent(self,  event):
        factor = 1.1;
        if event.angleDelta().y() < 0:
            factor = 1.0 / factor
        self.scale(factor, factor)

    def mousePressEvent(self,  event):
        if event.button() == Qt.LeftButton:
            self.mousePressed = True
            if self.isPanning:
                self.setCursor(Qt.ClosedHandCursor)
                self.dragPos = event.pos()
                event.accept()
            else:
                # This will pass the mouse move event to the scene
                super(Flowgraph, self).mousePressEvent(event)

    def mouseMoveEvent(self, event):
        if self.mousePressed and self.isPanning:
            newPos = event.pos()
            diff = newPos - self.dragPos
            self.dragPos = newPos
            self.horizontalScrollBar().setValue(self.horizontalScrollBar().value() - diff.x())
            self.verticalScrollBar().setValue(self.verticalScrollBar().value() - diff.y())
            event.accept()
        else:
            # This will pass the mouse move event to the scene
            super(Flowgraph, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            if event.modifiers() & Qt.ControlModifier:
                self.setCursor(Qt.OpenHandCursor)
                pass
            else:
                self.isPanning = False
                self.setCursor(Qt.ArrowCursor)
            self.mousePressed = False
        super(Flowgraph, self).mouseReleaseEvent(event)

    def mouseDoubleClickEvent(self, event): # Will be used to open up dialog box of a block
        pass

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Control and not self.mousePressed:
            self.isPanning = True
            self.setCursor(Qt.OpenHandCursor)
        else:
            super(Flowgraph, self).keyPressEvent(event)

    def keyReleaseEvent(self, event):
        if event.key() == Qt.Key_Control:
            if not self.mousePressed:
                self.isPanning = False
                self.setCursor(Qt.ArrowCursor)
        else:
            super(Flowgraph, self).keyPressEvent(event)
            
    def mouseDoubleClickEvent(self, event):
        # This will pass the double click event to the scene
        super(Flowgraph, self).mouseDoubleClickEvent(event)


