"""
Copyright 2007-2011, 2016q Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import ast
import functools
import random
from shutil import which as find_executable
from itertools import count

from gi.repository import GLib, Gtk

from . import colors
from .drawable import Drawable
from .connection import DummyConnection
from .. import Actions, Constants, Utils, Bars, Dialogs, MainWindow
from ..external_editor import ExternalEditor
from ...core import Messages
from ...core.FlowGraph import FlowGraph as CoreFlowgraph


class _ContextMenu(object):
    """
    Help with drawing the right click context menu
    """

    def __init__(self, main_window):
        self._menu = Gtk.Menu.new_from_model(Bars.ContextMenu())
        self._menu.attach_to_widget(main_window)

        # In GTK 3.22 Menu.popup was deprecated, we want to popup at the
        # pointer, so use that new function instead if we can.
        if Gtk.check_version(3, 22, 0) is None:
            self.popup = self._menu.popup_at_pointer

    def popup(self, event):
        self._menu.popup(None, None, None, None, event.button, event.time)


class FlowGraph(CoreFlowgraph, Drawable):
    """
    FlowGraph is the data structure to store graphical signal blocks,
    graphical inputs and outputs,
    and the connections between inputs and outputs.
    """

    def __init__(self, parent, **kwargs):
        """
        FlowGraph constructor.
        Create a list for signal blocks and connections. Connect mouse handlers.
        """
        super(self.__class__, self).__init__(parent, **kwargs)
        Drawable.__init__(self)

        # We need to get the main window object so the context menu can be to the
        # registered actions
        app = Gtk.Application.get_default()
        main_window = None
        for window in app.get_windows():
            if isinstance(window, MainWindow.MainWindow):
                main_window = window
                break

        self.drawing_area = None
        # important vars dealing with mouse event tracking
        self.element_moved = False
        self.mouse_pressed = False
        self.press_coor = (0, 0)
        # selected
        self.selected_elements = set()
        self._old_selected_port = None
        self._new_selected_port = None
        # current mouse hover element
        self.element_under_mouse = None
        # context menu
        self._context_menu = _ContextMenu(main_window)
        self.get_context_menu = lambda: self._context_menu

        self._new_connection = None
        self._elements_to_draw = []
        self._external_updaters = {}

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

    def install_external_editor(self, param, parent=None):
        target = (param.parent_block.name, param.key)

        if target in self._external_updaters:
            editor = self._external_updaters[target]
        else:
            config = self.parent_platform.config
            editor = (find_executable(config.editor) or
                      Dialogs.choose_editor(parent, config))  # todo: pass in parent
            if not editor:
                return
            updater = functools.partial(
                self.handle_external_editor_change, target=target)
            editor = self._external_updaters[target] = ExternalEditor(
                editor=editor,
                name=target[0], value=param.get_value(),
                callback=functools.partial(GLib.idle_add, updater)
            )
            editor.start()
        try:
            editor.open_editor()
        except Exception as e:
            # Problem launching the editor. Need to select a new editor.
            Messages.send(
                '>>> Error opening an external editor. Please select a different editor.\n')
            # Reset the editor to force the user to select a new one.
            self.parent_platform.config.editor = ''

    def handle_external_editor_change(self, new_value, target):
        try:
            block_id, param_key = target
            self.get_block(block_id).params[param_key].set_value(new_value)

        except (IndexError, ValueError):  # block no longer exists
            self._external_updaters[target].stop()
            del self._external_updaters[target]
            return
        Actions.EXTERNAL_UPDATE()

    def add_new_block(self, key, coor=None):
        """
        Add a block of the given key to this flow graph.

        Args:
            key: the block key
            coor: an optional coordinate or None for random
        """
        id = self._get_unique_id(key)
        scroll_pane = self.drawing_area.get_parent().get_parent()
        # calculate the position coordinate
        h_adj = scroll_pane.get_hadjustment()
        v_adj = scroll_pane.get_vadjustment()
        if coor is None:
            coor = (
                int(random.uniform(.25, .75) *
                    h_adj.get_page_size() + h_adj.get_value()),
                int(random.uniform(.25, .75) *
                    v_adj.get_page_size() + v_adj.get_value()),
            )
        # get the new block
        block = self.new_block(key)
        block.coordinate = coor
        block.params['id'].set_value(id)
        Actions.ELEMENT_CREATE()
        return id

    def make_connection(self):
        """this selection and the last were ports, try to connect them"""
        if self._new_connection and self._new_connection.has_real_sink:
            self._old_selected_port = self._new_connection.source_port
            self._new_selected_port = self._new_connection.sink_port
        if self._old_selected_port and self._new_selected_port:
            try:
                self.connect(self._old_selected_port, self._new_selected_port)
                Actions.ELEMENT_CREATE()
            except Exception as e:
                Messages.send_fail_connection(e)
            self._old_selected_port = None
            self._new_selected_port = None
            return True
        return False

    def update(self):
        """
        Call the top level rewrite and validate.
        Call the top level create labels and shapes.
        """
        self.rewrite()
        self.validate()
        self.update_elements_to_draw()
        self.create_labels()
        self.create_shapes()

    def reload(self):
        """
        Reload flow-graph (with updated blocks)

        Args:
            page: the page to reload (None means current)
        Returns:
            False if some error occurred during import
        """
        success = False
        data = self.export_data()
        if data:
            self.unselect()
            success = self.import_data(data)
            self.update()
        return success

    ###########################################################################
    # Copy Paste
    ###########################################################################
    def copy_to_clipboard(self):
        """
        Copy the selected blocks and connections into the clipboard.

        Returns:
            the clipboard
        """
        # get selected blocks
        blocks = list(self.selected_blocks())
        if not blocks:
            return None
        # calc x and y min
        x_min, y_min = blocks[0].coordinate
        for block in blocks:
            x, y = block.coordinate
            x_min = min(x, x_min)
            y_min = min(y, y_min)
        # get connections between selected blocks
        connections = list(filter(
            lambda c: c.source_block in blocks and c.sink_block in blocks,
            self.connections,
        ))
        clipboard = (
            (x_min, y_min),
            [block.export_data() for block in blocks],
            [connection.export_data() for connection in connections],
        )
        return clipboard

    def paste_from_clipboard(self, clipboard):
        """
        Paste the blocks and connections from the clipboard.

        Args:
            clipboard: the nested data of blocks, connections
        """
        (x_min, y_min), blocks_n, connections_n = clipboard
        # recalc the position
        scroll_pane = self.drawing_area.get_parent().get_parent()
        h_adj = scroll_pane.get_hadjustment()
        v_adj = scroll_pane.get_vadjustment()
        x_off = h_adj.get_value() - x_min + h_adj.get_page_size() / 4
        y_off = v_adj.get_value() - y_min + v_adj.get_page_size() / 4

        if len(self.get_elements()) <= 1:
            x_off, y_off = 0, 0

        # create blocks
        pasted_blocks = {}
        for block_n in blocks_n:
            block_key = block_n.get('id')
            if block_key == 'options':
                continue

            block_name = block_n.get('name')
            # Verify whether a block with this name exists before adding it
            if block_name in (blk.name for blk in self.blocks):
                block_n = block_n.copy()
                block_n['name'] = self._get_unique_id(block_name)

            block = self.new_block(block_key)
            if not block:
                continue  # unknown block was pasted (e.g. dummy block)

            block.import_data(**block_n)
            pasted_blocks[block_name] = block  # that is before any rename

            block.move((x_off, y_off))
            while any(Utils.align_to_grid(block.coordinate) == Utils.align_to_grid(other.coordinate)
                      for other in self.blocks if other is not block):
                block.move((Constants.CANVAS_GRID_SIZE,
                           Constants.CANVAS_GRID_SIZE))
                # shift all following blocks
                x_off += Constants.CANVAS_GRID_SIZE
                y_off += Constants.CANVAS_GRID_SIZE

        self.selected_elements = set(pasted_blocks.values())

        # update before creating connections
        self.update()
        # create connections
        for src_block, src_port, dst_block, dst_port in connections_n:
            source = pasted_blocks[src_block].get_source(src_port)
            sink = pasted_blocks[dst_block].get_sink(dst_port)
            connection = self.connect(source, sink)
            self.selected_elements.add(connection)

    ###########################################################################
    # Modify Selected
    ###########################################################################
    def type_controller_modify_selected(self, direction):
        """
        Change the registered type controller for the selected signal blocks.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        return any([sb.type_controller_modify(direction) for sb in self.selected_blocks()])

    def port_controller_modify_selected(self, direction):
        """
        Change port controller for the selected signal blocks.

        Args:
            direction: +1 or -1

        Returns:
            true for changed
        """
        return any([sb.port_controller_modify(direction) for sb in self.selected_blocks()])

    def change_state_selected(self, new_state):
        """
        Enable/disable the selected blocks.

        Args:
            new_state: a block state

        Returns:
            true if changed
        """
        changed = False
        for block in self.selected_blocks():
            changed |= block.state != new_state
            block.state = new_state
        return changed

    def move_selected(self, delta_coordinate):
        """
        Move the element and by the change in coordinates.

        Args:
            delta_coordinate: the change in coordinates
        """

        # Determine selected blocks top left coordinate
        blocks = list(self.selected_blocks())
        if not blocks:
            return

        min_x, min_y = self.selected_block.coordinate
        for selected_block in blocks:
            x, y = selected_block.coordinate
            min_x, min_y = min(min_x, x), min(min_y, y)

        # Sanitize delta_coordinate so that blocks don't move to negative coordinate
        delta_coordinate = max(
            delta_coordinate[0], -min_x), max(delta_coordinate[1], -min_y)

        # Move selected blocks
        for selected_block in blocks:
            selected_block.move(delta_coordinate)
            self.element_moved = True

    def align_selected(self, calling_action=None):
        """
        Align the selected blocks.

        Args:
            calling_action: the action initiating the alignment

        Returns:
            True if changed, otherwise False
        """
        blocks = list(self.selected_blocks())
        if calling_action is None or not blocks:
            return False

        # compute common boundary of selected objects
        min_x, min_y = max_x, max_y = blocks[0].coordinate
        for selected_block in blocks:
            x, y = selected_block.coordinate
            min_x, min_y = min(min_x, x), min(min_y, y)
            x += selected_block.width
            y += selected_block.height
            max_x, max_y = max(max_x, x), max(max_y, y)
        ctr_x, ctr_y = (max_x + min_x) / 2, (max_y + min_y) / 2

        # align the blocks as requested
        transform = {
            Actions.BLOCK_VALIGN_TOP: lambda x, y, w, h: (x, min_y),
            Actions.BLOCK_VALIGN_MIDDLE: lambda x, y, w, h: (x, ctr_y - h / 2),
            Actions.BLOCK_VALIGN_BOTTOM: lambda x, y, w, h: (x, max_y - h),
            Actions.BLOCK_HALIGN_LEFT: lambda x, y, w, h: (min_x, y),
            Actions.BLOCK_HALIGN_CENTER: lambda x, y, w, h: (ctr_x - w / 2, y),
            Actions.BLOCK_HALIGN_RIGHT: lambda x, y, w, h: (max_x - w, y),
        }.get(calling_action, lambda *args: args)

        for selected_block in blocks:
            x, y = selected_block.coordinate
            w, h = selected_block.width, selected_block.height
            selected_block.coordinate = transform(x, y, w, h)

        return True

    def rotate_selected(self, rotation):
        """
        Rotate the selected blocks by multiples of 90 degrees.

        Args:
            rotation: the rotation in degrees

        Returns:
            true if changed, otherwise false.
        """
        if not any(self.selected_blocks()):
            return False
        # initialize min and max coordinates
        min_x, min_y = max_x, max_y = self.selected_block.coordinate
        # rotate each selected block, and find min/max coordinate
        for selected_block in self.selected_blocks():
            selected_block.rotate(rotation)
            # update the min/max coordinate
            x, y = selected_block.coordinate
            min_x, min_y = min(min_x, x), min(min_y, y)
            max_x, max_y = max(max_x, x), max(max_y, y)
        # calculate center point of selected blocks
        ctr_x, ctr_y = (max_x + min_x) / 2, (max_y + min_y) / 2
        # rotate the blocks around the center point
        for selected_block in self.selected_blocks():
            x, y = selected_block.coordinate
            x, y = Utils.get_rotated_coordinate(
                (x - ctr_x, y - ctr_y), rotation)
            selected_block.coordinate = (x + ctr_x, y + ctr_y)
        return True

    def remove_selected(self):
        """
        Remove selected elements

        Returns:
            true if changed.
        """
        changed = False
        for selected_element in self.selected_elements:
            self.remove_element(selected_element)
            changed = True
        return changed

    def update_selected(self):
        """
        Remove deleted elements from the selected elements list.
        Update highlighting so only the selected are highlighted.
        """
        selected_elements = self.selected_elements
        elements = self.get_elements()
        # remove deleted elements
        for selected in list(selected_elements):
            if selected in elements:
                continue
            selected_elements.remove(selected)
        if self._old_selected_port and self._old_selected_port.parent not in elements:
            self._old_selected_port = None
        if self._new_selected_port and self._new_selected_port.parent not in elements:
            self._new_selected_port = None
        # update highlighting
        for element in elements:
            element.highlighted = element in selected_elements

    ###########################################################################
    # Draw stuff
    ###########################################################################

    def update_elements_to_draw(self):
        hide_disabled_blocks = Actions.TOGGLE_HIDE_DISABLED_BLOCKS.get_active()
        hide_variables = Actions.TOGGLE_HIDE_VARIABLES.get_active()

        def draw_order(elem):
            return elem.highlighted, elem.is_block, elem.enabled

        elements = sorted(self.get_elements(), key=draw_order)
        del self._elements_to_draw[:]

        for element in elements:
            if hide_disabled_blocks and not element.enabled:
                continue  # skip hidden disabled blocks and connections
            if hide_variables and (element.is_variable or element.is_import):
                continue  # skip hidden disabled blocks and connections
            self._elements_to_draw.append(element)

    def create_labels(self, cr=None):
        for element in self._elements_to_draw:
            element.create_labels(cr)

    def create_shapes(self):
        # TODO - this is a workaround for bus ports not having a proper coordinate
        # until the shape is drawn.  The workaround is to draw blocks before connections

        for element in filter(lambda x: x.is_block, self._elements_to_draw):
            element.create_shapes()

        for element in filter(lambda x: not x.is_block, self._elements_to_draw):
            element.create_shapes()

    def _drawables(self):
        # todo: cache that
        show_comments = Actions.TOGGLE_SHOW_BLOCK_COMMENTS.get_active()
        hide_disabled_blocks = Actions.TOGGLE_HIDE_DISABLED_BLOCKS.get_active()
        for element in self._elements_to_draw:
            if element.is_block and show_comments and element.enabled:
                yield element.draw_comment
        if self._new_connection is not None:
            yield self._new_connection.draw
        for element in self._elements_to_draw:
            if element not in self.selected_elements:
                yield element.draw
        for element in self.selected_elements:
            if element.enabled or not hide_disabled_blocks:
                yield element.draw

    def draw(self, cr):
        """Draw blocks connections comment and select rectangle"""
        for draw_element in self._drawables():
            cr.save()
            draw_element(cr)
            cr.restore()

        draw_multi_select_rectangle = (
            self.mouse_pressed and
            (not self.selected_elements or self.drawing_area.ctrl_mask) and
            not self._new_connection
        )
        if draw_multi_select_rectangle:
            x1, y1 = self.press_coor
            x2, y2 = self.coordinate
            x, y = int(min(x1, x2)), int(min(y1, y2))
            w, h = int(abs(x1 - x2)), int(abs(y1 - y2))
            cr.set_source_rgba(
                colors.HIGHLIGHT_COLOR[0],
                colors.HIGHLIGHT_COLOR[1],
                colors.HIGHLIGHT_COLOR[2],
                0.5,
            )
            cr.rectangle(x, y, w, h)
            cr.fill()
            cr.rectangle(x, y, w, h)
            cr.stroke()

    ##########################################################################
    # selection handling
    ##########################################################################
    def update_selected_elements(self):
        """
        Update the selected elements.
        The update behavior depends on the state of the mouse button.
        When the mouse button pressed the selection will change when
        the control mask is set or the new selection is not in the current group.
        When the mouse button is released the selection will change when
        the mouse has moved and the control mask is set or the current group is empty.
        Attempt to make a new connection if the old and ports are filled.
        If the control mask is set, merge with the current elements.
        """
        selected_elements = None
        if self.mouse_pressed:
            new_selections = self.what_is_selected(self.coordinate)
            # update the selections if the new selection is not in the current selections
            # allows us to move entire selected groups of elements
            if not new_selections:
                selected_elements = set()
            elif self.drawing_area.ctrl_mask or self.selected_elements.isdisjoint(new_selections):
                selected_elements = new_selections

            if self._old_selected_port:
                self._old_selected_port.force_show_label = False
                self.create_shapes()
                self.drawing_area.queue_draw()
            elif self._new_selected_port:
                self._new_selected_port.force_show_label = True

        else:  # called from a mouse release
            if not self.element_moved and (not self.selected_elements or self.drawing_area.ctrl_mask) and not self._new_connection:
                selected_elements = self.what_is_selected(
                    self.coordinate, self.press_coor)

        # this selection and the last were ports, try to connect them
        if self.make_connection():
            return

        # update selected elements
        if selected_elements is None:
            return

        # if ctrl, set the selected elements to the union - intersection of old and new
        if self.drawing_area.ctrl_mask:
            self.selected_elements ^= selected_elements
        else:
            self.selected_elements.clear()
            self.selected_elements.update(selected_elements)
        Actions.ELEMENT_SELECT()

    def what_is_selected(self, coor, coor_m=None):
        """
        What is selected?
        At the given coordinate, return the elements found to be selected.
        If coor_m is unspecified, return a list of only the first element found to be selected:
        Iterate though the elements backwards since top elements are at the end of the list.
        If an element is selected, place it at the end of the list so that is is drawn last,
        and hence on top. Update the selected port information.

        Args:
            coor: the coordinate of the mouse click
            coor_m: the coordinate for multi select

        Returns:
            the selected blocks and connections or an empty list
        """
        selected_port = None
        selected = set()
        # check the elements
        for element in reversed(self._elements_to_draw):
            selected_element = element.what_is_selected(coor, coor_m)
            if not selected_element:
                continue
            # update the selected port information
            if selected_element.is_port:
                if not coor_m:
                    selected_port = selected_element
                selected_element = selected_element.parent_block

            selected.add(selected_element)
            if not coor_m:
                break

        if selected_port and selected_port.is_source:
            selected.remove(selected_port.parent_block)
            self._new_connection = DummyConnection(
                selected_port, coordinate=coor)
            self.drawing_area.queue_draw()
        # update selected ports
        if selected_port is not self._new_selected_port:
            self._old_selected_port = self._new_selected_port
            self._new_selected_port = selected_port
        return selected

    def unselect(self):
        """
        Set selected elements to an empty set.
        """
        self.selected_elements.clear()

    def select_all(self):
        """Select all blocks in the flow graph"""
        self.selected_elements.clear()
        self.selected_elements.update(self._elements_to_draw)

    def selected_blocks(self):
        """
        Get a group of selected blocks.

        Returns:
            sub set of blocks in this flow graph
        """
        return (e for e in self.selected_elements if e.is_block)

    @property
    def selected_block(self):
        """
        Get the selected block when a block or port is selected.

        Returns:
            a block or None
        """
        return next(self.selected_blocks(), None)

    def get_selected_elements(self):
        """
        Get the group of selected elements.

        Returns:
            sub set of elements in this flow graph
        """
        return self.selected_elements

    def get_selected_element(self):
        """
        Get the selected element.

        Returns:
            a block, port, or connection or None
        """
        return next(iter(self.selected_elements), None)

    ##########################################################################
    # Event Handlers
    ##########################################################################
    def handle_mouse_context_press(self, coordinate, event):
        """
        The context mouse button was pressed:
        If no elements were selected, perform re-selection at this coordinate.
        Then, show the context menu at the mouse click location.
        """
        selections = self.what_is_selected(coordinate)
        if not selections.intersection(self.selected_elements):
            self.coordinate = coordinate
            self.mouse_pressed = True
            self.update_selected_elements()
            self.mouse_pressed = False
        if self._new_connection:
            self._new_connection = None
            self.drawing_area.queue_draw()
        self._context_menu.popup(event)

    def handle_mouse_selector_press(self, double_click, coordinate):
        """
        The selector mouse button was pressed:
        Find the selected element. Attempt a new connection if possible.
        Open the block params window on a double click.
        Update the selection state of the flow graph.
        """
        self.press_coor = coordinate
        self.coordinate = coordinate
        self.mouse_pressed = True
        if double_click:
            self.unselect()
        self.update_selected_elements()

        if double_click and self.selected_block:
            self.mouse_pressed = False
            Actions.BLOCK_PARAM_MODIFY()

    def handle_mouse_selector_release(self, coordinate):
        """
        The selector mouse button was released:
        Update the state, handle motion (dragging).
        And update the selected flowgraph elements.
        """
        self.coordinate = coordinate
        self.mouse_pressed = False
        if self.element_moved:
            Actions.BLOCK_MOVE()
            self.element_moved = False
        self.update_selected_elements()
        if self._new_connection:
            self._new_connection = None
            self.drawing_area.queue_draw()

    def handle_mouse_motion(self, coordinate):
        """
        The mouse has moved, respond to mouse dragging or notify elements
        Move a selected element to the new coordinate.
        Auto-scroll the scroll bars at the boundaries.
        """
        # to perform a movement, the mouse must be pressed
        # (no longer checking pending events via Gtk.events_pending() - always true in Windows)
        redraw = False
        if not self.mouse_pressed or self._new_connection:
            redraw = self._handle_mouse_motion_move(coordinate)
        if self.mouse_pressed:
            redraw = redraw or self._handle_mouse_motion_drag(coordinate)
        if redraw:
            self.drawing_area.queue_draw()

    def _handle_mouse_motion_move(self, coordinate):
        # only continue if mouse-over stuff is enabled (just the auto-hide port label stuff for now)
        redraw = False
        for element in self._elements_to_draw:
            over_element = element.what_is_selected(coordinate)
            if not over_element:
                continue
            if over_element != self.element_under_mouse:  # over sth new
                if self.element_under_mouse:
                    redraw |= self.element_under_mouse.mouse_out() or False
                self.element_under_mouse = over_element
                redraw |= over_element.mouse_over() or False
            break
        else:
            if self.element_under_mouse:
                redraw |= self.element_under_mouse.mouse_out() or False
                self.element_under_mouse = None
        if not Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active():
            return
        if redraw:
            # self.create_labels()
            self.create_shapes()
        return redraw

    def _handle_mouse_motion_drag(self, coordinate):
        redraw = False
        # remove the connection if selected in drag event
        if len(self.selected_elements) == 1 and self.get_selected_element().is_connection:
            Actions.ELEMENT_DELETE()
            redraw = True

        if self._new_connection:
            e = self.element_under_mouse
            if e and e.is_port and e.is_sink:
                self._new_connection.update(sink_port=self.element_under_mouse)
            else:
                self._new_connection.update(coordinate=coordinate, rotation=0)
            return True
        # move the selected elements and record the new coordinate
        x, y = coordinate
        if not self.drawing_area.ctrl_mask:
            X, Y = self.coordinate
            dX, dY = x - X, y - Y

            if Actions.TOGGLE_SNAP_TO_GRID.get_active() or self.drawing_area.mod1_mask:
                dX, dY = int(round(dX / Constants.CANVAS_GRID_SIZE)
                             ), int(round(dY / Constants.CANVAS_GRID_SIZE))
                dX, dY = dX * Constants.CANVAS_GRID_SIZE, dY * Constants.CANVAS_GRID_SIZE
            else:
                dX, dY = int(round(dX)), int(round(dY))

            if dX != 0 or dY != 0:
                self.move_selected((dX, dY))
                self.coordinate = (X + dX, Y + dY)
                redraw = True
        return redraw

    def get_extents(self):
        show_comments = Actions.TOGGLE_SHOW_BLOCK_COMMENTS.get_active()

        def sub_extents():
            for element in self._elements_to_draw:
                yield element.get_extents()
                if element.is_block and show_comments and element.enabled:
                    yield element.get_extents_comment()

        extent = 10000000, 10000000, 0, 0
        cmps = (min, min, max, max)
        for sub_extent in sub_extents():
            extent = [cmp(xy, e_xy)
                      for cmp, xy, e_xy in zip(cmps, extent, sub_extent)]
        return tuple(extent)
