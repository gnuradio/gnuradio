"""
Copyright 2007, 2008, 2009, 2015, 2016 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import logging

from gi.repository import Gtk, GObject, Gio, GLib

from . import Actions


log = logging.getLogger(__name__)


'''
# Menu/Toolbar Lists:
#
# Sub items can be 1 of 3 types
#  - List    Creates a section within the current menu
#  - Tuple   Creates a submenu using a string or action as the parent. The child
#            can be another menu list or an identifier used to call a helper function.
#  - Action  Appends a new menu item to the current menu
#

LIST_NAME = [
    [Action1, Action2], # New section
    (Action3, [Action4, Action5]), # Submenu with action as parent
    ("Label", [Action6, Action7]), # Submenu with string as parent
    ("Label2", "helper") # Submenu with helper function. Calls 'create_helper()'
]
'''


# The list of actions for the toolbar.
TOOLBAR_LIST = [
    [(Actions.FLOW_GRAPH_NEW, 'flow_graph_new_type'),
     (Actions.FLOW_GRAPH_OPEN, 'flow_graph_recent'),
     Actions.FLOW_GRAPH_SAVE, Actions.FLOW_GRAPH_CLOSE],
    [Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR, Actions.FLOW_GRAPH_SCREEN_CAPTURE],
    [Actions.BLOCK_CUT, Actions.BLOCK_COPY,
        Actions.BLOCK_PASTE, Actions.ELEMENT_DELETE],
    [Actions.FLOW_GRAPH_UNDO, Actions.FLOW_GRAPH_REDO],
    [Actions.ERRORS_WINDOW_DISPLAY, Actions.FLOW_GRAPH_GEN,
        Actions.FLOW_GRAPH_EXEC, Actions.FLOW_GRAPH_KILL],
    [Actions.BLOCK_ROTATE_CCW, Actions.BLOCK_ROTATE_CW],
    [Actions.BLOCK_ENABLE, Actions.BLOCK_DISABLE,
        Actions.BLOCK_BYPASS, Actions.TOGGLE_HIDE_DISABLED_BLOCKS],
    [Actions.FIND_BLOCKS, Actions.RELOAD_BLOCKS, Actions.OPEN_HIER]
]


# The list of actions and categories for the menu bar.
MENU_BAR_LIST = [
    ('_File', [
        [(Actions.FLOW_GRAPH_NEW, 'flow_graph_new_type'), Actions.FLOW_GRAPH_DUPLICATE,
         Actions.FLOW_GRAPH_OPEN, (Actions.FLOW_GRAPH_OPEN_RECENT, 'flow_graph_recent')],
        [Actions.FLOW_GRAPH_SAVE, Actions.FLOW_GRAPH_SAVE_AS,
            Actions.FLOW_GRAPH_SAVE_COPY],
        [Actions.FLOW_GRAPH_SCREEN_CAPTURE],
        [Actions.FLOW_GRAPH_CLOSE, Actions.APPLICATION_QUIT]
    ]),
    ('_Edit', [
        [Actions.FLOW_GRAPH_UNDO, Actions.FLOW_GRAPH_REDO],
        [Actions.BLOCK_CUT, Actions.BLOCK_COPY, Actions.BLOCK_PASTE,
            Actions.ELEMENT_DELETE, Actions.SELECT_ALL],
        [Actions.BLOCK_ROTATE_CCW, Actions.BLOCK_ROTATE_CW,
            ('_Align', Actions.BLOCK_ALIGNMENTS)],
        [Actions.BLOCK_ENABLE, Actions.BLOCK_DISABLE, Actions.BLOCK_BYPASS],
        [Actions.BLOCK_PARAM_MODIFY]
    ]),
    ('_View', [
        [Actions.TOGGLE_BLOCKS_WINDOW],
        [Actions.TOGGLE_CONSOLE_WINDOW, Actions.TOGGLE_SCROLL_LOCK,
            Actions.SAVE_CONSOLE, Actions.CLEAR_CONSOLE],
        [Actions.TOGGLE_HIDE_VARIABLES, Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR, Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR_SIDEBAR,
            Actions.TOGGLE_SHOW_PARAMETER_EXPRESSION, Actions.TOGGLE_SHOW_PARAMETER_EVALUATION],
        [Actions.TOGGLE_HIDE_DISABLED_BLOCKS, Actions.TOGGLE_AUTO_HIDE_PORT_LABELS,
            Actions.TOGGLE_SNAP_TO_GRID, Actions.TOGGLE_SHOW_BLOCK_COMMENTS, Actions.TOGGLE_SHOW_BLOCK_IDS, ],
        [Actions.TOGGLE_SHOW_CODE_PREVIEW_TAB],
        [Actions.ZOOM_IN],
        [Actions.ZOOM_OUT],
        [Actions.ZOOM_RESET],
        [Actions.ERRORS_WINDOW_DISPLAY, Actions.FIND_BLOCKS],
    ]),
    ('_Run', [
        Actions.FLOW_GRAPH_GEN, Actions.FLOW_GRAPH_EXEC, Actions.FLOW_GRAPH_KILL
    ]),
    ('_Tools', [
        [Actions.TOOLS_RUN_FDESIGN, Actions.FLOW_GRAPH_OPEN_QSS_THEME],
        [Actions.TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY]
    ]),
    ('_Help', [
        [Actions.HELP_WINDOW_DISPLAY, Actions.TYPES_WINDOW_DISPLAY,
            Actions.KEYBOARD_SHORTCUTS_WINDOW_DISPLAY, Actions.XML_PARSER_ERRORS_DISPLAY],
        [Actions.GET_INVOLVED_WINDOW_DISPLAY, Actions.ABOUT_WINDOW_DISPLAY]
    ])]


# The list of actions for the context menu.
CONTEXT_MENU_LIST = [
    [Actions.BLOCK_CUT, Actions.BLOCK_COPY,
        Actions.BLOCK_PASTE, Actions.ELEMENT_DELETE],
    [Actions.BLOCK_ROTATE_CCW, Actions.BLOCK_ROTATE_CW,
        Actions.BLOCK_ENABLE, Actions.BLOCK_DISABLE, Actions.BLOCK_BYPASS],
    [("_More", [
        [Actions.BLOCK_CREATE_HIER, Actions.OPEN_HIER],
        [Actions.BUSSIFY_SOURCES, Actions.BUSSIFY_SINKS]
    ])],
    [Actions.BLOCK_PARAM_MODIFY],
]


class SubMenuHelper(object):
    ''' Generates custom submenus for the main menu or toolbar. '''

    def __init__(self):
        self.submenus = {}

    def build_submenu(self, name, parent_obj, obj_idx, obj, set_func):
        # Get the correct helper function
        create_func = getattr(self, "create_{}".format(name))
        # Save the helper functions for rebuilding the menu later
        self.submenus[name] = (create_func, parent_obj, obj_idx, obj, set_func)
        # Actually build the menu
        set_func(obj, create_func())

    def create_flow_graph_new_type(self):
        """ Different flowgraph types """
        menu = Gio.Menu()
        platform = Gtk.Application.get_default().platform
        generate_modes = platform.get_generate_options()
        for key, name, default in generate_modes:
            target = "app.flowgraph.new_type::{}".format(key)
            menu.append(name, target)
        return menu

    def create_flow_graph_recent(self):
        """ Recent flow graphs """

        config = Gtk.Application.get_default().config
        recent_files = config.get_recent_files()
        menu = Gio.Menu()
        if len(recent_files) > 0:
            files = Gio.Menu()
            for i, file_name in enumerate(recent_files):
                target = "app.flowgraph.open_recent::{}".format(file_name)
                files.append(file_name.replace("_", "__"), target)
            menu.append_section(None, files)
            #clear = Gio.Menu()
            #clear.append("Clear recent files", "app.flowgraph.clear_recent")
            #menu.append_section(None, clear)
        else:
            # Show an empty menu
            menuitem = Gio.MenuItem.new("No items found", "app.none")
            menu.append_item(menuitem)
        return menu


class MenuHelper(SubMenuHelper):
    """
    Recursively builds a menu from a given list of actions.

    Args:
     - actions:  List of actions to build the menu
     - menu:     Current menu being built

    Notes:
     - Tuple:  Create a new submenu from the parent (1st) and child (2nd) elements
     - Action: Append to current menu
     - List:   Start a new section
    """

    def __init__(self):
        SubMenuHelper.__init__(self)

    def build_menu(self, actions, menu):
        for idx, item in enumerate(actions):
            log.debug("build_menu idx, action: %s, %s", idx, item)
            if isinstance(item, tuple):
                # Create a new submenu
                parent, child = (item[0], item[1])

                # Create the parent
                label, target = (parent, None)
                if isinstance(parent, Actions.Action):
                    label = parent.label
                    target = "{}.{}".format(parent.prefix, parent.name)
                menuitem = Gio.MenuItem.new(label, None)
                if hasattr(parent, "icon_name"):
                    menuitem.set_icon(
                        Gio.Icon.new_for_string(parent.icon_name))

                # Create the new submenu
                if isinstance(child, list):
                    submenu = Gio.Menu()
                    self.build_menu(child, submenu)
                    menuitem.set_submenu(submenu)
                elif isinstance(child, str):
                    # Child is the name of the submenu to create
                    def set_func(obj, menu):
                        obj.set_submenu(menu)
                    self.build_submenu(child, menu, idx, menuitem, set_func)
                menu.append_item(menuitem)

            elif isinstance(item, list):
                # Create a new section
                section = Gio.Menu()
                self.build_menu(item, section)
                menu.append_section(None, section)

            elif isinstance(item, Actions.Action):
                # Append a new menuitem
                target = "{}.{}".format(item.prefix, item.name)
                menuitem = Gio.MenuItem.new(item.label, target)
                if item.icon_name:
                    menuitem.set_icon(Gio.Icon.new_for_string(item.icon_name))
                menu.append_item(menuitem)

    def refresh_submenus(self):
        for name in self.submenus:
            create_func, parent_obj, obj_idx, obj, set_func = self.submenus[name]
            set_func(obj, create_func())
            parent_obj.remove(obj_idx)
            parent_obj.insert_item(obj_idx, obj)


class ToolbarHelper(SubMenuHelper):
    """
     Builds a toolbar from a given list of actions.

    Args:
     - actions:  List of actions to build the menu
     - item:     Current menu being built

    Notes:
     - Tuple:  Create a new submenu from the parent (1st) and child (2nd) elements
     - Action: Append to current menu
     - List:   Start a new section
    """

    def __init__(self):
        SubMenuHelper.__init__(self)

    def build_toolbar(self, actions, current):
        for idx, item in enumerate(actions):
            if isinstance(item, list):
                # Toolbar's don't have sections like menus, so call this function
                #  recursively with the "section" and just append a separator.
                self.build_toolbar(item, self)
                current.insert(Gtk.SeparatorToolItem.new(), -1)

            elif isinstance(item, tuple):
                parent, child = (item[0], item[1])
                # Create an item with a submenu
                # Generate the submenu and add to the item.
                # Add the item to the toolbar
                button = Gtk.MenuToolButton.new()
                # The tuple should be made up of an Action and something.
                button.set_label(parent.label)
                button.set_tooltip_text(parent.tooltip)
                button.set_icon_name(parent.icon_name)

                target = "{}.{}".format(parent.prefix, parent.name)
                button.set_action_name(target)

                def set_func(obj, menu):
                    obj.set_menu(Gtk.Menu.new_from_model(menu))

                self.build_submenu(child, current, idx, button, set_func)
                current.insert(button, -1)

            elif isinstance(item, Actions.Action):
                button = Gtk.ToolButton.new()
                button.set_label(item.label)
                button.set_tooltip_text(item.tooltip)
                button.set_icon_name(item.icon_name)
                target = "{}.{}".format(item.prefix, item.name)
                button.set_action_name(target)
                current.insert(button, -1)

    def refresh_submenus(self):
        for name in self.submenus:
            create_func, parent_obj, _, obj, set_func = self.submenus[name]
            set_func(obj, create_func())


class Menu(Gio.Menu, MenuHelper):
    """ Main Menu """

    def __init__(self):
        GObject.GObject.__init__(self)
        MenuHelper.__init__(self)

        log.debug("Building the main menu")
        self.build_menu(MENU_BAR_LIST, self)


class ContextMenu(Gio.Menu, MenuHelper):
    """ Context menu for the drawing area """

    def __init__(self):
        GObject.GObject.__init__(self)

        log.debug("Building the context menu")
        self.build_menu(CONTEXT_MENU_LIST, self)


class Toolbar(Gtk.Toolbar, ToolbarHelper):
    """ The gtk toolbar with actions added from the toolbar list. """

    def __init__(self):
        """
        Parse the list of action names in the toolbar list.
        Look up the action for each name in the action list and add it to the
        toolbar.
        """
        GObject.GObject.__init__(self)
        ToolbarHelper.__init__(self)

        self.set_style(Gtk.ToolbarStyle.ICONS)
        # self.get_style_context().add_class(Gtk.STYLE_CLASS_PRIMARY_TOOLBAR)

        # SubMenuCreator.__init__(self)
        self.build_toolbar(TOOLBAR_LIST, self)
