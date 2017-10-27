"""
Copyright 2007, 2008, 2009, 2015 Free Software Foundation, Inc.
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

from . import Actions


# The list of actions for the toolbar.
TOOLBAR_LIST = (
    (Actions.FLOW_GRAPH_NEW, 'flow_graph_new'),
    (Actions.FLOW_GRAPH_OPEN, 'flow_graph_recent'),
    Actions.FLOW_GRAPH_SAVE,
    Actions.FLOW_GRAPH_CLOSE,
    None,
    Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR,
    Actions.FLOW_GRAPH_SCREEN_CAPTURE,
    None,
    Actions.BLOCK_CUT,
    Actions.BLOCK_COPY,
    Actions.BLOCK_PASTE,
    Actions.ELEMENT_DELETE,
    None,
    Actions.FLOW_GRAPH_UNDO,
    Actions.FLOW_GRAPH_REDO,
    None,
    Actions.ERRORS_WINDOW_DISPLAY,
    Actions.FLOW_GRAPH_GEN,
    Actions.FLOW_GRAPH_EXEC,
    Actions.FLOW_GRAPH_KILL,
    None,
    Actions.BLOCK_ROTATE_CCW,
    Actions.BLOCK_ROTATE_CW,
    None,
    Actions.BLOCK_ENABLE,
    Actions.BLOCK_DISABLE,
    Actions.BLOCK_BYPASS,
    Actions.TOGGLE_HIDE_DISABLED_BLOCKS,
    None,
    Actions.FIND_BLOCKS,
    Actions.RELOAD_BLOCKS,
    Actions.OPEN_HIER,
)

# The list of actions and categories for the menu bar.
MENU_BAR_LIST = (
    (gtk.Action('File', '_File', None, None), [
        'flow_graph_new',
        Actions.FLOW_GRAPH_DUPLICATE,
        Actions.FLOW_GRAPH_OPEN,
        'flow_graph_recent',
        None,
        Actions.FLOW_GRAPH_SAVE,
        Actions.FLOW_GRAPH_SAVE_AS,
        Actions.FLOW_GRAPH_SAVE_A_COPY,
        None,
        Actions.FLOW_GRAPH_SCREEN_CAPTURE,
        None,
        Actions.FLOW_GRAPH_CLOSE,
        Actions.APPLICATION_QUIT,
    ]),
    (gtk.Action('Edit', '_Edit', None, None), [
        Actions.FLOW_GRAPH_UNDO,
        Actions.FLOW_GRAPH_REDO,
        None,
        Actions.BLOCK_CUT,
        Actions.BLOCK_COPY,
        Actions.BLOCK_PASTE,
        Actions.ELEMENT_DELETE,
        Actions.SELECT_ALL,
        None,
        Actions.BLOCK_ROTATE_CCW,
        Actions.BLOCK_ROTATE_CW,
        (gtk.Action('Align', '_Align', None, None), Actions.BLOCK_ALIGNMENTS),
        None,
        Actions.BLOCK_ENABLE,
        Actions.BLOCK_DISABLE,
        Actions.BLOCK_BYPASS,
        None,
        Actions.BLOCK_PARAM_MODIFY,
    ]),
    (gtk.Action('View', '_View', None, None), [
        Actions.TOGGLE_BLOCKS_WINDOW,
        None,
        Actions.TOGGLE_CONSOLE_WINDOW,
        Actions.TOGGLE_SCROLL_LOCK,
        Actions.SAVE_CONSOLE,
        Actions.CLEAR_CONSOLE,
        None,
        Actions.TOGGLE_HIDE_VARIABLES,
        Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR,
        Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR_SIDEBAR,
        None,
        Actions.TOGGLE_HIDE_DISABLED_BLOCKS,
        Actions.TOGGLE_AUTO_HIDE_PORT_LABELS,
        Actions.TOGGLE_SNAP_TO_GRID,
        Actions.TOGGLE_SHOW_BLOCK_COMMENTS,
        None,
        Actions.TOGGLE_SHOW_CODE_PREVIEW_TAB,
        None,
        Actions.ERRORS_WINDOW_DISPLAY,
        Actions.FIND_BLOCKS,
    ]),
    (gtk.Action('Run', '_Run', None, None), [
        Actions.FLOW_GRAPH_GEN,
        Actions.FLOW_GRAPH_EXEC,
        Actions.FLOW_GRAPH_KILL,
    ]),
    (gtk.Action('Tools', '_Tools', None, None), [
        Actions.TOOLS_RUN_FDESIGN,
        Actions.FLOW_GRAPH_OPEN_QSS_THEME,
        None,
        Actions.TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY,
        None,
        Actions.TOOLS_MORE_TO_COME,
    ]),
    (gtk.Action('Help', '_Help', None, None), [
        Actions.HELP_WINDOW_DISPLAY,
        Actions.TYPES_WINDOW_DISPLAY,
        Actions.XML_PARSER_ERRORS_DISPLAY,
        None,
        Actions.ABOUT_WINDOW_DISPLAY,
    ]),
)

# The list of actions for the context menu.
CONTEXT_MENU_LIST = [
    Actions.BLOCK_CUT,
    Actions.BLOCK_COPY,
    Actions.BLOCK_PASTE,
    Actions.ELEMENT_DELETE,
    None,
    Actions.BLOCK_ROTATE_CCW,
    Actions.BLOCK_ROTATE_CW,
    Actions.BLOCK_ENABLE,
    Actions.BLOCK_DISABLE,
    Actions.BLOCK_BYPASS,
    None,
    (gtk.Action('More', '_More', None, None), [
        Actions.BLOCK_CREATE_HIER,
        Actions.OPEN_HIER,
        None,
        Actions.BUSSIFY_SOURCES,
        Actions.BUSSIFY_SINKS,
    ]),
    Actions.BLOCK_PARAM_MODIFY
]


class SubMenuCreator(object):

    def __init__(self, generate_modes, action_handler_callback):
        self.generate_modes = generate_modes
        self.action_handler_callback = action_handler_callback
        self.submenus = []

    def create_submenu(self, action_tuple, item):
        func = getattr(self, '_fill_' + action_tuple[1] + "_submenu")
        self.submenus.append((action_tuple[0], func, item))
        self.refresh_submenus()

    def refresh_submenus(self):
        for action, func, item in self.submenus:
            try:
                item.set_property("menu", func(action))
            except TypeError:
                item.set_property("submenu", func(action))
            item.set_property('sensitive', True)

    def callback_adaptor(self, item, action_key):
        action, key = action_key
        self.action_handler_callback(action, key)

    def _fill_flow_graph_new_submenu(self, action):
        """Sub menu to create flow-graph with pre-set generate mode"""
        menu = gtk.Menu()
        for key, name, default in self.generate_modes:
            if default:
                item = Actions.FLOW_GRAPH_NEW.create_menu_item()
                item.set_label(name)
            else:
                item = gtk.MenuItem(name, use_underline=False)
                item.connect('activate', self.callback_adaptor, (action, key))
            menu.append(item)
        menu.show_all()
        return menu

    def _fill_flow_graph_recent_submenu(self, action):
        """menu showing recent flow-graphs"""
        import Preferences
        menu = gtk.Menu()
        recent_files = Preferences.get_recent_files()
        if len(recent_files) > 0:
            for i, file_name in enumerate(recent_files):
                item = gtk.MenuItem("%d. %s" % (i+1, file_name), use_underline=False)
                item.connect('activate', self.callback_adaptor,
                             (action, file_name))
                menu.append(item)
            menu.show_all()
            return menu
        return None


class Toolbar(gtk.Toolbar, SubMenuCreator):
    """The gtk toolbar with actions added from the toolbar list."""

    def __init__(self, generate_modes, action_handler_callback):
        """
        Parse the list of action names in the toolbar list.
        Look up the action for each name in the action list and add it to the
        toolbar.
        """
        gtk.Toolbar.__init__(self)
        self.set_style(gtk.TOOLBAR_ICONS)
        SubMenuCreator.__init__(self, generate_modes, action_handler_callback)

        for action in TOOLBAR_LIST:
            if isinstance(action, tuple) and isinstance(action[1], str):
                # create a button with a sub-menu
                action[0].set_tool_item_type(gtk.MenuToolButton)
                item = action[0].create_tool_item()
                self.create_submenu(action, item)
                self.refresh_submenus()

            elif action is None:
                item = gtk.SeparatorToolItem()

            else:
                action.set_tool_item_type(gtk.ToolButton)
                item = action.create_tool_item()
                # this reset of the tooltip property is required
                # (after creating the tool item) for the tooltip to show
                action.set_property('tooltip', action.get_property('tooltip'))
            self.add(item)


class MenuHelperMixin(object):
    """Mixin class to help build menus from the above action lists"""

    def _fill_menu(self, actions, menu=None):
        """Create a menu from list of actions"""
        menu = menu or gtk.Menu()
        for item in actions:
            if isinstance(item, tuple):
                menu_item = self._make_sub_menu(*item)
            elif isinstance(item, str):
                menu_item = getattr(self, 'create_' + item)()
            elif item is None:
                menu_item = gtk.SeparatorMenuItem()
            else:
                menu_item = item.create_menu_item()
            menu.append(menu_item)
        menu.show_all()
        return menu

    def _make_sub_menu(self, main, actions):
        """Create a submenu from a main action and a list of actions"""
        main = main.create_menu_item()
        main.set_submenu(self._fill_menu(actions))
        return main


class MenuBar(gtk.MenuBar, MenuHelperMixin, SubMenuCreator):
    """The gtk menu bar with actions added from the menu bar list."""

    def __init__(self, generate_modes, action_handler_callback):
        """
        Parse the list of submenus from the menubar list.
        For each submenu, get a list of action names.
        Look up the action for each name in the action list and add it to the
        submenu. Add the submenu to the menu bar.
        """
        gtk.MenuBar.__init__(self)
        SubMenuCreator.__init__(self, generate_modes, action_handler_callback)
        for main_action, actions in MENU_BAR_LIST:
            self.append(self._make_sub_menu(main_action, actions))

    def create_flow_graph_new(self):
        main = gtk.ImageMenuItem(gtk.STOCK_NEW)
        main.set_label(Actions.FLOW_GRAPH_NEW.get_label())
        func = self._fill_flow_graph_new_submenu
        self.submenus.append((Actions.FLOW_GRAPH_NEW, func, main))
        self.refresh_submenus()
        return main

    def create_flow_graph_recent(self):
        main = gtk.ImageMenuItem(gtk.STOCK_OPEN)
        main.set_label(Actions.FLOW_GRAPH_OPEN_RECENT.get_label())
        func = self._fill_flow_graph_recent_submenu
        self.submenus.append((Actions.FLOW_GRAPH_OPEN, func, main))
        self.refresh_submenus()
        if main.get_submenu() is None:
            main.set_property('sensitive', False)
        return main


class ContextMenu(gtk.Menu, MenuHelperMixin):
    """The gtk menu with actions added from the context menu list."""

    def __init__(self):
        gtk.Menu.__init__(self)
        self._fill_menu(CONTEXT_MENU_LIST, self)
