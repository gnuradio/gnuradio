from __future__ import absolute_import, print_function

# Standard modules
import logging

import xml.etree.ElementTree as ET

from ast import literal_eval
from qtpy import QtGui, QtCore, QtWidgets
from qtpy.QtCore import Qt

# Custom modules
from .canvas.block import Block
from .. import base
from .canvas.flowgraph import FlowgraphScene, Flowgraph
from .canvas.colors import LIGHT_FLOWGRAPH_BACKGROUND_COLOR, DARK_FLOWGRAPH_BACKGROUND_COLOR

from ...core.generator import Generator

# Logging
log = logging.getLogger(__name__)

DEFAULT_MAX_X = 400
DEFAULT_MAX_Y = 300


class FlowgraphView(
    QtWidgets.QGraphicsView, base.Component
):  # added base.Component so it can see platform
    def __init__(self, parent, platform, filename=None):
        super(FlowgraphView, self).__init__()
        self.setParent(parent)
        self.setAlignment(Qt.AlignLeft | Qt.AlignTop)

        self.setScene(FlowgraphScene(self, platform))

        self.scalefactor = 0.8
        self.scale(self.scalefactor, self.scalefactor)

        self.setSceneRect(0, 0, DEFAULT_MAX_X, DEFAULT_MAX_Y)
        if filename is not None:
            self.readFile(filename)
        else:
            self.initEmpty()

        self.fitInView(self.scene().sceneRect(), QtCore.Qt.KeepAspectRatio)
        if self.app.theme == "dark":
            self.setBackgroundBrush(QtGui.QBrush(DARK_FLOWGRAPH_BACKGROUND_COLOR))
        else:
            self.setBackgroundBrush(QtGui.QBrush(LIGHT_FLOWGRAPH_BACKGROUND_COLOR))

        self.isPanning = False
        self.mousePressed = False

        self.setDragMode(self.RubberBandDrag)

        self.generator = None
        self.process = None

    def createActions(self, actions):
        log.debug("Creating actions")

    def contextMenuEvent(self, event):
        super(FlowgraphView, self).contextMenuEvent(event)

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

    def get_generator(self) -> Generator:
        return self.generator

    def process_is_done(self) -> bool:
        if self.process is None:
            return True
        else:
            return (False if self.process.returncode is None else True)

    def readFile(self, filename):
        tree = ET.parse(filename)

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

            #     old_pos = self.mapToScene(event.pos())

            #     self.scale(factor, factor)
            #     new_pos = self.mapToScene(event.pos())

            #     delta = new_pos - old_pos
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
            new_pos = event.pos()
            diff = new_pos - self.dragPos
            self.dragPos = new_pos
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
