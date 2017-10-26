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

import gtk

from . import Bars, Actions, Preferences, Utils
from .BlockTreeWindow import BlockTreeWindow
from .VariableEditor import VariableEditor
from .Constants import \
    NEW_FLOGRAPH_TITLE, DEFAULT_CONSOLE_WINDOW_WIDTH
from .Dialogs import TextDisplay, MessageDialogHelper
from .NotebookPage import NotebookPage

from ..core import Messages

MAIN_WINDOW_TITLE_TMPL = """\
#if not $saved
*#slurp
#end if
#if $basename
$basename#slurp
#else
$new_flowgraph_title#slurp
#end if
#if $read_only
 (read only)#slurp
#end if
#if $dirname
 - $dirname#slurp
#end if
 - $platform_name#slurp
"""

PAGE_TITLE_MARKUP_TMPL = """\
#set $foreground = $saved and 'black' or 'red'
<span foreground="$foreground">$encode($title or $new_flowgraph_title)</span>#slurp
#if $read_only
 (ro)#slurp
#end if
"""


############################################################
# Main window
############################################################

class MainWindow(gtk.Window):
    """The topmost window with menus, the tool bar, and other major windows."""

    # Constants the action handler can use to indicate which panel visibility to change.
    BLOCKS = 0
    CONSOLE = 1
    VARIABLES = 2

    def __init__(self, platform, action_handler_callback):
        """
        MainWindow constructor
        Setup the menu, toolbar, flow graph editor notebook, block selection window...
        """
        self._platform = platform

        gen_opts = platform.blocks['options'].get_param('generate_options')
        generate_mode_default = gen_opts.get_value()
        generate_modes = [
            (o.get_key(), o.get_name(), o.get_key() == generate_mode_default)
            for o in gen_opts.get_options()]

        # Load preferences
        Preferences.load(platform)

        # Setup window
        gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
        vbox = gtk.VBox()
        self.add(vbox)

        icon_theme = gtk.icon_theme_get_default()
        icon = icon_theme.lookup_icon("gnuradio-grc", 48, 0)
        if not icon:
            # Set window icon
            self.set_icon_from_file(os.path.dirname(os.path.abspath(__file__)) + "/icon.png")

        # Create the menu bar and toolbar
        self.add_accel_group(Actions.get_accel_group())
        self.menu_bar = Bars.MenuBar(generate_modes, action_handler_callback)
        vbox.pack_start(self.menu_bar, False)
        self.tool_bar = Bars.Toolbar(generate_modes, action_handler_callback)
        vbox.pack_start(self.tool_bar, False)

        # Main parent container for the different panels
        self.container = gtk.HPaned()
        vbox.pack_start(self.container)

        # Create the notebook
        self.notebook = gtk.Notebook()
        self.page_to_be_closed = None
        self.current_page = None
        self.notebook.set_show_border(False)
        self.notebook.set_scrollable(True)  # scroll arrows for page tabs
        self.notebook.connect('switch-page', self._handle_page_change)

        # Create the console window
        self.text_display = TextDisplay()
        self.console_window = gtk.ScrolledWindow()
        self.console_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self.console_window.add(self.text_display)
        self.console_window.set_size_request(-1, DEFAULT_CONSOLE_WINDOW_WIDTH)

        # Create the block tree and variable panels
        self.btwin = BlockTreeWindow(platform, self.get_flow_graph)
        self.vars = VariableEditor(platform, self.get_flow_graph)

        # Figure out which place to put the variable editor
        self.left = gtk.VPaned()
        self.right = gtk.VPaned()
        self.left_subpanel = gtk.HPaned()

        self.variable_panel_sidebar = Preferences.variable_editor_sidebar()
        if self.variable_panel_sidebar:
            self.left.pack1(self.notebook)
            self.left.pack2(self.console_window, False)
            self.right.pack1(self.btwin)
            self.right.pack2(self.vars, False)
        else:
            # Put the variable editor in a panel with the console
            self.left.pack1(self.notebook)
            self.left_subpanel.pack1(self.console_window, shrink=False)
            self.left_subpanel.pack2(self.vars, resize=False, shrink=True)
            self.left.pack2(self.left_subpanel, False)

            # Create the right panel
            self.right.pack1(self.btwin)

        self.container.pack1(self.left)
        self.container.pack2(self.right, False)

        # load preferences and show the main window
        self.resize(*Preferences.main_window_size())
        self.container.set_position(Preferences.blocks_window_position())
        self.left.set_position(Preferences.console_window_position())
        if self.variable_panel_sidebar:
            self.right.set_position(Preferences.variable_editor_position(sidebar=True))
        else:
            self.left_subpanel.set_position(Preferences.variable_editor_position())

        self.show_all()
        self.console_window.hide()
        self.vars.hide()
        self.btwin.hide()

    ############################################################
    # Event Handlers
    ############################################################

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
        self.current_page = self.notebook.get_nth_page(page_num)
        Actions.PAGE_CHANGE()

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
	        self.console_window.show()
            else:
	        self.console_window.hide()
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
            if not (self.vars.get_property('visible')) and not (self.console_window.get_property('visible')):
                self.left_subpanel.hide()
            else:
                self.left_subpanel.show()

    ############################################################
    # Console Window
    ############################################################

    def add_console_line(self, line):
        """
        Place line at the end of the text buffer, then scroll its window all the way down.

        Args:
            line: the new text
        """
        self.text_display.insert(line)

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
        #if the file is already open, show the open page and return
        if file_path and file_path in self._get_files(): #already open
            page = self.notebook.get_nth_page(self._get_files().index(file_path))
            self._set_page(page)
            return
        try: #try to load from file
            if file_path: Messages.send_start_load(file_path)
            flow_graph = self._platform.get_new_flow_graph()
            flow_graph.grc_file_path = file_path
            #print flow_graph
            page = NotebookPage(
                self,
                flow_graph=flow_graph,
                file_path=file_path,
            )
            if file_path: Messages.send_end_load()
        except Exception, e: #return on failure
            Messages.send_fail_load(e)
            if isinstance(e, KeyError) and str(e) == "'options'":
                # This error is unrecoverable, so crash gracefully
                exit(-1)
            return
        #add this page to the notebook
        self.notebook.append_page(page, page.get_tab())
        try: self.notebook.set_tab_reorderable(page, True)
        except: pass #gtk too old
        self.notebook.set_tab_label_packing(page, False, False, gtk.PACK_START)
        #only show if blank or manual
        if not file_path or show: self._set_page(page)

    def close_pages(self):
        """
        Close all the pages in this notebook.

        Returns:
            true if all closed
        """
        open_files = filter(lambda file: file, self._get_files()) #filter blank files
        open_file = self.get_page().get_file_path()
        #close each page
        for page in sorted(self.get_pages(), key=lambda p: p.get_saved()):
            self.page_to_be_closed = page
            closed = self.close_page(False)
            if not closed:
                break
        if self.notebook.get_n_pages(): return False
        #save state before closing
        Preferences.set_open_files(open_files)
        Preferences.file_open(open_file)
        Preferences.main_window_size(self.get_size())
        Preferences.console_window_position(self.left.get_position())
        Preferences.blocks_window_position(self.container.get_position())
        if self.variable_panel_sidebar:
            Preferences.variable_editor_position(self.right.get_position(), sidebar=True)
        else:
            Preferences.variable_editor_position(self.left_subpanel.get_position())
        Preferences.save()
        return True

    def close_page(self, ensure=True):
        """
        Close the current page.
        If the notebook becomes empty, and ensure is true,
        call new page upon exit to ensure that at least one page exists.

        Args:
            ensure: boolean
        """
        if not self.page_to_be_closed: self.page_to_be_closed = self.get_page()
        #show the page if it has an executing flow graph or is unsaved
        if self.page_to_be_closed.get_proc() or not self.page_to_be_closed.get_saved():
            self._set_page(self.page_to_be_closed)
        #unsaved? ask the user
        if not self.page_to_be_closed.get_saved():
            response = self._save_changes() # return value is either OK, CLOSE, or CANCEL
            if response == gtk.RESPONSE_OK:
                Actions.FLOW_GRAPH_SAVE() #try to save
                if not self.page_to_be_closed.get_saved(): #still unsaved?
                    self.page_to_be_closed = None #set the page to be closed back to None
                    return False
            elif response == gtk.RESPONSE_CANCEL:
                self.page_to_be_closed = None
                return False
        #stop the flow graph if executing
        if self.page_to_be_closed.get_proc(): Actions.FLOW_GRAPH_KILL()
        #remove the page
        self.notebook.remove_page(self.notebook.page_num(self.page_to_be_closed))
        if ensure and self.notebook.get_n_pages() == 0: self.new_page() #no pages, make a new one
        self.page_to_be_closed = None #set the page to be closed back to None
        return True

    ############################################################
    # Misc
    ############################################################

    def update(self):
        """
        Set the title of the main window.
        Set the titles on the page tabs.
        Show/hide the console window.

        Args:
            title: the window title
        """
        gtk.Window.set_title(self, Utils.parse_template(MAIN_WINDOW_TITLE_TMPL,
                basename=os.path.basename(self.get_page().get_file_path()),
                dirname=os.path.dirname(self.get_page().get_file_path()),
                new_flowgraph_title=NEW_FLOGRAPH_TITLE,
                read_only=self.get_page().get_read_only(),
                saved=self.get_page().get_saved(),
                platform_name=self._platform.config.name,
            )
        )
        #set tab titles
        for page in self.get_pages(): page.set_markup(
            Utils.parse_template(PAGE_TITLE_MARKUP_TMPL,
                #get filename and strip out file extension
                title=os.path.splitext(os.path.basename(page.get_file_path()))[0],
                read_only=page.get_read_only(), saved=page.get_saved(),
                new_flowgraph_title=NEW_FLOGRAPH_TITLE,
            )
        )
        #show/hide notebook tabs
        self.notebook.set_show_tabs(len(self.get_pages()) > 1)

         # Need to update the variable window when changing
        self.vars.update_gui()

    def update_pages(self):
        """
        Forces a reload of all the pages in this notebook.
        """
        for page in self.get_pages():
            success = page.get_flow_graph().reload()
            if success:  # Only set saved if errors occurred during import
                page.set_saved(False)

    def get_page(self):
        """
        Get the selected page.

        Returns:
            the selected page
        """
        return self.current_page

    def get_flow_graph(self):
        """
        Get the selected flow graph.

        Returns:
            the selected flow graph
        """
        return self.get_page().get_flow_graph()

    def get_focus_flag(self):
        """
        Get the focus flag from the current page.

        Returns:
            the focus flag
        """
        return self.get_page().get_drawing_area().get_focus_flag()

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
        self.notebook.set_current_page(self.notebook.page_num(self.current_page))

    def _save_changes(self):
        """
        Save changes to flow graph?

        Returns:
            the response_id (see buttons variable below)
        """
        buttons = (
            'Close without saving', gtk.RESPONSE_CLOSE,
            gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
            gtk.STOCK_SAVE, gtk.RESPONSE_OK
        )
        return MessageDialogHelper(
            gtk.MESSAGE_QUESTION, gtk.BUTTONS_NONE, self, 'Unsaved Changes!',
            'Would you like to save changes before closing?', gtk.RESPONSE_OK, buttons
        )

    def _get_files(self):
        """
        Get the file names for all the pages, in order.

        Returns:
            list of file paths
        """
        return map(lambda page: page.get_file_path(), self.get_pages())

    def get_pages(self):
        """
        Get a list of all pages in the notebook.

        Returns:
            list of pages
        """
        return [self.notebook.get_nth_page(page_num) for page_num in range(self.notebook.get_n_pages())]
