"""
Copyright 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
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

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
from gi.repository import Gdk
from gi.repository import GObject


from Constants import MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT, DND_TARGETS
import Colors


class DrawingArea(Gtk.DrawingArea):
    """
    DrawingArea is the gtk pixel map that graphical elements may draw themselves on.
    The drawing area also responds to mouse and key events.
    """

    def __init__(self, flow_graph):
        """
        DrawingArea contructor.
        Connect event handlers.

        Args:
            main_window: the main_window containing all flow graphs
        """
        self.ctrl_mask = False
        self.mod1_mask = False
        self._flow_graph = flow_graph
        GObject.GObject.__init__(self)
        self.set_size_request(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT)
        self.connect('realize', self._handle_window_realize)
        self.connect('draw', self.draw)
        self.connect('motion-notify-event', self._handle_mouse_motion)
        self.connect('button-press-event', self._handle_mouse_button_press)
        self.connect('button-release-event', self._handle_mouse_button_release)
        self.connect('scroll-event', self._handle_mouse_scroll)
        self.add_events(
            Gdk.EventMask.BUTTON_PRESS_MASK | \
            Gdk.EventMask.POINTER_MOTION_MASK | \
            Gdk.EventMask.BUTTON_RELEASE_MASK | \
            Gdk.EventMask.LEAVE_NOTIFY_MASK | \
            Gdk.EventMask.ENTER_NOTIFY_MASK #| \
            #Gdk.EventMask.FOCUS_CHANGE_MASK
        )
        #setup drag and drop
        self.drag_dest_set(Gtk.DestDefaults.ALL, [], Gdk.DragAction.COPY)
        self.connect('drag-data-received', self._handle_drag_data_received)
        self.drag_dest_set_target_list(None)
        self.drag_dest_add_text_targets()
        #setup the focus flag
        self._focus_flag = False
        self.get_focus_flag = lambda: self._focus_flag
        def _handle_notify_event(widget, event, focus_flag): self._focus_flag = focus_flag
        self.connect('leave-notify-event', _handle_notify_event, False)
        self.connect('enter-notify-event', _handle_notify_event, True)
#        self.set_flags(Gtk.CAN_FOCUS)  # self.set_can_focus(True)
#        self.connect('focus-out-event', self._handle_focus_lost_event)

    ##########################################################################
    ## Handlers
    ##########################################################################
    def _handle_drag_data_received(self, widget, drag_context, x, y, selection_data, info, time):
        """
        Handle a drag and drop by adding a block at the given coordinate.
        """
        self._flow_graph.add_new_block(selection_data.get_text(), (x, y))

    def _handle_mouse_scroll(self, widget, event):
        if event.get_state() & Gdk.ModifierType.SHIFT_MASK:
           if event.direction == Gdk.ScrollDirection.UP:
               event.direction = Gdk.ScrollDirection.LEFT
           else:
               event.direction = Gdk.ScrollDirection.RIGHT

    def _handle_mouse_button_press(self, widget, event):
        """
        Forward button click information to the flow graph.
        """
        self.grab_focus()
        self.ctrl_mask = event.get_state() & Gdk.ModifierType.CONTROL_MASK
        self.mod1_mask = event.get_state() & Gdk.ModifierType.MOD1_MASK
        if event.button == 1: self._flow_graph.handle_mouse_selector_press(
            double_click=(event.type == Gdk.EventType._2BUTTON_PRESS),
            coordinate=(event.x, event.y),
        )
        if event.button == 3: self._flow_graph.handle_mouse_context_press(
            coordinate=(event.x, event.y),
            event=event,
        )

    def _handle_mouse_button_release(self, widget, event):
        """
        Forward button release information to the flow graph.
        """
        self.ctrl_mask = event.get_state() & Gdk.ModifierType.CONTROL_MASK
        self.mod1_mask = event.get_state() & Gdk.ModifierType.MOD1_MASK
        if event.button == 1: self._flow_graph.handle_mouse_selector_release(
            coordinate=(event.x, event.y),
        )

    def _handle_mouse_motion(self, widget, event):
        """
        Forward mouse motion information to the flow graph.
        """
        self.ctrl_mask = event.get_state() & Gdk.ModifierType.CONTROL_MASK
        self.mod1_mask = event.get_state() & Gdk.ModifierType.MOD1_MASK
        self._flow_graph.handle_mouse_motion(
            coordinate=(event.x, event.y),
        )

    def _handle_window_realize(self, widget):
        """
        Called when the window is realized.
        Update the flowgraph, which calls new pixmap.
        """
        self._flow_graph.update()

    def draw(self, widget, cr):
        self._flow_graph.draw(widget, cr)

    def _handle_focus_lost_event(self, widget, event):
        # don't clear selection while context menu is active
        if not self._flow_graph.get_context_menu().flags() & Gtk.VISIBLE:
            self._flow_graph.unselect()
            self._flow_graph.update_selected()
            self._flow_graph.queue_draw()
