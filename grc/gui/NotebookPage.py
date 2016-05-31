"""
Copyright 2008, 2009, 2011 Free Software Foundation, Inc.
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

import os

from gi.repository import Gtk, Gdk, GObject

from . import Actions
from .StateCache import StateCache
from .Constants import MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT
from .DrawingArea import DrawingArea


class NotebookPage(Gtk.HBox):
    """A page in the notebook."""

    def __init__(self, main_window, flow_graph, file_path=''):
        """
        Page constructor.

        Args:
            main_window: main window
            file_path: path to a flow graph file
        """
        Gtk.HBox.__init__(self)

        self.main_window = main_window
        self._flow_graph = flow_graph
        self.process = None

        # import the file
        self.file_path = file_path
        initial_state = flow_graph.get_parent().parse_flow_graph(file_path)
        self.get_flow_graph().import_data(initial_state)
        self.state_cache = StateCache(initial_state)
        self.saved = True

        # tab box to hold label and close button
        self.label = Gtk.Label()
        image = Gtk.Image()
        image.set_from_stock('gtk-close', Gtk.IconSize.MENU)
        image_box = Gtk.HBox(homogeneous=False, spacing=0)
        image_box.pack_start(image, True, False, 0)
        button = Gtk.Button()
        button.connect("clicked", self._handle_button)
        button.set_relief(Gtk.ReliefStyle.NONE)
        button.add(image_box)

        tab = self.tab = Gtk.HBox(homogeneous=False, spacing=0)
        tab.pack_start(self.label, False, False, 0)
        tab.pack_start(button, False, False, 0)
        tab.show_all()

        # setup scroll window and drawing area
        self.drawing_area = DrawingArea(self.get_flow_graph())

        self.scrolled_window = Gtk.ScrolledWindow()
        self.scrolled_window.set_size_request(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT)
        self.scrolled_window.set_policy(Gtk.PolicyType.ALWAYS, Gtk.PolicyType.ALWAYS)
        self.scrolled_window.connect('key-press-event', self._handle_scroll_window_key_press)
        self.scrolled_window.add_with_viewport(self.drawing_area)
        self.pack_start(self.scrolled_window, True, True, 0)

        # inject drawing area into flow graph
        self.get_flow_graph().drawing_area = self.drawing_area
        self.show_all()

    def _handle_scroll_window_key_press(self, widget, event):
        """forward Ctrl-PgUp/Down to NotebookPage (switch fg instead of horiz. scroll"""
        is_ctrl_pg = (
            event.state & Gdk.ModifierType.CONTROL_MASK and
            event.keyval in (Gdk.KEY_Page_Up, Gdk.KEY_Page_Down)
        )
        if is_ctrl_pg:
            return self.get_parent().event(event)

    def get_generator(self):
        """
        Get the generator object for this flow graph.

        Returns:
            generator
        """
        platform = self.get_flow_graph().get_parent()
        return platform.Generator(self.get_flow_graph(), self.get_file_path())

    def _handle_button(self, button):
        """
        The button was clicked.
        Make the current page selected, then close.

        Args:
            the: button
        """
        self.main_window.page_to_be_closed = self
        Actions.FLOW_GRAPH_CLOSE()

    def set_markup(self, markup):
        """
        Set the markup in this label.

        Args:
            markup: the new markup text
        """
        self.label.set_markup(markup)

    def get_proc(self):
        """
        Get the subprocess for the flow graph.

        Returns:
            the subprocess object
        """
        return self.process

    def set_proc(self, process):
        """
        Set the subprocess object.

        Args:
            process: the new subprocess
        """
        self.process = process

    def get_flow_graph(self):
        """
        Get the flow graph.

        Returns:
            the flow graph
        """
        return self._flow_graph

    def get_read_only(self):
        """
        Get the read-only state of the file.
        Always false for empty path.

        Returns:
            true for read-only
        """
        if not self.get_file_path(): return False
        return os.path.exists(self.get_file_path()) and \
        not os.access(self.get_file_path(), os.W_OK)

    def get_file_path(self):
        """
        Get the file path for the flow graph.

        Returns:
            the file path or ''
        """
        return self.file_path

    def set_file_path(self, file_path=''):
        """
        Set the file path, '' for no file path.

        Args:
            file_path: file path string
        """
        self.file_path = os.path.abspath(file_path) if file_path else ''

    def get_saved(self):
        """
        Get the saved status for the flow graph.

        Returns:
            true if saved
        """
        return self.saved

    def set_saved(self, saved=True):
        """
        Set the saved status.

        Args:
            saved: boolean status
        """
        self.saved = saved

    def get_state_cache(self):
        """
        Get the state cache for the flow graph.

        Returns:
            the state cache
        """
        return self.state_cache
