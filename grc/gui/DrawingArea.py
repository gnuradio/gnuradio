"""
Copyright 2007–2010 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later
"""

from gi.repository import Gtk, Gdk

from . import Constants, Actions
from .canvas import colors
from .canvas.colors import FLOWGRAPH_BACKGROUND_COLOR, GRID_COLOR


class DrawingArea(Gtk.DrawingArea):
    """
    DrawingArea is the GTK pixel map that graphical elements may draw themselves on.
    The drawing area also responds to mouse and key events.
    """

    def __init__(self, flow_graph):
        Gtk.DrawingArea.__init__(self)

        self._flow_graph = flow_graph
        self.set_property('can_focus', True)

        self.zoom_factor = 1.0
        self._update_after_zoom = False
        self.ctrl_mask = False
        self.mod1_mask = False
        self.button_state = [False] * 10

        # middle mouse panning
        self._old_mouse_coodinates = (0, 0)

        # --- Added: Grid visibility flag (default off, can be loaded from prefs) ---
        self.show_grid = False

        # setup GTK events
        self.connect('realize', self._handle_window_realize)
        self.connect('draw', self.draw)
        self.connect('motion-notify-event', self._handle_mouse_motion)
        self.connect('button-press-event', self._handle_mouse_button_press)
        self.connect('button-release-event', self._handle_mouse_button_release)
        self.connect('scroll-event', self._handle_mouse_scroll)
        self.add_events(
            Gdk.EventMask.BUTTON_PRESS_MASK
            | Gdk.EventMask.POINTER_MOTION_MASK
            | Gdk.EventMask.BUTTON_RELEASE_MASK
            | Gdk.EventMask.SCROLL_MASK
            | Gdk.EventMask.LEAVE_NOTIFY_MASK
            | Gdk.EventMask.ENTER_NOTIFY_MASK
        )

        # setup drag and drop
        self.drag_dest_set(Gtk.DestDefaults.ALL, [], Gdk.DragAction.COPY)
        self.connect('drag-data-received', self._handle_drag_data_received)
        self.drag_dest_set_target_list(None)
        self.drag_dest_add_text_targets()

        # focus flag setup
        self._focus_flag = False
        self.get_focus_flag = lambda: self._focus_flag

        def _handle_notify_event(widget, event, focus_flag):
            self._focus_flag = focus_flag

        self.connect('leave-notify-event', _handle_notify_event, False)
        self.connect('enter-notify-event', _handle_notify_event, True)

        self.set_can_focus(True)
        self.connect('focus-out-event', self._handle_focus_lost_event)

    ##########################################################################
    # Handlers
    ##########################################################################

    def _handle_drag_data_received(self, widget, drag_context, x, y, selection_data, info, time):
        coords = x / self.zoom_factor, y / self.zoom_factor
        self._flow_graph.add_new_block(selection_data.get_text(), coords)

    def zoom_in(self):
        self._set_zoom_factor(min(self.zoom_factor * 1.2, 5.0))

    def zoom_out(self):
        self._set_zoom_factor(max(self.zoom_factor / 1.2, 0.1))

    def reset_zoom(self):
        self._set_zoom_factor(1.0)

    def _set_zoom_factor(self, zoom_factor):
        if zoom_factor != self.zoom_factor:
            self.zoom_factor = zoom_factor
            self._update_after_zoom = True
            self.queue_draw()

    def _middle_mouse_pan(self, event):
        x, y = event.x, event.y
        old_x, old_y = self._old_mouse_coodinates
        scrollbox = self.get_parent().get_parent()

        def scroll(dpos, adj):
            adj_val = adj.get_value()
            if abs(dpos) >= Constants.SCROLL_DISTANCE:
                adj.set_value(adj_val - dpos)
                adj.emit('changed')

        dx = x - old_x
        dy = y - old_y
        scroll(dx, scrollbox.get_hadjustment())
        scroll(dy, scrollbox.get_vadjustment())

    def _handle_mouse_scroll(self, widget, event):
        if event.get_state() & Gdk.ModifierType.CONTROL_MASK:
            if event.direction == Gdk.ScrollDirection.UP:
                self.zoom_in()
            else:
                self.zoom_out()
            return True
        return False

    def _handle_mouse_button_press(self, widget, event):
        self.grab_focus()
        self.ctrl_mask = event.get_state() & Gdk.ModifierType.CONTROL_MASK
        self.mod1_mask = event.get_state() & Gdk.ModifierType.MOD1_MASK
        self.button_state[event.button] = True

        if event.button == 1:
            double_click = (event.type == Gdk.EventType._2BUTTON_PRESS)
            self.button_state[1] = not double_click
            self._flow_graph.handle_mouse_selector_press(
                double_click=double_click,
                coordinate=self._translate_event_coords(event),
            )
        elif event.button == 3:
            self._flow_graph.handle_mouse_context_press(
                coordinate=self._translate_event_coords(event),
                event=event,
            )
        elif event.button == 2:
            self._old_mouse_coodinates = (event.x, event.y)

    def _handle_mouse_button_release(self, widget, event):
        self.ctrl_mask = event.get_state() & Gdk.ModifierType.CONTROL_MASK
        self.mod1_mask = event.get_state() & Gdk.ModifierType.MOD1_MASK
        self.button_state[event.button] = False
        if event.button == 1:
            self._flow_graph.handle_mouse_selector_release(
                coordinate=self._translate_event_coords(event),
            )

    def _handle_mouse_motion(self, widget, event):
        self.ctrl_mask = event.get_state() & Gdk.ModifierType.CONTROL_MASK
        self.mod1_mask = event.get_state() & Gdk.ModifierType.MOD1_MASK

        if self.button_state[1]:
            self._auto_scroll(event)
        elif self.button_state[2]:
            self._middle_mouse_pan(event)

        self._flow_graph.handle_mouse_motion(
            coordinate=self._translate_event_coords(event),
        )

    def _update_size(self):
        w, h = self._flow_graph.get_extents()[2:]
        self.set_size_request(w * self.zoom_factor + 100, h * self.zoom_factor + 100)

    def _auto_scroll(self, event):
        x, y = event.x, event.y
        scrollbox = self.get_parent().get_parent()
        self._update_size()

        def scroll(pos, adj):
            adj_val = adj.get_value()
            adj_len = adj.get_page_size()
            if pos - adj_val > adj_len - Constants.SCROLL_PROXIMITY_SENSITIVITY:
                adj.set_value(adj_val + Constants.SCROLL_DISTANCE)
                adj.emit('changed')
            elif pos - adj_val < Constants.SCROLL_PROXIMITY_SENSITIVITY:
                adj.set_value(adj_val - Constants.SCROLL_DISTANCE)
                adj.emit('changed')

        scroll(x, scrollbox.get_hadjustment())
        scroll(y, scrollbox.get_vadjustment())

    def _handle_window_realize(self, widget):
        self._flow_graph.update()
        self._update_size()

    # --- Added: draw grid ---
    def draw_grid(self, cr):
        grid_size = Constants.GRID_SIZE
        cr.set_source_rgba(*GRID_COLOR)
        width = self.get_allocated_width() / self.zoom_factor
        height = self.get_allocated_height() / self.zoom_factor

        x = 0
        while x < width:
            cr.move_to(x, 0)
            cr.line_to(x, height)
            x += grid_size

        y = 0
        while y < height:
            cr.move_to(0, y)
            cr.line_to(width, y)
            y += grid_size

        cr.set_line_width(1.0)
        cr.stroke()

    def draw(self, widget, cr):
        width = widget.get_allocated_width()
        height = widget.get_allocated_height()

        cr.set_source_rgba(*FLOWGRAPH_BACKGROUND_COLOR)
        cr.rectangle(0, 0, width, height)
        cr.fill()

        cr.scale(self.zoom_factor, self.zoom_factor)
        cr.set_line_width(2.0 / self.zoom_factor)

        # --- Draw grid before flowgraph if enabled ---
        if self.show_grid:
            self.draw_grid(cr)

        if self._update_after_zoom:
            self._flow_graph.create_labels(cr)
            self._flow_graph.create_shapes()
            self._update_size()
            self._update_after_zoom = False

        self._flow_graph.draw(cr)

    def _translate_event_coords(self, event):
        return event.x / self.zoom_factor, event.y / self.zoom_factor

    def _handle_focus_lost_event(self, widget, event):
        if not self._flow_graph.get_context_menu()._menu.get_visible():
            self._flow_graph.unselect()
            self._flow_graph.update_selected()
            self.queue_draw()
            Actions.ELEMENT_SELECT()

    # --- Added: toggle method for menu binding ---
    def toggle_grid_visibility(self, show=None):
        """
        Toggle grid visibility manually or via preferences.
        """
        if show is not None:
            self.show_grid = bool(show)
        else:
            self.show_grid = not self.show_grid
        self.queue_draw()
