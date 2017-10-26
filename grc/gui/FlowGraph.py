"""
Copyright 2007-2011 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import functools
import random
from distutils.spawn import find_executable
from itertools import chain, count
from operator import methodcaller

import gobject

from . import Actions, Colors, Constants, Utils, Bars, Dialogs
from .Constants import SCROLL_PROXIMITY_SENSITIVITY, SCROLL_DISTANCE
from .Element import Element
from .external_editor import ExternalEditor

from ..core.FlowGraph import FlowGraph as _Flowgraph
from ..core import Messages


class FlowGraph(Element, _Flowgraph):
    """
    FlowGraph is the data structure to store graphical signal blocks,
    graphical inputs and outputs,
    and the connections between inputs and outputs.
    """

    def __init__(self, **kwargs):
        """
        FlowGraph constructor.
        Create a list for signal blocks and connections. Connect mouse handlers.
        """
        Element.__init__(self)
        _Flowgraph.__init__(self, **kwargs)
        #when is the flow graph selected? (used by keyboard event handler)
        self.is_selected = lambda: bool(self.get_selected_elements())
        #important vars dealing with mouse event tracking
        self.element_moved = False
        self.mouse_pressed = False
        self._selected_elements = []
        self.press_coor = (0, 0)
        #selected ports
        self._old_selected_port = None
        self._new_selected_port = None
        # current mouse hover element
        self.element_under_mouse = None
        #context menu
        self._context_menu = Bars.ContextMenu()
        self.get_context_menu = lambda: self._context_menu

        self._external_updaters = {}

    def _get_unique_id(self, base_id=''):
        """
        Get a unique id starting with the base id.

        Args:
            base_id: the id starts with this and appends a count

        Returns:
            a unique id
        """
        for index in count():
            block_id = '{0}_{1}'.format(base_id, index)
            if block_id not in (b.get_id() for b in self.blocks):
                break
        return block_id

    def install_external_editor(self, param, parent=None):
        target = (param.get_parent().get_id(), param.get_key())

        if target in self._external_updaters:
            editor = self._external_updaters[target]
        else:
            config = self.get_parent().config
            editor = (find_executable(config.editor) or
                      Dialogs.ChooseEditorDialog(config, parent))
            if not editor:
                return
            updater = functools.partial(
                self.handle_external_editor_change, target=target)
            editor = self._external_updaters[target] = ExternalEditor(
                editor=editor,
                name=target[0], value=param.get_value(),
                callback=functools.partial(gobject.idle_add, updater)
            )
            editor.start()
        try:
            editor.open_editor()
        except Exception as e:
            # Problem launching the editor. Need to select a new editor.
            Messages.send('>>> Error opening an external editor. Please select a different editor.\n')
            # Reset the editor to force the user to select a new one.
            self.get_parent().config.editor = ''

    def handle_external_editor_change(self, new_value, target):
        try:
            block_id, param_key = target
            self.get_block(block_id).get_param(param_key).set_value(new_value)

        except (IndexError, ValueError):  # block no longer exists
            self._external_updaters[target].stop()
            del self._external_updaters[target]
            return
        Actions.EXTERNAL_UPDATE()


    ###########################################################################
    # Access Drawing Area
    ###########################################################################
    def get_drawing_area(self): return self.drawing_area
    def queue_draw(self): self.get_drawing_area().queue_draw()
    def get_size(self): return self.get_drawing_area().get_size_request()
    def set_size(self, *args): self.get_drawing_area().set_size_request(*args)
    def get_scroll_pane(self): return self.drawing_area.get_parent()
    def get_ctrl_mask(self): return self.drawing_area.ctrl_mask
    def get_mod1_mask(self): return self.drawing_area.mod1_mask
    def new_pixmap(self, *args): return self.get_drawing_area().new_pixmap(*args)

    def add_new_block(self, key, coor=None):
        """
        Add a block of the given key to this flow graph.

        Args:
            key: the block key
            coor: an optional coordinate or None for random
        """
        id = self._get_unique_id(key)
        #calculate the position coordinate
        W, H = self.get_size()
        h_adj = self.get_scroll_pane().get_hadjustment()
        v_adj = self.get_scroll_pane().get_vadjustment()
        if coor is None: coor = (
            int(random.uniform(.25, .75) * min(h_adj.page_size, W) +
                h_adj.get_value()),
            int(random.uniform(.25, .75) * min(v_adj.page_size, H) +
                v_adj.get_value()),
        )
        #get the new block
        block = self.new_block(key)
        block.set_coordinate(coor)
        block.set_rotation(0)
        block.get_param('id').set_value(id)
        Actions.ELEMENT_CREATE()
        return id

    ###########################################################################
    # Copy Paste
    ###########################################################################
    def copy_to_clipboard(self):
        """
        Copy the selected blocks and connections into the clipboard.

        Returns:
            the clipboard
        """
        #get selected blocks
        blocks = self.get_selected_blocks()
        if not blocks: return None
        #calc x and y min
        x_min, y_min = blocks[0].get_coordinate()
        for block in blocks:
            x, y = block.get_coordinate()
            x_min = min(x, x_min)
            y_min = min(y, y_min)
        #get connections between selected blocks
        connections = filter(
            lambda c: c.get_source().get_parent() in blocks and c.get_sink().get_parent() in blocks,
            self.connections,
        )
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
        selected = set()
        (x_min, y_min), blocks_n, connections_n = clipboard
        old_id2block = dict()
        #recalc the position
        h_adj = self.get_scroll_pane().get_hadjustment()
        v_adj = self.get_scroll_pane().get_vadjustment()
        x_off = h_adj.get_value() - x_min + h_adj.page_size/4
        y_off = v_adj.get_value() - y_min + v_adj.page_size/4
        if len(self.get_elements()) <= 1:
            x_off, y_off = 0, 0
        #create blocks
        for block_n in blocks_n:
            block_key = block_n.find('key')
            if block_key == 'options': continue
            block = self.new_block(block_key)
            if not block:
                continue  # unknown block was pasted (e.g. dummy block)
            selected.add(block)
            #set params
            params = dict((n.find('key'), n.find('value'))
                          for n in block_n.findall('param'))
            if block_key == 'epy_block':
                block.get_param('_io_cache').set_value(params.pop('_io_cache'))
                block.get_param('_source_code').set_value(params.pop('_source_code'))
                block.rewrite()  # this creates the other params
            for param_key, param_value in params.iteritems():
                #setup id parameter
                if param_key == 'id':
                    old_id2block[param_value] = block
                    #if the block id is not unique, get a new block id
                    if param_value in (blk.get_id() for blk in self.blocks):
                        param_value = self._get_unique_id(param_value)
                #set value to key
                block.get_param(param_key).set_value(param_value)
            #move block to offset coordinate
            block.move((x_off, y_off))
        #update before creating connections
        self.update()
        #create connections
        for connection_n in connections_n:
            source = old_id2block[connection_n.find('source_block_id')].get_source(connection_n.find('source_key'))
            sink = old_id2block[connection_n.find('sink_block_id')].get_sink(connection_n.find('sink_key'))
            self.connect(source, sink)
        #set all pasted elements selected
        for block in selected: selected = selected.union(set(block.get_connections()))
        self._selected_elements = list(selected)

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
        return any([sb.type_controller_modify(direction) for sb in self.get_selected_blocks()])

    def port_controller_modify_selected(self, direction):
        """
        Change port controller for the selected signal blocks.

        Args:
            direction: +1 or -1

        Returns:
            true for changed
        """
        return any([sb.port_controller_modify(direction) for sb in self.get_selected_blocks()])

    def enable_selected(self, enable):
        """
        Enable/disable the selected blocks.

        Args:
            enable: true to enable

        Returns:
            true if changed
        """
        changed = False
        for selected_block in self.get_selected_blocks():
            if selected_block.set_enabled(enable): changed = True
        return changed

    def bypass_selected(self):
        """
        Bypass the selected blocks.

        Args:
            None
        Returns:
            true if changed
        """
        changed = False
        for selected_block in self.get_selected_blocks():
            if selected_block.set_bypassed(): changed = True
        return changed

    def move_selected(self, delta_coordinate):
        """
        Move the element and by the change in coordinates.

        Args:
            delta_coordinate: the change in coordinates
        """
        for selected_block in self.get_selected_blocks():
            delta_coordinate = selected_block.bound_move_delta(delta_coordinate)

        for selected_block in self.get_selected_blocks():
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
        blocks = self.get_selected_blocks()
        if calling_action is None or not blocks:
            return False

        # compute common boundary of selected objects
        min_x, min_y = max_x, max_y = blocks[0].get_coordinate()
        for selected_block in blocks:
            x, y = selected_block.get_coordinate()
            min_x, min_y = min(min_x, x), min(min_y, y)
            x += selected_block.W
            y += selected_block.H
            max_x, max_y = max(max_x, x), max(max_y, y)
        ctr_x, ctr_y = (max_x + min_x)/2, (max_y + min_y)/2

        # align the blocks as requested
        transform = {
                Actions.BLOCK_VALIGN_TOP: lambda x, y, w, h: (x, min_y),
                Actions.BLOCK_VALIGN_MIDDLE: lambda x, y, w, h: (x, ctr_y - h/2),
                Actions.BLOCK_VALIGN_BOTTOM: lambda x, y, w, h: (x, max_y - h),
                Actions.BLOCK_HALIGN_LEFT: lambda x, y, w, h: (min_x, y),
                Actions.BLOCK_HALIGN_CENTER: lambda x, y, w, h: (ctr_x-w/2, y),
                Actions.BLOCK_HALIGN_RIGHT: lambda x, y, w, h: (max_x - w, y),
        }.get(calling_action, lambda *args: args)

        for selected_block in blocks:
            x, y = selected_block.get_coordinate()
            w, h = selected_block.W, selected_block.H
            selected_block.set_coordinate(transform(x, y, w, h))

        return True

    def rotate_selected(self, rotation):
        """
        Rotate the selected blocks by multiples of 90 degrees.

        Args:
            rotation: the rotation in degrees

        Returns:
            true if changed, otherwise false.
        """
        if not self.get_selected_blocks():
            return False
        #initialize min and max coordinates
        min_x, min_y = self.get_selected_block().get_coordinate()
        max_x, max_y = self.get_selected_block().get_coordinate()
        #rotate each selected block, and find min/max coordinate
        for selected_block in self.get_selected_blocks():
            selected_block.rotate(rotation)
            #update the min/max coordinate
            x, y = selected_block.get_coordinate()
            min_x, min_y = min(min_x, x), min(min_y, y)
            max_x, max_y = max(max_x, x), max(max_y, y)
        #calculate center point of slected blocks
        ctr_x, ctr_y = (max_x + min_x)/2, (max_y + min_y)/2
        #rotate the blocks around the center point
        for selected_block in self.get_selected_blocks():
            x, y = selected_block.get_coordinate()
            x, y = Utils.get_rotated_coordinate((x - ctr_x, y - ctr_y), rotation)
            selected_block.set_coordinate((x + ctr_x, y + ctr_y))
        return True

    def remove_selected(self):
        """
        Remove selected elements

        Returns:
            true if changed.
        """
        changed = False
        for selected_element in self.get_selected_elements():
            self.remove_element(selected_element)
            changed = True
        return changed

    def draw(self, gc, window):
        """
        Draw the background and grid if enabled.
        Draw all of the elements in this flow graph onto the pixmap.
        Draw the pixmap to the drawable window of this flow graph.
        """

        W, H = self.get_size()
        hide_disabled_blocks = Actions.TOGGLE_HIDE_DISABLED_BLOCKS.get_active()
        hide_variables = Actions.TOGGLE_HIDE_VARIABLES.get_active()

        #draw the background
        gc.set_foreground(Colors.FLOWGRAPH_BACKGROUND_COLOR)
        window.draw_rectangle(gc, True, 0, 0, W, H)

        # draw comments first
        if Actions.TOGGLE_SHOW_BLOCK_COMMENTS.get_active():
            for block in self.blocks:
                if hide_variables and (block.is_variable or block.is_import):
                    continue  # skip hidden disabled blocks and connections
                if block.get_enabled():
                    block.draw_comment(gc, window)
        #draw multi select rectangle
        if self.mouse_pressed and (not self.get_selected_elements() or self.get_ctrl_mask()):
            #coordinates
            x1, y1 = self.press_coor
            x2, y2 = self.get_coordinate()
            #calculate top-left coordinate and width/height
            x, y = int(min(x1, x2)), int(min(y1, y2))
            w, h = int(abs(x1 - x2)), int(abs(y1 - y2))
            #draw
            gc.set_foreground(Colors.HIGHLIGHT_COLOR)
            window.draw_rectangle(gc, True, x, y, w, h)
            gc.set_foreground(Colors.BORDER_COLOR)
            window.draw_rectangle(gc, False, x, y, w, h)
        #draw blocks on top of connections
        blocks = sorted(self.blocks, key=methodcaller('get_enabled'))
        for element in chain(self.connections, blocks):
            if hide_disabled_blocks and not element.get_enabled():
                continue  # skip hidden disabled blocks and connections
            if hide_variables and (element.is_variable or element.is_import):
                continue  # skip hidden disabled blocks and connections
            element.draw(gc, window)
        #draw selected blocks on top of selected connections
        for selected_element in self.get_selected_connections() + self.get_selected_blocks():
            selected_element.draw(gc, window)

    def update_selected(self):
        """
        Remove deleted elements from the selected elements list.
        Update highlighting so only the selected are highlighted.
        """
        selected_elements = self.get_selected_elements()
        elements = self.get_elements()
        #remove deleted elements
        for selected in selected_elements:
            if selected in elements: continue
            selected_elements.remove(selected)
        if self._old_selected_port and self._old_selected_port.get_parent() not in elements:
            self._old_selected_port = None
        if self._new_selected_port and self._new_selected_port.get_parent() not in elements:
            self._new_selected_port = None
        #update highlighting
        for element in elements:
            element.set_highlighted(element in selected_elements)

    def update(self):
        """
        Call the top level rewrite and validate.
        Call the top level create labels and shapes.
        """
        self.rewrite()
        self.validate()
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

    ##########################################################################
    ## Get Selected
    ##########################################################################
    def unselect(self):
        """
        Set selected elements to an empty set.
        """
        self._selected_elements = []

    def select_all(self):
        """Select all blocks in the flow graph"""
        self._selected_elements = list(self.get_elements())

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
        #check the elements
        hide_disabled_blocks = Actions.TOGGLE_HIDE_DISABLED_BLOCKS.get_active()
        hide_variables = Actions.TOGGLE_HIDE_VARIABLES.get_active()
        for element in reversed(self.get_elements()):
            if hide_disabled_blocks and not element.get_enabled():
                continue  # skip hidden disabled blocks and connections
            if hide_variables and (element.is_variable or element.is_import):
                continue  # skip hidden disabled blocks and connections
            selected_element = element.what_is_selected(coor, coor_m)
            if not selected_element:
                continue
            #update the selected port information
            if selected_element.is_port:
                if not coor_m: selected_port = selected_element
                selected_element = selected_element.get_parent()
            selected.add(selected_element)
            #place at the end of the list
            self.get_elements().remove(element)
            self.get_elements().append(element)
            #single select mode, break
            if not coor_m: break
        #update selected ports
        if selected_port is not self._new_selected_port:
            self._old_selected_port = self._new_selected_port
            self._new_selected_port = selected_port
        return list(selected)

    def get_selected_connections(self):
        """
        Get a group of selected connections.

        Returns:
            sub set of connections in this flow graph
        """
        selected = set()
        for selected_element in self.get_selected_elements():
            if selected_element.is_connection:
                selected.add(selected_element)
        return list(selected)

    def get_selected_blocks(self):
        """
        Get a group of selected blocks.

        Returns:
            sub set of blocks in this flow graph
        """
        selected = set()
        for selected_element in self.get_selected_elements():
            if selected_element.is_block:
                selected.add(selected_element)
        return list(selected)

    def get_selected_block(self):
        """
        Get the selected block when a block or port is selected.

        Returns:
            a block or None
        """
        selected_blocks = self.get_selected_blocks()
        return selected_blocks[0] if selected_blocks else None

    def get_selected_elements(self):
        """
        Get the group of selected elements.

        Returns:
            sub set of elements in this flow graph
        """
        return self._selected_elements

    def get_selected_element(self):
        """
        Get the selected element.

        Returns:
            a block, port, or connection or None
        """
        selected_elements = self.get_selected_elements()
        return selected_elements[0] if selected_elements else None

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
            new_selections = self.what_is_selected(self.get_coordinate())
            #update the selections if the new selection is not in the current selections
            #allows us to move entire selected groups of elements
            if self.get_ctrl_mask() or not (
                new_selections and new_selections[0] in self.get_selected_elements()
            ): selected_elements = new_selections
            if self._old_selected_port:
                self._old_selected_port.force_label_unhidden(False)
                self.create_shapes()
                self.queue_draw()
            elif self._new_selected_port:
                self._new_selected_port.force_label_unhidden()
        else:  # called from a mouse release
            if not self.element_moved and (not self.get_selected_elements() or self.get_ctrl_mask()):
                selected_elements = self.what_is_selected(self.get_coordinate(), self.press_coor)
        #this selection and the last were ports, try to connect them
        if self._old_selected_port and self._new_selected_port:
            try:
                self.connect(self._old_selected_port, self._new_selected_port)
                Actions.ELEMENT_CREATE()
            except: Messages.send_fail_connection()
            self._old_selected_port = None
            self._new_selected_port = None
            return
        #update selected elements
        if selected_elements is None: return
        old_elements = set(self.get_selected_elements())
        self._selected_elements = list(set(selected_elements))
        new_elements = set(self.get_selected_elements())
        #if ctrl, set the selected elements to the union - intersection of old and new
        if self.get_ctrl_mask():
            self._selected_elements = list(
                set.union(old_elements, new_elements) - set.intersection(old_elements, new_elements)
            )
        Actions.ELEMENT_SELECT()

    ##########################################################################
    ## Event Handlers
    ##########################################################################
    def handle_mouse_context_press(self, coordinate, event):
        """
        The context mouse button was pressed:
        If no elements were selected, perform re-selection at this coordinate.
        Then, show the context menu at the mouse click location.
        """
        selections = self.what_is_selected(coordinate)
        if not set(selections).intersection(self.get_selected_elements()):
            self.set_coordinate(coordinate)
            self.mouse_pressed = True
            self.update_selected_elements()
            self.mouse_pressed = False
        self._context_menu.popup(None, None, None, event.button, event.time)

    def handle_mouse_selector_press(self, double_click, coordinate):
        """
        The selector mouse button was pressed:
        Find the selected element. Attempt a new connection if possible.
        Open the block params window on a double click.
        Update the selection state of the flow graph.
        """
        self.press_coor = coordinate
        self.set_coordinate(coordinate)
        self.time = 0
        self.mouse_pressed = True
        if double_click: self.unselect()
        self.update_selected_elements()
        #double click detected, bring up params dialog if possible
        if double_click and self.get_selected_block():
            self.mouse_pressed = False
            Actions.BLOCK_PARAM_MODIFY()

    def handle_mouse_selector_release(self, coordinate):
        """
        The selector mouse button was released:
        Update the state, handle motion (dragging).
        And update the selected flowgraph elements.
        """
        self.set_coordinate(coordinate)
        self.time = 0
        self.mouse_pressed = False
        if self.element_moved:
            Actions.BLOCK_MOVE()
            self.element_moved = False
        self.update_selected_elements()

    def handle_mouse_motion(self, coordinate):
        """
        The mouse has moved, respond to mouse dragging or notify elements
        Move a selected element to the new coordinate.
        Auto-scroll the scroll bars at the boundaries.
        """
        #to perform a movement, the mouse must be pressed
        # (no longer checking pending events via gtk.events_pending() - always true in Windows)
        if not self.mouse_pressed:
            # only continue if mouse-over stuff is enabled (just the auto-hide port label stuff for now)
            if not Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active(): return
            redraw = False
            for element in reversed(self.get_elements()):
                over_element = element.what_is_selected(coordinate)
                if not over_element: continue
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
            if redraw:
                #self.create_labels()
                self.create_shapes()
                self.queue_draw()
        else:
            #perform auto-scrolling
            width, height = self.get_size()
            x, y = coordinate
            h_adj = self.get_scroll_pane().get_hadjustment()
            v_adj = self.get_scroll_pane().get_vadjustment()
            for pos, length, adj, adj_val, adj_len in (
                (x, width, h_adj, h_adj.get_value(), h_adj.page_size),
                (y, height, v_adj, v_adj.get_value(), v_adj.page_size),
            ):
                #scroll if we moved near the border
                if pos-adj_val > adj_len-SCROLL_PROXIMITY_SENSITIVITY and adj_val+SCROLL_DISTANCE < length-adj_len:
                    adj.set_value(adj_val+SCROLL_DISTANCE)
                    adj.emit('changed')
                elif pos-adj_val < SCROLL_PROXIMITY_SENSITIVITY:
                    adj.set_value(adj_val-SCROLL_DISTANCE)
                    adj.emit('changed')
            #remove the connection if selected in drag event
            if len(self.get_selected_elements()) == 1 and self.get_selected_element().is_connection:
                Actions.ELEMENT_DELETE()
            #move the selected elements and record the new coordinate
            if not self.get_ctrl_mask():
                X, Y = self.get_coordinate()
                dX, dY = int(x - X), int(y - Y)
                active = Actions.TOGGLE_SNAP_TO_GRID.get_active() or self.get_mod1_mask()
                if not active or abs(dX) >= Utils.CANVAS_GRID_SIZE or abs(dY) >= Utils.CANVAS_GRID_SIZE:
                    self.move_selected((dX, dY))
                    self.set_coordinate((x, y))
            #queue draw for animation
            self.queue_draw()
