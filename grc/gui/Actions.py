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

import pygtk
pygtk.require('2.0')
import gtk

import Preferences

NO_MODS_MASK = 0

########################################################################
# Actions API
########################################################################
_actions_keypress_dict = dict()
_keymap = gtk.gdk.keymap_get_default()
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
    _used_mods_mask = reduce(lambda x, y: x | y, [mod_mask for keyval, mod_mask in _actions_keypress_dict], NO_MODS_MASK)
    #extract the key value and the consumed modifiers
    keyval, egroup, level, consumed = _keymap.translate_keyboard_state(
        event.hardware_keycode, event.state, event.group)
    #get the modifier mask and ignore irrelevant modifiers
    mod_mask = event.state & ~consumed & _used_mods_mask
    #look up the keypress and call the action
    try: _actions_keypress_dict[(keyval, mod_mask)]()
    except KeyError: return False #not handled
    return True #handled here

_all_actions_list = list()
def get_all_actions(): return _all_actions_list

_accel_group = gtk.AccelGroup()
def get_accel_group(): return _accel_group


class _ActionBase(object):
    """
    Base class for Action and ToggleAction
    Register actions and keypresses with this module.
    """
    def __init__(self, label, keypresses):
        _all_actions_list.append(self)
        for i in range(len(keypresses)/2):
            keyval, mod_mask = keypresses[i*2:(i+1)*2]
            #register this keypress
            if _actions_keypress_dict.has_key((keyval, mod_mask)):
                raise KeyError('keyval/mod_mask pair already registered "%s"'%str((keyval, mod_mask)))
            _actions_keypress_dict[(keyval, mod_mask)] = self
            #set the accelerator group, and accelerator path
            #register the key name and mod mask with the accelerator path
            if label is None: continue #dont register accel
            accel_path = '<main>/'+self.get_name()
            self.set_accel_group(get_accel_group())
            self.set_accel_path(accel_path)
            gtk.accel_map_add_entry(accel_path, keyval, mod_mask)

    def __str__(self):
        """
        The string representation should be the name of the action id.
        Try to find the action id for this action by searching this module.
        """
        try:
            import Actions
            return filter(lambda attr: getattr(Actions, attr) == self, dir(Actions))[0]
        except: return self.get_name()

    def __repr__(self): return str(self)

    def __call__(self):
        """
        Emit the activate signal when called with ().
        """
        self.emit('activate')


class Action(gtk.Action, _ActionBase):
    """
    A custom Action class based on gtk.Action.
    Pass additional arguments such as keypresses.
    """

    def __init__(self, keypresses=(), name=None, label=None, tooltip=None, stock_id=None):
        """
        Create a new Action instance.

        Args:
            key_presses: a tuple of (keyval1, mod_mask1, keyval2, mod_mask2, ...)
            the: regular gtk.Action parameters (defaults to None)
        """
        if name is None: name = label
        gtk.Action.__init__(self,
            name=name, label=label,
            tooltip=tooltip, stock_id=stock_id,
        )
        #register this action
        _ActionBase.__init__(self, label, keypresses)


class ToggleAction(gtk.ToggleAction, _ActionBase):
    """
    A custom Action class based on gtk.ToggleAction.
    Pass additional arguments such as keypresses.
    """

    def __init__(self, keypresses=(), name=None, label=None, tooltip=None, stock_id=None, preference_name=None):
        """
        Create a new ToggleAction instance.

        Args:
            key_presses: a tuple of (keyval1, mod_mask1, keyval2, mod_mask2, ...)
            the: regular gtk.Action parameters (defaults to None)
        """
        if name is None: name = label
        gtk.ToggleAction.__init__(self,
            name=name, label=label,
            tooltip=tooltip, stock_id=stock_id,
        )
        #register this action
        _ActionBase.__init__(self, label, keypresses)
        self.preference_name = preference_name

    def load_from_preferences(self):
        if self.preference_name is not None:
            self.set_active(Preferences.bool_entry(self.preference_name))

    def save_to_preferences(self):
        if self.preference_name is not None:
            Preferences.bool_entry(self.preference_name, self.get_active())

