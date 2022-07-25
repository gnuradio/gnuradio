"""
Copyright 2008, 2009, 2011 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import os
import logging

from gi.repository import Gtk, Gdk, GObject

from . import Bars, Actions, Utils
from .BlockTreeWindow import BlockTreeWindow
from .Console import Console
from .VariableEditor import VariableEditor
from .Constants import \
    NEW_FLOGRAPH_TITLE, DEFAULT_CONSOLE_WINDOW_WIDTH
from .Dialogs import TextDisplay, MessageDialogWrapper
from .Notebook import Notebook, Page

from ..core import Messages


log = logging.getLogger(__name__)


############################################################
# Main window
############################################################
class MainWindow(Gtk.ApplicationWindow):
    """The topmost window with menus, the tool bar, and other major windows."""

    # Constants the action handler can use to indicate which panel visibility to change.
    BLOCKS = 0
    CONSOLE = 1
    VARIABLES = 2

    def __init__(self, app, platform):
        """
        MainWindow constructor
        Setup the menu, toolbar, flow graph editor notebook, block selection window...
        """
        Gtk.ApplicationWindow.__init__(
            self, title="GNU Radio Companion", application=app)
        log.debug("__init__()")

        self._platform = platform
        self.app = app
        self.config = platform.config

        # Add all "win" actions to the local
        for x in Actions.get_actions():
            if x.startswith("win."):
                self.add_action(Actions.actions[x])

        # Setup window
        vbox = Gtk.VBox()
        self.add(vbox)

        icon_theme = Gtk.IconTheme.get_default()
        icon = icon_theme.lookup_icon("gnuradio-grc", 48, 0)
        if not icon:
            # Set default window icon
            self.set_icon_from_file(os.path.dirname(
                os.path.abspath(__file__)) + "/icon.png")
        else:
            # Use gnuradio icon
            self.set_icon(icon.load_icon())

        # Create the menu bar and toolbar
        log.debug("Creating menu")
        generate_modes = platform.get_generate_options()

        # This needs to be replaced
        # Have an option for either the application menu or this menu
        self.menu = Bars.Menu()
        self.menu_bar = Gtk.MenuBar.new_from_model(self.menu)
        vbox.pack_start(self.menu_bar, False, False, 0)

        self.tool_bar = Bars.Toolbar()
        self.tool_bar.set_hexpand(True)
        # Show the toolbar
        self.tool_bar.show()
        vbox.pack_start(self.tool_bar, False, False, 0)

        # Main parent container for the different panels
        self.main = Gtk.HPaned()  # (orientation=Gtk.Orientation.HORIZONTAL)
        vbox.pack_start(self.main, True, True, 0)

        # Create the notebook
        self.notebook = Notebook()
        self.page_to_be_closed = None

        self.current_page = None  # type: Page

        # Create the console window
        self.console = Console()

        # Create the block tree and variable panels
        self.btwin = BlockTreeWindow(platform)
        self.btwin.connect('create_new_block',
                           self._add_block_to_current_flow_graph)
        self.vars = VariableEditor()
        self.vars.connect('create_new_block',
                          self._add_block_to_current_flow_graph)
        self.vars.connect(
            'remove_block', self._remove_block_from_current_flow_graph)

        # Figure out which place to put the variable editor
        self.left = Gtk.VPaned()  # orientation=Gtk.Orientation.VERTICAL)
        self.right = Gtk.VPaned()  # orientation=Gtk.Orientation.VERTICAL)
        # orientation=Gtk.Orientation.HORIZONTAL)
        self.left_subpanel = Gtk.HPaned()

        self.variable_panel_sidebar = self.config.variable_editor_sidebar()
        if self.variable_panel_sidebar:
            self.left.pack1(self.notebook)
            self.left.pack2(self.console, False)
            self.right.pack1(self.btwin)
            self.right.pack2(self.vars, False)
        else:
            # Put the variable editor in a panel with the console
            self.left.pack1(self.notebook)
            self.left_subpanel.pack1(self.console, shrink=False)
            self.left_subpanel.pack2(self.vars, resize=False, shrink=True)
            self.left.pack2(self.left_subpanel, False)

            # Create the right panel
            self.right.pack1(self.btwin)

        self.main.pack1(self.left)
        self.main.pack2(self.right, False)

        # Load preferences and show the main window
        self.resize(*self.config.main_window_size())
        self.main.set_position(self.config.blocks_window_position())
        self.left.set_position(self.config.console_window_position())
        if self.variable_panel_sidebar:
            self.right.set_position(
                self.config.variable_editor_position(sidebar=True))
        else:
            self.left_subpanel.set_position(
                self.config.variable_editor_position())

        self.show_all()
        log.debug("Main window ready")

    ############################################################
    # Event Handlers
    ############################################################

    def _add_block_to_current_flow_graph(self, widget, key):
        self.current_flow_graph.add_new_block(key)

    def _remove_block_from_current_flow_graph(self, widget, key):
        block = self.current_flow_graph.get_block(key)
        self.current_flow_graph.remove_element(block)

    def _quit(self, window, event):
        """
        Handle the delete event from the main window.
        Generated by pressing X to close, alt+f4, or right click+close.
        This method in turns calls the state handler to quit.

        Returns:
            true
        """
        Actions.APPLICATION_QUIT()
        return True

    def update_panel_visibility(self, panel, visibility=True):
        """
        Handles changing visibility of panels.
        """
        # Set the visibility for the requested panel, then update the containers if they need
        #  to be hidden as well.

        if panel == self.BLOCKS:
            if visibility:
                self.btwin.show()
            else:
                self.btwin.hide()
        elif panel == self.CONSOLE:
            if visibility:
                self.console.show()
            else:
                self.console.hide()
        elif panel == self.VARIABLES:
            if visibility:
                self.vars.show()
            else:
                self.vars.hide()
        else:
            return

        if self.variable_panel_sidebar:
            # If both the variable editor and block panels are hidden, hide the right container
            if not (self.btwin.get_property('visible')) and not (self.vars.get_property('visible')):
                self.right.hide()
            else:
                self.right.show()
        else:
            if not (self.btwin.get_property('visible')):
                self.right.hide()
            else:
                self.right.show()
            if not (self.vars.get_property('visible')) and not (self.console.get_property('visible')):
                self.left_subpanel.hide()
            else:
                self.left_subpanel.show()

    ############################################################
    # Console Window
    ############################################################

    @property
    def current_page(self):
        return self.notebook.current_page

    @current_page.setter
    def current_page(self, page):
        self.notebook.current_page = page

    def add_console_line(self, line):
        """
        Place line at the end of the text buffer, then scroll its window all the way down.

        Args:
            line: the new text
        """
        self.console.add_line(line)

    ############################################################
    # Pages: create and close
    ############################################################

    def new_page(self, file_path='', show=False):
        """
        Create a new notebook page.
        Set the tab to be selected.

        Args:
            file_path: optional file to load into the flow graph
            show: true if the page should be shown after loading
        """
        # if the file is already open, show the open page and return
        if file_path and file_path in self._get_files():  # already open
            page = self.notebook.get_nth_page(
                self._get_files().index(file_path))
            self._set_page(page)
            return
        try:  # try to load from file
            if file_path:
                Messages.send_start_load(file_path)
            flow_graph = self._platform.make_flow_graph()
            flow_graph.grc_file_path = file_path
            # print flow_graph
            page = Page(
                self,
                flow_graph=flow_graph,
                file_path=file_path,
            )
            if getattr(Messages, 'flowgraph_error') is not None:
                Messages.send(
                    ">>> Check: {}\n>>> FlowGraph Error: {}\n".format(
                        str(Messages.flowgraph_error_file),
                        str(Messages.flowgraph_error)
                    )
                )
            if file_path:
                Messages.send_end_load()
        except Exception as e:  # return on failure
            Messages.send_fail_load(e)
            if isinstance(e, KeyError) and str(e) == "'options'":
                # This error is unrecoverable, so crash gracefully
                exit(-1)
            return
        # add this page to the notebook
        self.notebook.append_page(page, page.tab)
        self.notebook.set_tab_reorderable(page, True)
        # only show if blank or manual
        if not file_path or show:
            self._set_page(page)

    def close_pages(self):
        """
        Close all the pages in this notebook.

        Returns:
            true if all closed
        """
        open_files = [file for file in self._get_files()
                      if file]  # filter blank files
        open_file = self.current_page.file_path
        # close each page
        for page in sorted(self.get_pages(), key=lambda p: p.saved):
            self.page_to_be_closed = page
            closed = self.close_page(False)
            if not closed:
                break
        if self.notebook.get_n_pages():
            return False
        # save state before closing
        self.config.set_open_files(open_files)
        self.config.file_open(open_file)
        self.config.main_window_size(self.get_size())
        self.config.console_window_position(self.left.get_position())
        self.config.blocks_window_position(self.main.get_position())
        if self.variable_panel_sidebar:
            self.config.variable_editor_position(
                self.right.get_position(), sidebar=True)
        else:
            self.config.variable_editor_position(
                self.left_subpanel.get_position())
        self.config.save()
        return True

    def close_page(self, ensure=True):
        """
        Close the current page.
        If the notebook becomes empty, and ensure is true,
        call new page upon exit to ensure that at least one page exists.

        Args:
            ensure: boolean
        """
        if not self.page_to_be_closed:
            self.page_to_be_closed = self.current_page
        # show the page if it has an executing flow graph or is unsaved
        if self.page_to_be_closed.process or not self.page_to_be_closed.saved:
            self._set_page(self.page_to_be_closed)
        # unsaved? ask the user
        if not self.page_to_be_closed.saved:
            response = self._save_changes()  # return value is either OK, CLOSE, or CANCEL
            if response == Gtk.ResponseType.OK:
                Actions.FLOW_GRAPH_SAVE()  # try to save
                if not self.page_to_be_closed.saved:  # still unsaved?
                    self.page_to_be_closed = None  # set the page to be closed back to None
                    return False
            elif response == Gtk.ResponseType.CANCEL:
                self.page_to_be_closed = None
                return False
        # stop the flow graph if executing
        if self.page_to_be_closed.process:
            Actions.FLOW_GRAPH_KILL()
        # remove the page
        self.notebook.remove_page(
            self.notebook.page_num(self.page_to_be_closed))
        if ensure and self.notebook.get_n_pages() == 0:
            self.new_page()  # no pages, make a new one
        self.page_to_be_closed = None  # set the page to be closed back to None
        return True

    ############################################################
    # Misc
    ############################################################

    def update(self):
        """
        Set the title of the main window.
        Set the titles on the page tabs.
        Show/hide the console window.
        """
        page = self.current_page

        basename = os.path.basename(page.file_path)
        dirname = os.path.dirname(page.file_path)
        Gtk.Window.set_title(self, ''.join((
            '*' if not page.saved else '', basename if basename else NEW_FLOGRAPH_TITLE,
            '(read only)' if page.get_read_only() else '', ' - ',
            dirname if dirname else self._platform.config.name,
        )))
        # set tab titles
        for page in self.get_pages():
            file_name = os.path.splitext(os.path.basename(page.file_path))[0]
            page.set_markup('<span foreground="{foreground}">{title}{ro}</span>'.format(
                foreground='black' if page.saved else 'red', ro=' (ro)' if page.get_read_only() else '',
                title=Utils.encode(file_name or NEW_FLOGRAPH_TITLE),
            ))
            fpath = page.file_path
            if not fpath:
                fpath = '(unsaved)'
            page.set_tooltip(fpath)
        # show/hide notebook tabs
        self.notebook.set_show_tabs(len(self.get_pages()) > 1)

        # Need to update the variable window when changing
        self.vars.update_gui(self.current_flow_graph.blocks)

    def update_pages(self):
        """
        Forces a reload of all the pages in this notebook.
        """
        for page in self.get_pages():
            success = page.flow_graph.reload()
            if success:  # Only set saved if errors occurred during import
                page.saved = False

    @property
    def current_flow_graph(self):
        return self.current_page.flow_graph

    def get_focus_flag(self):
        """
        Get the focus flag from the current page.
        Returns:
            the focus flag
        """
        return self.current_page.drawing_area.get_focus_flag()

    ############################################################
    # Helpers
    ############################################################

    def _set_page(self, page):
        """
        Set the current page.

        Args:
            page: the page widget
        """
        self.current_page = page
        self.notebook.set_current_page(
            self.notebook.page_num(self.current_page))

    def _save_changes(self):
        """
        Save changes to flow graph?

        Returns:
            the response_id (see buttons variable below)
        """
        buttons = (
            'Close without saving', Gtk.ResponseType.CLOSE,
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
            Gtk.STOCK_SAVE, Gtk.ResponseType.OK
        )
        return MessageDialogWrapper(
            self, Gtk.MessageType.QUESTION, Gtk.ButtonsType.NONE, 'Unsaved Changes!',
            'Would you like to save changes before closing?', Gtk.ResponseType.OK, buttons
        ).run_and_destroy()

    def _get_files(self):
        """
        Get the file names for all the pages, in order.

        Returns:
            list of file paths
        """
        return [page.file_path for page in self.get_pages()]

    def get_pages(self):
        """
        Get a list of all pages in the notebook.

        Returns:
            list of pages
        """
        return [self.notebook.get_nth_page(page_num)
                for page_num in range(self.notebook.get_n_pages())]
