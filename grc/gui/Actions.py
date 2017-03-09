"""
Copyright 2007-2011 Free Software Foundation, Inc.
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

import six

from gi.repository import Gtk, Gdk, GObject

NO_MODS_MASK = 0


########################################################################
# Actions API
########################################################################
_actions_keypress_dict = dict()
_keymap = Gdk.Keymap.get_default()
_used_mods_mask = NO_MODS_MASK


def handle_key_press(event):
    """
    Call the action associated with the key press event.
    Both the key value and the mask must have a match.

    Args:
        event: a gtk key press event

    Returns:
        true if handled
    """
    # extract the key value and the consumed modifiers
    _unused, keyval, egroup, level, consumed = _keymap.translate_keyboard_state(
        event.hardware_keycode, event.get_state(), event.group)
    # get the modifier mask and ignore irrelevant modifiers
    mod_mask = event.get_state() & ~consumed & _used_mods_mask
    # look up the keypress and call the action
    try:
        _actions_keypress_dict[(keyval, mod_mask)]()
    except KeyError:
        return False  # not handled
    else:
        return True  # handled here

_all_actions_list = list()


def get_all_actions():
    return _all_actions_list

_accel_group = Gtk.AccelGroup()


def get_accel_group():
    return _accel_group


class _ActionBase(object):
    """
    Base class for Action and ToggleAction
    Register actions and keypresses with this module.
    """
    def __init__(self, label, keypresses):
        global _used_mods_mask

        _all_actions_list.append(self)
        for i in range(len(keypresses)/2):
            keyval, mod_mask = keypresses[i*2:(i+1)*2]
            # register this keypress
            if (keyval, mod_mask) in _actions_keypress_dict:
                raise KeyError('keyval/mod_mask pair already registered "%s"' % str((keyval, mod_mask)))
            _actions_keypress_dict[(keyval, mod_mask)] = self
            _used_mods_mask |= mod_mask
            # set the accelerator group, and accelerator path
            # register the key name and mod mask with the accelerator path
            if label is None:
                continue  # dont register accel
            accel_path = '<main>/' + self.get_name()
            self.set_accel_group(get_accel_group())
            self.set_accel_path(accel_path)
            Gtk.AccelMap.add_entry(accel_path, keyval, mod_mask)
        self.args = None

    def __str__(self):
        """
        The string representation should be the name of the action id.
        Try to find the action id for this action by searching this module.
        """
        for name, value in six.iteritems(globals()):
            if value == self:
                return name
        return self.get_name()

    def __repr__(self): return str(self)

    def __call__(self, *args):
        """
        Emit the activate signal when called with ().
        """
        self.args = args
        self.emit('activate')


class Action(Gtk.Action, _ActionBase):
    """
    A custom Action class based on Gtk.Action.
    Pass additional arguments such as keypresses.
    """

    def __init__(self, keypresses=(), name=None, label=None, tooltip=None,
                 stock_id=None):
        """
        Create a new Action instance.

        Args:
            key_presses: a tuple of (keyval1, mod_mask1, keyval2, mod_mask2, ...)
            the: regular Gtk.Action parameters (defaults to None)
        """
        if name is None:
            name = label
        GObject.GObject.__init__(self, name=name, label=label, tooltip=tooltip,
                            stock_id=stock_id)
        _ActionBase.__init__(self, label, keypresses)


class ToggleAction(Gtk.ToggleAction, _ActionBase):
    """
    A custom Action class based on Gtk.ToggleAction.
    Pass additional arguments such as keypresses.
    """

    def __init__(self, keypresses=(), name=None, label=None, tooltip=None,
                 stock_id=None, preference_name=None, default=True):
        """
        Create a new ToggleAction instance.

        Args:
            key_presses: a tuple of (keyval1, mod_mask1, keyval2, mod_mask2, ...)
            the: regular Gtk.Action parameters (defaults to None)
        """
        if name is None:
            name = label
        GObject.GObject.__init__(self, name=name, label=label,
                                  tooltip=tooltip, stock_id=stock_id)
        _ActionBase.__init__(self, label, keypresses)
        self.preference_name = preference_name
        self.default = default

    def load_from_preferences(self):
        if self.preference_name is not None:
            config = Gtk.Application.get_default().config
            self.set_active(config.entry(
                self.preference_name, default=bool(self.default)))

    def save_to_preferences(self):
        if self.preference_name is not None:
            config = Gtk.Application.get_default().config
            config.entry(self.preference_name, value=self.get_active())

########################################################################
# Actions
########################################################################
PAGE_CHANGE = Action()
EXTERNAL_UPDATE = Action()
VARIABLE_EDITOR_UPDATE = Action()
FLOW_GRAPH_NEW = Action(
    label='_New',
    tooltip='Create a new flow graph',
    stock_id=Gtk.STOCK_NEW,
    keypresses=(Gdk.KEY_n, Gdk.ModifierType.CONTROL_MASK),
)
FLOW_GRAPH_OPEN = Action(
    label='_Open',
    tooltip='Open an existing flow graph',
    stock_id=Gtk.STOCK_OPEN,
    keypresses=(Gdk.KEY_o, Gdk.ModifierType.CONTROL_MASK),
)
FLOW_GRAPH_OPEN_RECENT = Action(
    label='Open _Recent',
    tooltip='Open a recently used flow graph',
    stock_id=Gtk.STOCK_OPEN,
)
FLOW_GRAPH_SAVE = Action(
    label='_Save',
    tooltip='Save the current flow graph',
    stock_id=Gtk.STOCK_SAVE,
    keypresses=(Gdk.KEY_s, Gdk.ModifierType.CONTROL_MASK),
)
FLOW_GRAPH_SAVE_AS = Action(
    label='Save _As',
    tooltip='Save the current flow graph as...',
    stock_id=Gtk.STOCK_SAVE_AS,
    keypresses=(Gdk.KEY_s, Gdk.ModifierType.CONTROL_MASK | Gdk.ModifierType.SHIFT_MASK),
)
FLOW_GRAPH_CLOSE = Action(
    label='_Close',
    tooltip='Close the current flow graph',
    stock_id=Gtk.STOCK_CLOSE,
    keypresses=(Gdk.KEY_w, Gdk.ModifierType.CONTROL_MASK),
)
APPLICATION_INITIALIZE = Action()
APPLICATION_QUIT = Action(
    label='_Quit',
    tooltip='Quit program',
    stock_id=Gtk.STOCK_QUIT,
    keypresses=(Gdk.KEY_q, Gdk.ModifierType.CONTROL_MASK),
)
FLOW_GRAPH_UNDO = Action(
    label='_Undo',
    tooltip='Undo a change to the flow graph',
    stock_id=Gtk.STOCK_UNDO,
    keypresses=(Gdk.KEY_z, Gdk.ModifierType.CONTROL_MASK),
)
FLOW_GRAPH_REDO = Action(
    label='_Redo',
    tooltip='Redo a change to the flow graph',
    stock_id=Gtk.STOCK_REDO,
    keypresses=(Gdk.KEY_y, Gdk.ModifierType.CONTROL_MASK),
)
NOTHING_SELECT = Action()
SELECT_ALL = Action(
    label='Select _All',
    tooltip='Select all blocks and connections in the flow graph',
    stock_id=Gtk.STOCK_SELECT_ALL,
    keypresses=(Gdk.KEY_a, Gdk.ModifierType.CONTROL_MASK),
)
ELEMENT_SELECT = Action()
ELEMENT_CREATE = Action()
ELEMENT_DELETE = Action(
    label='_Delete',
    tooltip='Delete the selected blocks',
    stock_id=Gtk.STOCK_DELETE,
    keypresses=(Gdk.KEY_Delete, NO_MODS_MASK),
)
BLOCK_MOVE = Action()
BLOCK_ROTATE_CCW = Action(
    label='Rotate Counterclockwise',
    tooltip='Rotate the selected blocks 90 degrees to the left',
    stock_id=Gtk.STOCK_GO_BACK,
    keypresses=(Gdk.KEY_Left, NO_MODS_MASK),
)
BLOCK_ROTATE_CW = Action(
    label='Rotate Clockwise',
    tooltip='Rotate the selected blocks 90 degrees to the right',
    stock_id=Gtk.STOCK_GO_FORWARD,
    keypresses=(Gdk.KEY_Right, NO_MODS_MASK),
)
BLOCK_VALIGN_TOP = Action(
    label='Vertical Align Top',
    tooltip='Align tops of selected blocks',
    keypresses=(Gdk.KEY_t, Gdk.ModifierType.SHIFT_MASK),
)
BLOCK_VALIGN_MIDDLE = Action(
    label='Vertical Align Middle',
    tooltip='Align centers of selected blocks vertically',
    keypresses=(Gdk.KEY_m, Gdk.ModifierType.SHIFT_MASK),
)
BLOCK_VALIGN_BOTTOM = Action(
    label='Vertical Align Bottom',
    tooltip='Align bottoms of selected blocks',
    keypresses=(Gdk.KEY_b, Gdk.ModifierType.SHIFT_MASK),
)
BLOCK_HALIGN_LEFT = Action(
    label='Horizontal Align Left',
    tooltip='Align left edges of blocks selected blocks',
    keypresses=(Gdk.KEY_l, Gdk.ModifierType.SHIFT_MASK),
)
BLOCK_HALIGN_CENTER = Action(
    label='Horizontal Align Center',
    tooltip='Align centers of selected blocks horizontally',
    keypresses=(Gdk.KEY_c, Gdk.ModifierType.SHIFT_MASK),
)
BLOCK_HALIGN_RIGHT = Action(
    label='Horizontal Align Right',
    tooltip='Align right edges of selected blocks',
    keypresses=(Gdk.KEY_r, Gdk.ModifierType.SHIFT_MASK),
)
BLOCK_ALIGNMENTS = [
    BLOCK_VALIGN_TOP,
    BLOCK_VALIGN_MIDDLE,
    BLOCK_VALIGN_BOTTOM,
    None,
    BLOCK_HALIGN_LEFT,
    BLOCK_HALIGN_CENTER,
    BLOCK_HALIGN_RIGHT,
]
BLOCK_PARAM_MODIFY = Action(
    label='_Properties',
    tooltip='Modify params for the selected block',
    stock_id=Gtk.STOCK_PROPERTIES,
    keypresses=(Gdk.KEY_Return, NO_MODS_MASK),
)
BLOCK_ENABLE = Action(
    label='E_nable',
    tooltip='Enable the selected blocks',
    stock_id=Gtk.STOCK_CONNECT,
    keypresses=(Gdk.KEY_e, NO_MODS_MASK),
)
BLOCK_DISABLE = Action(
    label='D_isable',
    tooltip='Disable the selected blocks',
    stock_id=Gtk.STOCK_DISCONNECT,
    keypresses=(Gdk.KEY_d, NO_MODS_MASK),
)
BLOCK_BYPASS = Action(
    label='_Bypass',
    tooltip='Bypass the selected block',
    stock_id=Gtk.STOCK_MEDIA_FORWARD,
    keypresses=(Gdk.KEY_b, NO_MODS_MASK),
)
TOGGLE_SNAP_TO_GRID = ToggleAction(
    label='_Snap to grid',
    tooltip='Snap blocks to a grid for an easier connection alignment',
    preference_name='snap_to_grid'
)
TOGGLE_HIDE_DISABLED_BLOCKS = ToggleAction(
    label='Hide _Disabled Blocks',
    tooltip='Toggle visibility of disabled blocks and connections',
    stock_id=Gtk.STOCK_MISSING_IMAGE,
    keypresses=(Gdk.KEY_d, Gdk.ModifierType.CONTROL_MASK),
)
TOGGLE_HIDE_VARIABLES = ToggleAction(
    label='Hide Variables',
    tooltip='Hide all variable blocks',
    preference_name='hide_variables',
    default=False,
)
TOGGLE_FLOW_GRAPH_VAR_EDITOR = ToggleAction(
    label='Show _Variable Editor',
    tooltip='Show the variable editor. Modify variables and imports in this flow graph',
    stock_id=Gtk.STOCK_EDIT,
    default=True,
    keypresses=(Gdk.KEY_e, Gdk.ModifierType.CONTROL_MASK),
    preference_name='variable_editor_visable',
)
TOGGLE_FLOW_GRAPH_VAR_EDITOR_SIDEBAR = ToggleAction(
    label='Move the Variable Editor to the Sidebar',
    tooltip='Move the variable editor to the sidebar',
    default=False,
    preference_name='variable_editor_sidebar',
)
TOGGLE_AUTO_HIDE_PORT_LABELS = ToggleAction(
    label='Auto-Hide _Port Labels',
    tooltip='Automatically hide port labels',
    preference_name='auto_hide_port_labels'
)
TOGGLE_SHOW_BLOCK_COMMENTS = ToggleAction(
    label='Show Block Comments',
    tooltip="Show comment beneath each block",
    preference_name='show_block_comments'
)
TOGGLE_SHOW_CODE_PREVIEW_TAB = ToggleAction(
    label='Generated Code Preview',
    tooltip="Show a preview of the code generated for each Block in its "
            "Properties Dialog",
    preference_name='show_generated_code_tab',
    default=False,
)
TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY = ToggleAction(
    label='Show Flowgraph Complexity',
    tooltip="How many Balints is the flowgraph...",
    preference_name='show_flowgraph_complexity',
    default=False,
)
BLOCK_CREATE_HIER = Action(
    label='C_reate Hier',
    tooltip='Create hier block from selected blocks',
    stock_id=Gtk.STOCK_CONNECT,
#   keypresses=(Gdk.KEY_c, NO_MODS_MASK),
)
BLOCK_CUT = Action(
    label='Cu_t',
    tooltip='Cut',
    stock_id=Gtk.STOCK_CUT,
    keypresses=(Gdk.KEY_x, Gdk.ModifierType.CONTROL_MASK),
)
BLOCK_COPY = Action(
    label='_Copy',
    tooltip='Copy',
    stock_id=Gtk.STOCK_COPY,
    keypresses=(Gdk.KEY_c, Gdk.ModifierType.CONTROL_MASK),
)
BLOCK_PASTE = Action(
    label='_Paste',
    tooltip='Paste',
    stock_id=Gtk.STOCK_PASTE,
    keypresses=(Gdk.KEY_v, Gdk.ModifierType.CONTROL_MASK),
)
ERRORS_WINDOW_DISPLAY = Action(
    label='Flowgraph _Errors',
    tooltip='View flow graph errors',
    stock_id=Gtk.STOCK_DIALOG_ERROR,
)
TOGGLE_CONSOLE_WINDOW = ToggleAction(
    label='Show _Console Panel',
    tooltip='Toggle visibility of the console',
    keypresses=(Gdk.KEY_r, Gdk.ModifierType.CONTROL_MASK),
    preference_name='console_window_visible'
)
TOGGLE_BLOCKS_WINDOW = ToggleAction(
    label='Show _Block Tree Panel',
    tooltip='Toggle visibility of the block tree widget',
    keypresses=(Gdk.KEY_b, Gdk.ModifierType.CONTROL_MASK),
    preference_name='blocks_window_visible'
)
TOGGLE_SCROLL_LOCK = ToggleAction(
    label='Console Scroll _Lock',
    tooltip='Toggle scroll lock for the console window',
    preference_name='scroll_lock'
)
ABOUT_WINDOW_DISPLAY = Action(
    label='_About',
    tooltip='About this program',
    stock_id=Gtk.STOCK_ABOUT,
)
HELP_WINDOW_DISPLAY = Action(
    label='_Help',
    tooltip='Usage tips',
    stock_id=Gtk.STOCK_HELP,
    keypresses=(Gdk.KEY_F1, NO_MODS_MASK),
)
TYPES_WINDOW_DISPLAY = Action(
    label='_Types',
    tooltip='Types color mapping',
    stock_id=Gtk.STOCK_DIALOG_INFO,
)
FLOW_GRAPH_GEN = Action(
    label='_Generate',
    tooltip='Generate the flow graph',
    stock_id=Gtk.STOCK_CONVERT,
    keypresses=(Gdk.KEY_F5, NO_MODS_MASK),
)
FLOW_GRAPH_EXEC = Action(
    label='_Execute',
    tooltip='Execute the flow graph',
    stock_id=Gtk.STOCK_MEDIA_PLAY,
    keypresses=(Gdk.KEY_F6, NO_MODS_MASK),
)
FLOW_GRAPH_KILL = Action(
    label='_Kill',
    tooltip='Kill the flow graph',
    stock_id=Gtk.STOCK_STOP,
    keypresses=(Gdk.KEY_F7, NO_MODS_MASK),
)
FLOW_GRAPH_SCREEN_CAPTURE = Action(
    label='Screen Ca_pture',
    tooltip='Create a screen capture of the flow graph',
    stock_id=Gtk.STOCK_PRINT,
    keypresses=(Gdk.KEY_p, Gdk.ModifierType.CONTROL_MASK),
)
PORT_CONTROLLER_DEC = Action(
    keypresses=(Gdk.KEY_minus, NO_MODS_MASK, Gdk.KEY_KP_Subtract, NO_MODS_MASK),
)
PORT_CONTROLLER_INC = Action(
    keypresses=(Gdk.KEY_plus, NO_MODS_MASK, Gdk.KEY_KP_Add, NO_MODS_MASK),
)
BLOCK_INC_TYPE = Action(
    keypresses=(Gdk.KEY_Down, NO_MODS_MASK),
)
BLOCK_DEC_TYPE = Action(
    keypresses=(Gdk.KEY_Up, NO_MODS_MASK),
)
RELOAD_BLOCKS = Action(
    label='Reload _Blocks',
    tooltip='Reload Blocks',
    stock_id=Gtk.STOCK_REFRESH
)
FIND_BLOCKS = Action(
    label='_Find Blocks',
    tooltip='Search for a block by name (and key)',
    stock_id=Gtk.STOCK_FIND,
    keypresses=(Gdk.KEY_f, Gdk.ModifierType.CONTROL_MASK,
                Gdk.KEY_slash, NO_MODS_MASK),
)
CLEAR_CONSOLE = Action(
    label='_Clear Console',
    tooltip='Clear Console',
    stock_id=Gtk.STOCK_CLEAR,
)
SAVE_CONSOLE = Action(
    label='_Save Console',
    tooltip='Save Console',
    stock_id=Gtk.STOCK_SAVE,
)
OPEN_HIER = Action(
    label='Open H_ier',
    tooltip='Open the source of the selected hierarchical block',
    stock_id=Gtk.STOCK_JUMP_TO,
)
BUSSIFY_SOURCES = Action(
    label='Toggle So_urce Bus',
    tooltip='Gang source ports into a single bus port',
    stock_id=Gtk.STOCK_JUMP_TO,
)
BUSSIFY_SINKS = Action(
    label='Toggle S_ink Bus',
    tooltip='Gang sink ports into a single bus port',
    stock_id=Gtk.STOCK_JUMP_TO,
)
XML_PARSER_ERRORS_DISPLAY = Action(
    label='_Parser Errors',
    tooltip='View errors that occurred while parsing XML files',
    stock_id=Gtk.STOCK_DIALOG_ERROR,
)
FLOW_GRAPH_OPEN_QSS_THEME = Action(
    label='Set Default QT GUI _Theme',
    tooltip='Set a default QT Style Sheet file to use for QT GUI',
    stock_id=Gtk.STOCK_OPEN,
)
TOOLS_RUN_FDESIGN = Action(
    label='Filter Design Tool',
    tooltip='Execute gr_filter_design',
    stock_id=Gtk.STOCK_EXECUTE,
)
TOOLS_MORE_TO_COME = Action(
    label='More to come',
)
