# Copyright 2014-2020 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import, print_function

# Standard modules
import logging

import xml.etree.ElementTree as ET

from ast import literal_eval

# Third-party modules
import six

from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

from itertools import count

# Custom modules
from .canvas.block import Block
from .canvas.port import Port
from ...core.base import Element
from .canvas.connection import Connection
from .. import base
from ...core.FlowGraph import FlowGraph as CoreFlowgraph
from .. import Utils
from .undoable_actions import MoveCommand

# Logging
log = logging.getLogger(__name__)

DEFAULT_MAX_X = 1280
DEFAULT_MAX_Y = 1024


# TODO: Combine the scene and view? Maybe the scene should be the controller?
class Flowgraph(QtWidgets.QGraphicsScene, base.Component, CoreFlowgraph):
    def __init__(self, *args, **kwargs):
        super(Flowgraph, self).__init__()
        self.parent = self.platform
        self.parent_platform = self.platform
        CoreFlowgraph.__init__(self, self.platform)
        self.isPanning    = False
        self.mousePressed = False
        
        self.newConnection = None
        self.startPort = None

        self.undoStack = QtWidgets.QUndoStack()
        self.undoAction = self.undoStack.createUndoAction(self, "Undo")
        self.redoAction = self.undoStack.createRedoAction(self, "Redo")

    def update(self):
        """
        Call the top level rewrite and validate.
        Call the top level create labels and shapes.  
        """
        self.rewrite()
        self.validate()
        for block in self.blocks:
            block.create_shapes_and_labels()
        #self.update_elements_to_draw()
        #self.create_labels()
        #self.create_shapes()

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
    
    def _get_unique_id(self, base_id=''):
        """
        Get a unique id starting with the base id.

        Args:
            base_id: the id starts with this and appends a count

        Returns:
            a unique id
        """
        block_ids = set(b.name for b in self.blocks)
        for index in count():
            block_id = '{}_{}'.format(base_id, index)
            if block_id not in block_ids:
                break
        return block_id

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

                id = self._get_unique_id(block_key)
                
                block = self.new_block(block_key, attrib=attrib)
                block.states['coordinate'] = attrib['_coordinate']
                block.setPos(cursor_pos.x(), cursor_pos.y())
                block.params['id'].set_value(id)
                self.addItem(block)
                block.moveToTop()
                self.update()

                event.setDropAction(Qt.CopyAction)
                event.accept()
            else:
                return QtGui.QStandardItemModel.dropMimeData(self, data, action, row, column, parent)
        else:
            event.ignore()

    def selected_blocks(self):
        blocks = []
        for item in self.selectedItems():
            if item.is_block:
                blocks.append(item)
        return blocks

    def delete_selected(self):
        for item in self.selectedItems():
            self.remove_element(item)

    def rotate_selected(self, rotation):
        """
        Rotate the selected blocks by multiples of 90 degrees.
        Args:
            rotation: the rotation in degrees
        Returns:
            true if changed, otherwise false.
        """
        selected_blocks = self.selected_blocks()
        if not any(selected_blocks):
            return False
        #initialize min and max coordinates
        min_x, min_y = max_x, max_y = selected_blocks[0].x(),selected_blocks[0].y()
        # rotate each selected block, and find min/max coordinate
        for selected_block in selected_blocks:
            selected_block.rotate(rotation)
            #update the min/max coordinate
            x, y = selected_block.x(),selected_block.y()
            min_x, min_y = min(min_x, x), min(min_y, y)
            max_x, max_y = max(max_x, x), max(max_y, y)
        #calculate center point of selected blocks
        ctr_x, ctr_y = (max_x + min_x)/2, (max_y + min_y)/2
        #rotate the blocks around the center point
        for selected_block in selected_blocks:
            x, y = selected_block.x(),selected_block.y()
            x, y = Utils.get_rotated_coordinate((x - ctr_x, y - ctr_y), rotation)
            selected_block.setPos(x + ctr_x, y + ctr_y)
        return True

    def registerBlockMovement(self, clicked_block):
        # We need to pass the clicked block here because
        # it hasn't been registered as selected yet
        for block in self.selected_blocks() + [clicked_block]:
            block.registerMoveStarting()

    def registerMoveCommand(self, block):
        log.debug('move_cmd')
        for block in self.selected_blocks():
            block.registerMoveEnding()
        moveCommand = MoveCommand(self, self.selected_blocks())
        self.undoStack.push(moveCommand)
        self.app.MainWindow.updateActions()

    def mousePressEvent(self,  event):
        item = self.itemAt(event.scenePos(), QtGui.QTransform())
        if item:
            if item.is_port:
                self.startPort = item
                self.newConnection = QtWidgets.QGraphicsLineItem(QtCore.QLineF(event.scenePos(), event.scenePos()))
                self.newConnection.setPen(QtGui.QPen(1))
                self.addItem(self.newConnection)
                print("clicked a port")
        if event.button() == Qt.LeftButton:
            self.mousePressed = True
            super(Flowgraph, self).mousePressEvent(event)

    def mouseMoveEvent(self, event):
        if self.newConnection:
            newConnection_ = QtCore.QLineF(self.newConnection.line().p1(), event.scenePos())
            self.newConnection.setLine(newConnection_)

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

            super(Flowgraph, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        if self.newConnection:
            item = self.itemAt(event.scenePos(), QtGui.QTransform())
            if isinstance(item, Element):
                if item.is_port and item != self.startPort:
                    log.debug("Connecting two ports")
                    self.connections.add(Connection(self, self.startPort, item))
            self.removeItem(self.newConnection)
            self.newConnection = None
        '''
        if event.button() == Qt.LeftButton:
            if event.modifiers() & Qt.ControlModifier:
                #self.setCursor(Qt.OpenHandCursor)
                pass
            else:
                self.isPanning = False
                #self.setCursor(Qt.ArrowCursor)
            self.mousePressed = False
        '''
        super(Flowgraph, self).mouseReleaseEvent(event)

    def mouseDoubleClickEvent(self, event): # Will be used to open up dialog box of a block
        super(Flowgraph, self).mouseDoubleClickEvent(event)



    def createActions(self, actions):
        log.debug("Creating actions")

        '''
        # File Actions
        actions['save'] = Action(Icons("document-save"), _("save"), self,
                                shortcut=Keys.New, statusTip=_("save-tooltip"))

        actions['clear'] = Action(Icons("document-close"), _("clear"), self,
                                         shortcut=Keys.Open, statusTip=_("clear-tooltip"))
        '''

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")


    def import_data(self, data):
        super(Flowgraph, self).import_data(data)
        for block in self.blocks:
            self.addItem(block)

    def getMaxZValue(self):
        z_values = []
        for block in self.blocks:
             z_values.append(block.zValue())
        return max(z_values)

    def remove_element(self, element):
        self.removeItem(element)
        super(Flowgraph, self).remove_element(element)


class FlowgraphView(QtWidgets.QGraphicsView, base.Component): # added base.Component so it can see platform
    def __init__(self, parent, filename=None):
        super(FlowgraphView, self).__init__()
        self.setParent(parent)
        self.setAlignment(Qt.AlignLeft|Qt.AlignTop)

        self.flowgraph = Flowgraph()

        self.scalefactor = 1.0

        self.setSceneRect(0,0,DEFAULT_MAX_X, DEFAULT_MAX_Y)
        if filename is not None:
            self.readFile(filename)
        else:
            self.initEmpty()

        self.setScene(self.flowgraph)
        self.setBackgroundBrush(QtGui.QBrush(Qt.white))

        self.isPanning    = False
        self.mousePressed = False


        '''
        QGraphicsView.__init__(self, flow_graph, parent)
        self._flow_graph = flow_graph

        self.setFrameShape(QFrame.NoFrame)
        self.setRenderHints(QPainter.Antialiasing |
                            QPainter.SmoothPixmapTransform)
        self.setAcceptDrops(True)
        self.setAlignment(Qt.AlignLeft | Qt.AlignTop)
        self.setSceneRect(0, 0, self.width(), self.height())

        self._dragged_block = None

        #ToDo: Better put this in Block()
        #self.setContextMenuPolicy(Qt.ActionsContextMenu)
        #self.addActions(parent.main_window.menuEdit.actions())
        '''


    def createActions(self, actions):
        log.debug("Creating actions")

        '''
        # File Actions
        actions['save'] = Action(Icons("document-save"), _("save"), self,
                                shortcut=Keys.New, statusTip=_("save-tooltip"))

        actions['clear'] = Action(Icons("document-close"), _("clear"), self,
                                         shortcut=Keys.Open, statusTip=_("clear-tooltip"))
        '''

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

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
            try:
                block = self.platform.blocks[block_key]

                new_block = Block(block_key, block.label, attrib, params)
                self.scene.addItem(new_block)
            except:
                log.warning("Block '{}' was not found".format(block_key))

        # This part no longer works now that we are using a Scene with GraphicsItems, but I'm sure there's still some way to do it
        #bounds = self.scene.itemsBoundingRect()
        #self.setSceneRect(bounds)
        #self.fitInView(bounds)

    def initEmpty(self):
        self.setSceneRect(0,0,DEFAULT_MAX_X, DEFAULT_MAX_Y)

    def wheelEvent(self,  event):
        # TODO: Support multi touch drag and drop for scrolling through the view
        #if event.modifiers() == Qt.ControlModifier:
        if False:
            factor = 1.1

            if event.angleDelta().y() < 0:
                factor = 1.0 / factor

            new_scalefactor = self.scalefactor * factor

            if new_scalefactor > 0.25 and new_scalefactor < 2.5:
                self.scalefactor = new_scalefactor
                self.setTransformationAnchor(QtWidgets.QGraphicsView.NoAnchor)
                self.setResizeAnchor(QtWidgets.QGraphicsView.NoAnchor)

                oldPos = self.mapToScene(event.pos())

                self.scale(factor, factor)
                newPos = self.mapToScene(event.pos())

                delta = newPos - oldPos
                self.translate(delta.x(), delta.y())
        else:
            QtWidgets.QGraphicsView.wheelEvent(self, event)

    def mousePressEvent(self,  event):
        if event.button() == Qt.LeftButton:
            self.mousePressed = True
            # This will pass the mouse move event to the scene
            super(FlowgraphView, self).mousePressEvent(event)

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
            super(FlowgraphView, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.mousePressed = False
        super(FlowgraphView, self).mouseReleaseEvent(event)

    def mouseDoubleClickEvent(self, event): # Will be used to open up dialog box of a block
        pass

    def keyPressEvent(self, event):
        super(FlowgraphView, self).keyPressEvent(event)

    def keyReleaseEvent(self, event):
        super(FlowgraphView, self).keyPressEvent(event)

    def mouseDoubleClickEvent(self, event):
        # This will pass the double click event to the scene
        super(FlowgraphView, self).mouseDoubleClickEvent(event)


    '''
    def dragEnterEvent(self, event):
        key = event.mimeData().text()
        self._dragged_block = self._flow_graph.add_new_block(
            str(key), self.mapToScene(event.pos()))
        event.accept()

    def dragMoveEvent(self, event):
        if self._dragged_block:
            self._dragged_block.setPos(self.mapToScene(event.pos()))
            event.accept()
        else:
            event.ignore()

    def dragLeaveEvent(self, event):
        if self._dragged_block:
            self._flow_graph.remove_element(self._dragged_block)
            self._flow_graph.removeItem(self._dragged_block)

    def dropEvent(self, event):
        self._dragged_block = None
        event.accept()
    '''
