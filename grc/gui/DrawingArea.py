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

import pygtk
pygtk.require('2.0')
import gtk

from Constants import MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT, DND_TARGETS
import Colors


class DrawingArea(gtk.DrawingArea):
    """
    DrawingArea is the gtk pixel map that graphical elements may draw themselves on.
    The drawing area also responds to mouse and key events.
    """

    def __init__(self, flow_graph):
        """
        DrawingArea constructor.
        Connect event handlers.

        Args:
            main_window: the main_window containing all flow graphs
        """
        self.ctrl_mask = False
        self.mod1_mask = False
        self._flow_graph = flow_graph
        gtk.DrawingArea.__init__(self)
        self.set_size_request(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT)
        self.connect('realize', self._handle_window_realize)
        self.connect('configure-event', self._handle_window_configure)
        self.connect('expose-event', self._handle_window_expose)
        self.connect('motion-notify-event', self._handle_mouse_motion)
        self.connect('button-press-event', self._handle_mouse_button_press)
        self.connect('button-release-event', self._handle_mouse_button_release)
        self.connect('scroll-event', self._handle_mouse_scroll)
        self.add_events(
            gtk.gdk.BUTTON_PRESS_MASK | \
            gtk.gdk.POINTER_MOTION_MASK | \
            gtk.gdk.BUTTON_RELEASE_MASK | \
            gtk.gdk.LEAVE_NOTIFY_MASK | \
            gtk.gdk.ENTER_NOTIFY_MASK | \
            gtk.gdk.FOCUS_CHANGE_MASK
        )
        #setup drag and drop
        self.drag_dest_set(gtk.DEST_DEFAULT_ALL, DND_TARGETS, gtk.gdk.ACTION_COPY)
        self.connect('drag-data-received', self._handle_drag_data_received)
        #setup the focus flag
        self._focus_flag = False
        self.get_focus_flag = lambda: self._focus_flag
        def _handle_notify_event(widget, event, focus_flag): self._focus_flag = focus_flag
        self.connect('leave-notify-event', _handle_notify_event, False)
        self.connect('enter-notify-event', _handle_notify_event, True)
        self.set_flags(gtk.CAN_FOCUS)  # self.set_can_focus(True)
        self.connect('focus-out-event', self._handle_focus_lost_event)

    def new_pixmap(self, width, height):
        return gtk.gdk.Pixmap(self.window, width, height, -1)

    def get_screenshot(self, transparent_bg=False):
        pixmap = self._pixmap
        W, H = pixmap.get_size()
        pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, 0, 8, W, H)
        pixbuf.fill(0xFF + Colors.FLOWGRAPH_BACKGROUND_COLOR.pixel << 8)
        pixbuf.get_from_drawable(pixmap, pixmap.get_colormap(), 0, 0, 0, 0, W-1, H-1)
        if transparent_bg:
            bgc = Colors.FLOWGRAPH_BACKGROUND_COLOR
            pixbuf = pixbuf.add_alpha(True, bgc.red, bgc.green, bgc.blue)
        return pixbuf


    ##########################################################################
    ## Handlers
    ##########################################################################
    def _handle_drag_data_received(self, widget, drag_context, x, y, selection_data, info, time):
        """
        Handle a drag and drop by adding a block at the given coordinate.
        """
        self._flow_graph.add_new_block(selection_data.data, (x, y))

    def _handle_mouse_scroll(self, widget, event):
        if event.state & gtk.gdk.SHIFT_MASK:
           if event.direction == gtk.gdk.SCROLL_UP:
               event.direction = gtk.gdk.SCROLL_LEFT
           else:
               event.direction = gtk.gdk.SCROLL_RIGHT

    def _handle_mouse_button_press(self, widget, event):
        """
        Forward button click information to the flow graph.
        """
        self.grab_focus()
        self.ctrl_mask = event.state & gtk.gdk.CONTROL_MASK
        self.mod1_mask = event.state & gtk.gdk.MOD1_MASK
        if event.button == 1: self._flow_graph.handle_mouse_selector_press(
            double_click=(event.type == gtk.gdk._2BUTTON_PRESS),
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
        self.ctrl_mask = event.state & gtk.gdk.CONTROL_MASK
        self.mod1_mask = event.state & gtk.gdk.MOD1_MASK
        if event.button == 1: self._flow_graph.handle_mouse_selector_release(
            coordinate=(event.x, event.y),
        )

    def _handle_mouse_motion(self, widget, event):
        """
        Forward mouse motion information to the flow graph.
        """
        self.ctrl_mask = event.state & gtk.gdk.CONTROL_MASK
        self.mod1_mask = event.state & gtk.gdk.MOD1_MASK
        self._flow_graph.handle_mouse_motion(
            coordinate=(event.x, event.y),
        )

    def _handle_window_realize(self, widget):
        """
        Called when the window is realized.
        Update the flowgraph, which calls new pixmap.
        """
        self._flow_graph.update()

    def _handle_window_configure(self, widget, event):
        """
        Called when the window is resized.
        Create a new pixmap for background buffer.
        """
        self._pixmap = self.new_pixmap(*self.get_size_request())

    def _handle_window_expose(self, widget, event):
        """
        Called when window is exposed, or queue_draw is called.
        Double buffering: draw to pixmap, then draw pixmap to window.
        """
        gc = self.window.new_gc()
        self._flow_graph.draw(gc, self._pixmap)
        self.window.draw_drawable(gc, self._pixmap, 0, 0, 0, 0, -1, -1)
        # draw a light grey line on the bottom and right end of the canvas.
        # this is useful when the theme uses the same panel bg color as the canvas
        W, H = self._pixmap.get_size()
        gc.set_foreground(Colors.FLOWGRAPH_EDGE_COLOR)
        self.window.draw_line(gc, 0, H-1, W, H-1)
        self.window.draw_line(gc, W-1, 0, W-1, H)

    def _handle_focus_lost_event(self, widget, event):
        # don't clear selection while context menu is active
        if not self._flow_graph.get_context_menu().flags() & gtk.VISIBLE:
            self._flow_graph.unselect()
            self._flow_graph.update_selected()
            self._flow_graph.queue_draw()