########################################################################
# Actions
########################################################################
PAGE_CHANGE = Action()
FLOW_GRAPH_NEW = Action(
    label='_New',
    tooltip='Create a new flow graph',
    stock_id=gtk.STOCK_NEW,
    keypresses=(gtk.keysyms.n, gtk.gdk.CONTROL_MASK),
)
FLOW_GRAPH_OPEN = Action(
    label='_Open',
    tooltip='Open an existing flow graph',
    stock_id=gtk.STOCK_OPEN,
    keypresses=(gtk.keysyms.o, gtk.gdk.CONTROL_MASK),
)
FLOW_GRAPH_SAVE = Action(
    label='_Save',
    tooltip='Save the current flow graph',
    stock_id=gtk.STOCK_SAVE,
    keypresses=(gtk.keysyms.s, gtk.gdk.CONTROL_MASK),
)
FLOW_GRAPH_SAVE_AS = Action(
    label='Save _As',
    tooltip='Save the current flow graph as...',
    stock_id=gtk.STOCK_SAVE_AS,
    keypresses=(gtk.keysyms.s, gtk.gdk.CONTROL_MASK | gtk.gdk.SHIFT_MASK),
)
FLOW_GRAPH_CLOSE = Action(
    label='_Close',
    tooltip='Close the current flow graph',
    stock_id=gtk.STOCK_CLOSE,
    keypresses=(gtk.keysyms.w, gtk.gdk.CONTROL_MASK),
)
APPLICATION_INITIALIZE = Action()
APPLICATION_QUIT = Action(
    label='_Quit',
    tooltip='Quit program',
    stock_id=gtk.STOCK_QUIT,
    keypresses=(gtk.keysyms.q, gtk.gdk.CONTROL_MASK),
)
FLOW_GRAPH_UNDO = Action(
    label='_Undo',
    tooltip='Undo a change to the flow graph',
    stock_id=gtk.STOCK_UNDO,
    keypresses=(gtk.keysyms.z, gtk.gdk.CONTROL_MASK),
)
FLOW_GRAPH_REDO = Action(
    label='_Redo',
    tooltip='Redo a change to the flow graph',
    stock_id=gtk.STOCK_REDO,
    keypresses=(gtk.keysyms.y, gtk.gdk.CONTROL_MASK),
)
NOTHING_SELECT = Action()
ELEMENT_SELECT = Action()
ELEMENT_CREATE = Action()
ELEMENT_DELETE = Action(
    label='_Delete',
    tooltip='Delete the selected blocks',
    stock_id=gtk.STOCK_DELETE,
    keypresses=(gtk.keysyms.Delete, NO_MODS_MASK),
)
BLOCK_MOVE = Action()
BLOCK_ROTATE_CCW = Action(
    label='Rotate Counterclockwise',
    tooltip='Rotate the selected blocks 90 degrees to the left',
    stock_id=gtk.STOCK_GO_BACK,
    keypresses=(gtk.keysyms.Left, NO_MODS_MASK),
)
BLOCK_ROTATE_CW = Action(
    label='Rotate Clockwise',
    tooltip='Rotate the selected blocks 90 degrees to the right',
    stock_id=gtk.STOCK_GO_FORWARD,
    keypresses=(gtk.keysyms.Right, NO_MODS_MASK),
)
BLOCK_PARAM_MODIFY = Action(
    label='_Properties',
    tooltip='Modify params for the selected block',
    stock_id=gtk.STOCK_PROPERTIES,
    keypresses=(gtk.keysyms.Return, NO_MODS_MASK),
)
BLOCK_ENABLE = Action(
    label='E_nable',
    tooltip='Enable the selected blocks',
    stock_id=gtk.STOCK_CONNECT,
    keypresses=(gtk.keysyms.e, NO_MODS_MASK),
)
BLOCK_DISABLE = Action(
    label='D_isable',
    tooltip='Disable the selected blocks',
    stock_id=gtk.STOCK_DISCONNECT,
    keypresses=(gtk.keysyms.d, NO_MODS_MASK),
)
TOGGLE_SNAP_TO_GRID = ToggleAction(
    label='_Snap to grid',
    tooltip='Snap blocks to a grid for an easier connection alignment',
    preference_name='snap_to_grid'
)
TOGGLE_HIDE_DISABLED_BLOCKS = ToggleAction(
    label='Hide _Disabled Blocks',
    tooltip='Toggle visibility of disabled blocks and connections',
    stock_id=gtk.STOCK_MISSING_IMAGE,
    keypresses=(gtk.keysyms.d, gtk.gdk.CONTROL_MASK),
)
TOGGLE_AUTO_HIDE_PORT_LABELS = ToggleAction(
    label='Auto-Hide _Port Labels',
    tooltip='Automatically hide port labels',
    preference_name='auto_hide_port_labels'
)
BLOCK_CREATE_HIER = Action(
    label='C_reate Hier',
    tooltip='Create hier block from selected blocks',
    stock_id=gtk.STOCK_CONNECT,
#   keypresses=(gtk.keysyms.c, NO_MODS_MASK),
)
BLOCK_CUT = Action(
    label='Cu_t',
    tooltip='Cut',
    stock_id=gtk.STOCK_CUT,
    keypresses=(gtk.keysyms.x, gtk.gdk.CONTROL_MASK),
)
BLOCK_COPY = Action(
    label='_Copy',
    tooltip='Copy',
    stock_id=gtk.STOCK_COPY,
    keypresses=(gtk.keysyms.c, gtk.gdk.CONTROL_MASK),
)
BLOCK_PASTE = Action(
    label='_Paste',
    tooltip='Paste',
    stock_id=gtk.STOCK_PASTE,
    keypresses=(gtk.keysyms.v, gtk.gdk.CONTROL_MASK),
)
ERRORS_WINDOW_DISPLAY = Action(
    label='Flowgraph _Errors',
    tooltip='View flow graph errors',
    stock_id=gtk.STOCK_DIALOG_ERROR,
)
TOGGLE_REPORTS_WINDOW = ToggleAction(
    label='Show _Reports',
    tooltip='Toggle visibility of the Report widget',
    keypresses=(gtk.keysyms.r, gtk.gdk.CONTROL_MASK),
    preference_name='reports_window_visible'
)
TOGGLE_BLOCKS_WINDOW = ToggleAction(
    label='Show _Block Tree',
    tooltip='Toggle visibility of the block tree widget',
    keypresses=(gtk.keysyms.b, gtk.gdk.CONTROL_MASK),
    preference_name='blocks_window_visible'
)
TOGGLE_SCROLL_LOCK = ToggleAction(
    label='Reports Scroll _Lock',
    tooltip='Toggle scroll lock for the report window',
    preference_name='scroll_lock'
)
ABOUT_WINDOW_DISPLAY = Action(
    label='_About',
    tooltip='About this program',
    stock_id=gtk.STOCK_ABOUT,
)
HELP_WINDOW_DISPLAY = Action(
    label='_Help',
    tooltip='Usage tips',
    stock_id=gtk.STOCK_HELP,
    keypresses=(gtk.keysyms.F1, NO_MODS_MASK),
)
TYPES_WINDOW_DISPLAY = Action(
    label='_Types',
    tooltip='Types color mapping',
    stock_id=gtk.STOCK_DIALOG_INFO,
)
FLOW_GRAPH_GEN = Action(
    label='_Generate',
    tooltip='Generate the flow graph',
    stock_id=gtk.STOCK_CONVERT,
    keypresses=(gtk.keysyms.F5, NO_MODS_MASK),
)
FLOW_GRAPH_EXEC = Action(
    label='_Execute',
    tooltip='Execute the flow graph',
    stock_id=gtk.STOCK_MEDIA_PLAY,
    keypresses=(gtk.keysyms.F6, NO_MODS_MASK),
)
FLOW_GRAPH_KILL = Action(
    label='_Kill',
    tooltip='Kill the flow graph',
    stock_id=gtk.STOCK_STOP,
    keypresses=(gtk.keysyms.F7, NO_MODS_MASK),
)
FLOW_GRAPH_SCREEN_CAPTURE = Action(
    label='Sc_reen Capture',
    tooltip='Create a screen capture of the flow graph',
    stock_id=gtk.STOCK_PRINT,
    keypresses=(gtk.keysyms.Print, NO_MODS_MASK),
)
PORT_CONTROLLER_DEC = Action(
    keypresses=(gtk.keysyms.minus, NO_MODS_MASK, gtk.keysyms.KP_Subtract, NO_MODS_MASK),
)
PORT_CONTROLLER_INC = Action(
    keypresses=(gtk.keysyms.plus, NO_MODS_MASK, gtk.keysyms.KP_Add, NO_MODS_MASK),
)
BLOCK_INC_TYPE = Action(
    keypresses=(gtk.keysyms.Down, NO_MODS_MASK),
)
BLOCK_DEC_TYPE = Action(
    keypresses=(gtk.keysyms.Up, NO_MODS_MASK),
)
RELOAD_BLOCKS = Action(
    label='Reload _Blocks',
    tooltip='Reload Blocks',
    stock_id=gtk.STOCK_REFRESH
)
FIND_BLOCKS = Action(
    label='_Find Blocks',
    tooltip='Search for a block by name (and key)',
    stock_id=gtk.STOCK_FIND,
    keypresses=(gtk.keysyms.f, gtk.gdk.CONTROL_MASK,
                gtk.keysyms.slash, NO_MODS_MASK),
)
CLEAR_REPORTS = Action(
    label='_Clear Reports',
    tooltip='Clear Reports',
    stock_id=gtk.STOCK_CLEAR,
)
SAVE_REPORTS = Action(
    label='_Save Reports',
    tooltip='Save Reports',
    stock_id=gtk.STOCK_SAVE,
)
OPEN_HIER = Action(
    label='Open H_ier',
    tooltip='Open the source of the selected hierarchical block',
    stock_id=gtk.STOCK_JUMP_TO,
)
BUSSIFY_SOURCES = Action(
    label='Toggle So_urce Bus',
    tooltip='Gang source ports into a single bus port',
    stock_id=gtk.STOCK_JUMP_TO,
)
BUSSIFY_SINKS = Action(
    label='Toggle S_ink Bus',
    tooltip='Gang sink ports into a single bus port',
    stock_id=gtk.STOCK_JUMP_TO,
)
XML_PARSER_ERRORS_DISPLAY = Action(
    label='_Parser Errors',
    tooltip='View errors that occured while parsing XML files',
    stock_id=gtk.STOCK_DIALOG_ERROR,
)
TOOLS_RUN_FDESIGN = Action(
    label='Filter Design Tool',
    tooltip='Execute gr_filter_design',
    stock_id=gtk.STOCK_EXECUTE,
)
TOOLS_MORE_TO_COME = Action(
    label='More to come',
)
