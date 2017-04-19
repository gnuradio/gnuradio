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

from __future__ import absolute_import
import os
import logging

from gi.repository import Gtk, Gdk, GObject

from . import Actions
from .StateCache import StateCache
from .Constants import MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT
from .DrawingArea import DrawingArea


log = logging.getLogger(__name__)


class Notebook(Gtk.Notebook):
    def __init__(self):
        Gtk.Notebook.__init__(self)
        log.debug("notebook()")
        self.app = Gtk.Application.get_default()

        self.current_page = None
        self.set_show_border(False)
        self.set_scrollable(True)
        self.connect('switch-page', self._handle_page_change)

    def _handle_page_change(self, notebook, page, page_num):
        """
        Handle a page change. When the user clicks on a new tab,
        reload the flow graph to update the vars window and
        call handle states (select nothing) to update the buttons.

        Args:
            notebook: the notebook
            page: new page
            page_num: new page number
        """
        self.current_page = self.get_nth_page(page_num)
        Actions.PAGE_CHANGE()


class Page(Gtk.HBox):
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
        self.flow_graph = flow_graph
        self.file_path = file_path

        self.process = None
        self.saved = True

        # import the file
        initial_state = flow_graph.parent_platform.parse_flow_graph(file_path)
        flow_graph.import_data(initial_state)
        self.state_cache = StateCache(initial_state)

        # tab box to hold label and close button
        self.label = Gtk.Label()
        image = Gtk.Image.new_from_icon_name('window-close', Gtk.IconSize.MENU)
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
        self.drawing_area = DrawingArea(flow_graph)
        flow_graph.drawing_area = self.drawing_area

        self.scrolled_window = Gtk.ScrolledWindow()
        self.scrolled_window.set_size_request(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT)
        self.scrolled_window.set_policy(Gtk.PolicyType.ALWAYS, Gtk.PolicyType.ALWAYS)
        self.scrolled_window.connect('key-press-event', self._handle_scroll_window_key_press)
        self.scrolled_window.add(self.drawing_area)
        self.pack_start(self.scrolled_window, True, True, 0)

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
        platform = self.flow_graph.parent_platform
        return platform.Generator(self.flow_graph, self.file_path)

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

    def get_read_only(self):
        """
        Get the read-only state of the file.
        Always false for empty path.

        Returns:
            true for read-only
        """
        if not self.file_path:
            return False
        return (os.path.exists(self.file_path) and
                not os.access(self.file_path, os.W_OK))
