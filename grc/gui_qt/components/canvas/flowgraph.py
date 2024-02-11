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

from qtpy import QtGui, QtCore, QtWidgets, QT6
from qtpy.QtCore import Qt

from itertools import count

# Custom modules
from ....core.base import Element
from .connection import DummyConnection
from .port import GUIPort
from ... import base
from ....core.FlowGraph import FlowGraph as CoreFlowgraph
from ... import Utils

from .block import GUIBlock

# Logging
log = logging.getLogger(f"grc.application.{__name__}")

DEFAULT_MAX_X = 400
DEFAULT_MAX_Y = 300


class Flowgraph(CoreFlowgraph):
    def __init__(self, gui, platform, *args, **kwargs):
        self.gui = gui
        CoreFlowgraph.__init__(self, platform)


class FlowgraphScene(QtWidgets.QGraphicsScene, base.Component):
    itemMoved = QtCore.Signal([QtCore.QPointF])
    newElement = QtCore.Signal([Element])
    deleteElement = QtCore.Signal([Element])
    blockPropsChange = QtCore.Signal([Element])

    def __init__(self, view, platform, *args, **kwargs):
        self.core = Flowgraph(self, platform)
        super(FlowgraphScene, self).__init__()
        self.setParent(view)
        self.view = view
        self.isPanning = False
        self.mousePressed = False
        self.moving_blocks = False

        self.dummy_arrow = None
        self.start_port = None
        self._elements_to_draw = []

        if QT6:
            self.undoStack = QtGui.QUndoStack(self)
        else:
            self.undoStack = QtWidgets.QUndoStack(self)
        self.undoAction = self.undoStack.createUndoAction(self, "Undo")
        self.redoAction = self.undoStack.createRedoAction(self, "Redo")

        self.filename = None

        self.clickPos = None

        self.saved = False
        self.save_allowed = True

    def set_saved(self, saved):
        self.saved = saved

    def update(self):
        """
        Call the top level rewrite and validate.
        Call the top level create labels and shapes.
        """
        self.core.rewrite()
        self.core.validate()
        for block in self.core.blocks:
            block.gui.create_shapes_and_labels()
        self.update_elements_to_draw()
        self.app.VariableEditor.update_gui(self.core.blocks)

    def update_elements_to_draw(self):
        # hide_disabled_blocks = Actions.TOGGLE_HIDE_DISABLED_BLOCKS.get_active()
        hide_disabled_blocks = False
        # hide_variables = Actions.TOGGLE_HIDE_VARIABLES.get_active()
        hide_variables = False

        def draw_order(elem):
            return elem.gui.isSelected(), elem.is_block, elem.enabled

        elements = sorted(self.core.get_elements(), key=draw_order)
        del self._elements_to_draw[:]

        for element in elements:
            if hide_disabled_blocks and not element.enabled:
                continue  # skip hidden disabled blocks and connections
            if hide_variables and (element.is_variable or element.is_import):
                continue  # skip hidden disabled blocks and connections
            self._elements_to_draw.append(element)

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

    def _get_unique_id(self, base_id=""):
        """
        Get a unique id starting with the base id.

        Args:
            base_id: the id starts with this and appends a count

        Returns:
            a unique id
        """
        block_ids = set(b.name for b in self.core.blocks)
        for index in count():
            block_id = "{}_{}".format(base_id, index)
            if block_id not in block_ids:
                break
        return block_id

    def dropEvent(self, event):
        QtWidgets.QGraphicsScene.dropEvent(self, event)
        if event.mimeData().hasUrls:
            data = event.mimeData()
            if data.hasFormat("application/x-qabstractitemmodeldatalist"):
                bytearray = data.data("application/x-qabstractitemmodeldatalist")
                data_items = self.decode_data(bytearray)

                # Find block in tree so that we can pull out label
                block_key = data_items[0][QtCore.Qt.UserRole].value()

                # Add block of this key at the cursor position
                cursor_pos = event.scenePos()
                pos = (cursor_pos.x(), cursor_pos.y())

                self.add_block(block_key, pos)

                event.setDropAction(Qt.CopyAction)
                event.accept()
            else:
                return QtGui.QStandardItemModel.dropMimeData(
                    self, data, action, row, column, parent
                )
        else:
            event.ignore()

    def add_block(self, block_key, pos=(0, 0)):
        block = self.platform.blocks[block_key]
        # Pull out its params (keep in mind we still havent added the dialog box that lets you change param values so this is more for show)
        params = []
        for (
            p
        ) in (
            block.parameters_data
        ):  # block.parameters_data is a list of dicts, one per param
            if "label" in p:  # for now let's just show it as long as it has a label
                key = p["label"]
                value = p.get("default", "")  # just show default value for now
                params.append((key, value))

        id = self._get_unique_id(block_key)

        c_block = self.core.new_block(block_key)
        g_block = c_block.gui
        c_block.states["coordinate"] = pos
        g_block.setPos(*pos)
        c_block.params["id"].set_value(id)
        self.addItem(g_block)
        g_block.moveToTop()
        self.update()
        self.newElement.emit(c_block)

    def selected_blocks(self) -> list[GUIBlock]:
        blocks = []
        for item in self.selectedItems():
            if item.core.is_block:
                blocks.append(item)
        return blocks

    def selected_connections(self):
        conns = []
        for item in self.selectedItems():
            if item.core.is_connection:
                conns.append(item)
        return conns

    def delete_selected(self):
        for item in self.selectedItems():
            self.remove_element(item)

    def select_all(self):
        for block in self.core.blocks:
            block.gui.setSelected(True)
        for conn in self.core.connections:
            conn.gui.setSelected(True)

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
        # initialize min and max coordinates
        min_x, min_y = max_x, max_y = selected_blocks[0].x(), selected_blocks[0].y()
        # rotate each selected block, and find min/max coordinate
        for selected_block in selected_blocks:
            selected_block.rotate(rotation)
            # update the min/max coordinate
            x, y = selected_block.x(), selected_block.y()
            min_x, min_y = min(min_x, x), min(min_y, y)
            max_x, max_y = max(max_x, x), max(max_y, y)
        # calculate center point of selected blocks
        ctr_x, ctr_y = (max_x + min_x) / 2, (max_y + min_y) / 2
        # rotate the blocks around the center point
        for selected_block in selected_blocks:
            x, y = selected_block.x(), selected_block.y()
            x, y = Utils.get_rotated_coordinate((x - ctr_x, y - ctr_y), rotation)
            selected_block.setPos(x + ctr_x, y + ctr_y)
        return True

    def mousePressEvent(self, event):
        g_item = self.itemAt(event.scenePos(), QtGui.QTransform())
        self.clickPos = event.scenePos()
        selected = self.selectedItems()
        self.moving_blocks = False

        if g_item and not isinstance(g_item, DummyConnection):
            c_item = g_item.core
            if c_item.is_block:
                self.moving_blocks = True
            elif c_item.is_port:
                new_con = None
                if len(selected) == 1:
                    if selected[0].core.is_port and selected[0] != g_item:
                        if selected[0].core.is_source and c_item.is_sink:
                            new_con = self.core.connect(selected[0].core, c_item)
                        elif selected[0].is_sink and c_item.is_source:
                            new_con = self.core.connect(c_item, selected[0].core)
                if new_con:
                    log.debug("Created connection (click)")
                    self.addItem(new_con.gui)
                    self.newElement.emit(new_con)
                    self.update()
                else:
                    self.start_port = g_item
                    if c_item.is_source:
                        self.dummy_arrow = DummyConnection(self, g_item.connection_point, event.scenePos())
                        self.addItem(self.dummy_arrow)
        if event.button() == Qt.LeftButton:
            self.mousePressed = True
            super(FlowgraphScene, self).mousePressEvent(event)

    def mouseMoveEvent(self, event):
        self.view.setSceneRect(self.itemsBoundingRect())
        if self.dummy_arrow:
            self.dummy_arrow.update(event.scenePos())

        if self.mousePressed and self.isPanning:
            new_pos = event.pos()
            self.dragPos = new_pos
            event.accept()
        else:
            super(FlowgraphScene, self).mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        if self.dummy_arrow:  # We are currently dragging a DummyConnection
            g_item = self.itemAt(event.scenePos(), QtGui.QTransform())
            if isinstance(g_item, GUIPort):
                c_item = g_item.core
                if g_item != self.start_port:
                    log.debug("Created connection (drag)")
                    new_con = self.core.connect(self.start_port.core, c_item)
                    self.addItem(new_con.gui)
                    self.newElement.emit(new_con)
                    self.update()
            self.removeItem(self.dummy_arrow)
            self.dummy_arrow = None
        else:
            if self.clickPos != event.scenePos() and self.moving_blocks:
                self.itemMoved.emit(event.scenePos() - self.clickPos)
        super(FlowgraphScene, self).mouseReleaseEvent(event)

    def createActions(self, actions):
        log.debug("Creating actions")

        """
        # File Actions
        actions['save'] = Action(Icons("document-save"), _("save"), self,
                                shortcut=Keys.New, statusTip=_("save-tooltip"))

        actions['clear'] = Action(Icons("document-close"), _("clear"), self,
                                         shortcut=Keys.Open, statusTip=_("clear-tooltip"))
        """

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

    def import_data(self, data):
        self.core.import_data(data)
        for conn in self.core.connections:
            self.addItem(conn.gui)
        for block in self.core.blocks:
            self.addItem(block.gui)

    def getMaxZValue(self):
        z_values = []
        for block in self.core.blocks:
            z_values.append(block.gui.zValue())
        return max(z_values)

    def remove_element(self, element: GUIBlock):
        self.removeItem(element)
        self.core.remove_element(element.core)

    def get_extents(self):
        # show_comments = Actions.TOGGLE_SHOW_BLOCK_COMMENTS.get_active()
        show_comments = True

        def sub_extents():
            for element in self._elements_to_draw:
                yield element.get_extents()
                if element.is_block and show_comments and element.enabled:
                    yield element.get_extents_comment()

        extent = 10000000, 10000000, 0, 0
        cmps = (min, min, max, max)
        for sub_extent in sub_extents():
            extent = [cmp(xy, e_xy) for cmp, xy, e_xy in zip(cmps, extent, sub_extent)]
        return tuple(extent)

    def copy_to_clipboard(self):
        """
        Copy the selected blocks and connections into the clipboard.

        Returns:
            the clipboard
        """
        # get selected blocks
        g_blocks = list(self.selected_blocks())
        if not g_blocks:
            return None
        # calc x and y min
        x_min, y_min = g_blocks[0].core.states["coordinate"]
        for g_block in g_blocks:
            x, y = g_block.core.states["coordinate"]
            x_min = min(x, x_min)
            y_min = min(y, y_min)
        # get connections between selected blocks
        connections = list(
            filter(
                lambda c: c.source_block in g_blocks and c.sink_block in g_blocks,
                self.core.connections,
            )
        )
        clipboard = (
            (x_min, y_min),
            [g_block.core.export_data() for g_block in g_blocks],
            [connection.core.export_data() for connection in connections],
        )
        return clipboard

    def paste_from_clipboard(self, clipboard):
        """
        Paste the blocks and connections from the clipboard.

        Args:
            clipboard: the nested data of blocks, connections
        """
        self.clearSelection()
        (x_min, y_min), blocks_n, connections_n = clipboard
        """
        # recalc the position
        scroll_pane = self.drawing_area.get_parent().get_parent()
        h_adj = scroll_pane.get_hadjustment()
        v_adj = scroll_pane.get_vadjustment()
        x_off = h_adj.get_value() - x_min + h_adj.get_page_size() / 4
        y_off = v_adj.get_value() - y_min + v_adj.get_page_size() / 4

        if len(self.get_elements()) <= 1:
            x_off, y_off = 0, 0
        """
        x_off, y_off = 10, 10

        # create blocks
        pasted_blocks = {}
        for block_n in blocks_n:
            block_key = block_n.get("id")
            if block_key == "options":
                continue

            block_name = block_n.get("name")
            # Verify whether a block with this name exists before adding it
            if block_name in (blk.name for blk in self.core.blocks):
                block_n = block_n.copy()
                block_n["name"] = self._get_unique_id(block_name)

            block = self.core.new_block(block_key)
            if not block:
                continue  # unknown block was pasted (e.g. dummy block)

            block.import_data(**block_n)
            pasted_blocks[block_name] = block  # that is before any rename

            block.gui.moveBy(x_off, y_off)
            self.addItem(block.gui)
            block.gui.moveToTop()
            block.gui.setSelected(True)
            """
            while any(Utils.align_to_grid(block.states['coordinate']) == Utils.align_to_grid(other.states['coordinate'])
                      for other in self.blocks if other is not block):
                block.moveBy(Constants.CANVAS_GRID_SIZE,
                           Constants.CANVAS_GRID_SIZE)
                # shift all following blocks
                x_off += Constants.CANVAS_GRID_SIZE
                y_off += Constants.CANVAS_GRID_SIZE
            """

        # update before creating connections
        self.update()
        # create connections
        for src_block, src_port, dst_block, dst_port in connections_n:
            source = pasted_blocks[src_block].get_source(src_port)
            sink = pasted_blocks[dst_block].get_sink(dst_port)
            connection = self.connect(source, sink)
            connection.setSelected(True)

    def itemsBoundingRect(self):
        rect = QtWidgets.QGraphicsScene.itemsBoundingRect(self)
        return QtCore.QRectF(0.0, 0.0, rect.right(), rect.bottom())
