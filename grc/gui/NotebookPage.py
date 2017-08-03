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

import pygtk
pygtk.require('2.0')
import gtk
import gobject
import Actions
from StateCache import StateCache
from Constants import MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT
from DrawingArea import DrawingArea
import os


class NotebookPage(gtk.HBox):
    """A page in the notebook."""

    def __init__(self, main_window, flow_graph, file_path=''):
        """
        Page constructor.

        Args:
            main_window: main window
            file_path: path to a flow graph file
        """
        self._flow_graph = flow_graph
        self.process = None
        #import the file
        self.main_window = main_window
        self.file_path = file_path
        initial_state = flow_graph.get_parent().parse_flow_graph(file_path)
        self.state_cache = StateCache(initial_state)
        self.saved = True
        #import the data to the flow graph
        self.get_flow_graph().import_data(initial_state)
        #initialize page gui
        gtk.HBox.__init__(self, False, 0)
        self.show()
        #tab box to hold label and close button
        self.tab = gtk.HBox(False, 0)
        #setup tab label
        self.label = gtk.Label()
        self.tab.pack_start(self.label, False)
        #setup button image
        image = gtk.Image()
        image.set_from_stock('gtk-close', gtk.ICON_SIZE_MENU)
        #setup image box
        image_box = gtk.HBox(False, 0)
        image_box.pack_start(image, True, False, 0)
        #setup the button
        button = gtk.Button()
        button.connect("clicked", self._handle_button)
        button.set_relief(gtk.RELIEF_NONE)
        button.add(image_box)
        #button size
        w, h = gtk.icon_size_lookup_for_settings(button.get_settings(), gtk.ICON_SIZE_MENU)
        button.set_size_request(w+6, h+6)
        self.tab.pack_start(button, False)
        self.tab.show_all()
        #setup scroll window and drawing area
        self.scrolled_window = gtk.ScrolledWindow()
        self.scrolled_window.set_size_request(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT)
        self.scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self.scrolled_window.connect('key-press-event', self._handle_scroll_window_key_press)
        self.drawing_area = DrawingArea(self.get_flow_graph())
        self.scrolled_window.add_with_viewport(self.get_drawing_area())
        self.pack_start(self.scrolled_window)
        #inject drawing area into flow graph
        self.get_flow_graph().drawing_area = self.get_drawing_area()
        self.show_all()

    def get_drawing_area(self): return self.drawing_area

    def _handle_scroll_window_key_press(self, widget, event):
        """forward Ctrl-PgUp/Down to NotebookPage (switch fg instead of horiz. scroll"""
        is_ctrl_pg = (
            event.state & gtk.gdk.CONTROL_MASK and
            event.keyval in (gtk.keysyms.Page_Up, gtk.keysyms.Page_Down)
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

    def get_tab(self):
        """
        Get the gtk widget for this page's tab.

        Returns:
            gtk widget
        """
        return self.tab

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

    def term_proc(self):
        """
        Terminate the subprocess object

        Add a callback to kill the process
        after 2 seconds if not already terminated
        """
        def kill(process):
            """
            Kill process if not already terminated

            Called by gobject.timeout_add

            Returns:
                False to stop timeout_add periodic calls
            """
            is_terminated = process.poll()
            if is_terminated is None:
                process.kill()
            return False

        self.get_proc().terminate()
        gobject.timeout_add(2000, kill, self.get_proc())

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
