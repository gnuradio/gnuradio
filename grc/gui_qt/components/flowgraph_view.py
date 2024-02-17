from __future__ import absolute_import, print_function

# Standard modules
import logging

import xml.etree.ElementTree as ET

from ast import literal_eval

# Third-party modules
import six

from qtpy import QtGui, QtCore, QtWidgets
from qtpy.QtCore import Qt

from itertools import count

# Custom modules
from .canvas.block import Block
from .canvas.port import Port
from ...core.base import Element
from .canvas.connection import ConnectionArrow, Connection
from .. import base
from ...core.FlowGraph import FlowGraph as CoreFlowgraph
from .. import Utils
from .flowgraph import Flowgraph

# Logging
log = logging.getLogger(__name__)

DEFAULT_MAX_X = 400
DEFAULT_MAX_Y = 300


class FlowgraphView(
    QtWidgets.QGraphicsView, base.Component
):  # added base.Component so it can see platform
    def __init__(self, parent, filename=None):
        super(FlowgraphView, self).__init__()
        self.setParent(parent)
        self.setAlignment(Qt.AlignLeft | Qt.AlignTop)

        self.flowgraph = Flowgraph(self)

        self.scalefactor = 0.8
        self.scale(self.scalefactor, self.scalefactor)

        self.setSceneRect(0, 0, DEFAULT_MAX_X, DEFAULT_MAX_Y)
        if filename is not None:
            self.readFile(filename)
        else:
            self.initEmpty()

        self.setScene(self.flowgraph)
        self.fitInView(self.flowgraph.sceneRect(), QtCore.Qt.KeepAspectRatio)
        if self.app.qsettings.value("appearance/theme") == "dark":
            self.setBackgroundBrush(QtGui.QBrush(QtGui.QColor(25, 35, 45)))
        else:
            self.setBackgroundBrush(QtGui.QBrush(QtGui.QColor(255, 255, 255)))

        self.isPanning = False
        self.mousePressed = False

        self.setDragMode(self.RubberBandDrag)

    def createActions(self, actions):
        log.debug("Creating actions")

    def contextMenuEvent(self, event):
        super(FlowgraphView, self).contextMenuEvent(event)

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

    def readFile(self, filename):
        tree = ET.parse(filename)
        root = tree.getroot()
        blocks = {}

        for xml_block in tree.findall("block"):
            attrib = {}
            params = []
            block_key = xml_block.find("key").text

            for param in xml_block.findall("param"):
                key = param.find("key").text
                value = param.find("value").text
                if key.startswith("_"):
                    attrib[key] = literal_eval(value)
                else:
                    params.append((key, value))

            # Find block in tree so that we can pull out label
            try:
                block = self.platform.blocks[block_key]

                new_block = Block(block_key, block.label, attrib, params)
                self.scene.addItem(new_block)
                self.newElement.emit(new_block)
            except:
                log.warning("Block '{}' was not found".format(block_key))

        # This part no longer works now that we are using a Scene with GraphicsItems, but I'm sure there's still some way to do it
        # bounds = self.scene.itemsBoundingRect()
        # self.setSceneRect(bounds)
        # self.fitInView(bounds)

    def initEmpty(self):
        self.setSceneRect(0, 0, DEFAULT_MAX_X, DEFAULT_MAX_Y)

    def zoom(self, factor: float, anchor=QtWidgets.QGraphicsView.AnchorViewCenter):
        new_scalefactor = self.scalefactor * factor

        if new_scalefactor > 0.25 and new_scalefactor < 2.5:
            self.scalefactor = new_scalefactor
            self.setTransformationAnchor(anchor)
            self.setResizeAnchor(anchor)
            self.scale(factor, factor)

    def zoomOriginal(self):
        # TODO: Original scale factor as a constant?
        self.zoom(0.8 / self.scalefactor)

    def wheelEvent(self, event):
        # TODO: Support multi touch drag and drop for scrolling through the view
        if event.modifiers() == Qt.ControlModifier:
            factor = 1.1 if event.angleDelta().y() > 0 else (1.0 / 1.1)
            self.zoom(factor, anchor=QtWidgets.QGraphicsView.AnchorUnderMouse)

            # if new_scalefactor > 0.25 and new_scalefactor < 2.5:
            #     self.scalefactor = new_scalefactor
            #     self.setTransformationAnchor(QtWidgets.QGraphicsView.NoAnchor)
            #     self.setResizeAnchor(QtWidgets.QGraphicsView.NoAnchor)

            #     oldPos = self.mapToScene(event.pos())

            #     self.scale(factor, factor)
            #     newPos = self.mapToScene(event.pos())

            #     delta = newPos - oldPos
            #     self.translate(delta.x(), delta.y())

        elif event.modifiers() == Qt.ShiftModifier:
            self.horizontalScrollBar().setValue(
                self.horizontalScrollBar().value() - event.angleDelta().y())
        else:
            QtWidgets.QGraphicsView.wheelEvent(self, event)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.mousePressed = True
            # This will pass the mouse move event to the scene
            super(FlowgraphView, self).mousePressEvent(event)

    def mouseMoveEvent(self, event):
        if self.mousePressed and self.isPanning:
            newPos = event.pos()
            diff = newPos - self.dragPos
            self.dragPos = newPos
            self.horizontalScrollBar().setValue(
                self.horizontalScrollBar().value() - diff.x()
            )
            self.verticalScrollBar().setValue(
                self.verticalScrollBar().value() - diff.y()
            )
            event.accept()
        else:
            # This will pass the mouse move event to the scene
            super(FlowgraphView, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.mousePressed = False
        super(FlowgraphView, self).mouseReleaseEvent(event)

    def keyPressEvent(self, event):
        super(FlowgraphView, self).keyPressEvent(event)

    def keyReleaseEvent(self, event):
        super(FlowgraphView, self).keyPressEvent(event)

    def mouseDoubleClickEvent(self, event):
        # This will pass the double click event to the scene
        super(FlowgraphView, self).mouseDoubleClickEvent(event)
