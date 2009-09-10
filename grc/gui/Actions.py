"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
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

######################################################################################################
# Action Names
######################################################################################################
APPLICATION_INITIALIZE = 'app init'
APPLICATION_QUIT = 'app quit'
PARAM_MODIFY = 'param modify'
BLOCK_MOVE = 'block move'
BLOCK_ROTATE_CCW = 'block rotate ccw'
BLOCK_ROTATE_CW = 'block rotate cw'
BLOCK_PARAM_MODIFY = 'block param modify'
BLOCK_INC_TYPE = 'block increment type'
BLOCK_DEC_TYPE = 'block decrement type'
BLOCK_ENABLE = 'block enable'
BLOCK_DISABLE = 'block disable'
BLOCK_CUT = 'block cut'
BLOCK_COPY = 'block copy'
BLOCK_PASTE = 'block paste'
PORT_CONTROLLER_INC = 'port controller increment'
PORT_CONTROLLER_DEC = 'port controller decrement'
ELEMENT_CREATE = 'element create'
ELEMENT_DELETE = 'element delete'
ELEMENT_SELECT = 'element select'
NOTHING_SELECT = 'nothing select'
FLOW_GRAPH_OPEN = 'flow graph open'
FLOW_GRAPH_UNDO = 'flow graph undo'
FLOW_GRAPH_REDO = 'flow graph redo'
FLOW_GRAPH_SAVE = 'flow graph save'
FLOW_GRAPH_SAVE_AS = 'flow graph save as'
FLOW_GRAPH_CLOSE = 'flow graph close'
FLOW_GRAPH_NEW = 'flow graph new'
FLOW_GRAPH_GEN = 'flow graph gen'
FLOW_GRAPH_EXEC = 'flow graph exec'
FLOW_GRAPH_KILL = 'flow graph kill'
FLOW_GRAPH_SCREEN_CAPTURE = 'flow graph screen capture'
ABOUT_WINDOW_DISPLAY = 'about window display'
HELP_WINDOW_DISPLAY = 'help window display'
TYPES_WINDOW_DISPLAY = 'types window display'

######################################################################################################
# Action Key Map
######################################################################################################
_actions_key_list = (
	#action name, key name, mod mask
	(FLOW_GRAPH_NEW, 'n', gtk.gdk.CONTROL_MASK),
	(FLOW_GRAPH_OPEN, 'o', gtk.gdk.CONTROL_MASK),
	(FLOW_GRAPH_SAVE, 's', gtk.gdk.CONTROL_MASK),
	(FLOW_GRAPH_SAVE_AS, 's', gtk.gdk.CONTROL_MASK | gtk.gdk.SHIFT_MASK),
	(FLOW_GRAPH_CLOSE, 'w', gtk.gdk.CONTROL_MASK),
	(APPLICATION_QUIT, 'q', gtk.gdk.CONTROL_MASK),
	(FLOW_GRAPH_UNDO, 'z', gtk.gdk.CONTROL_MASK),
	(FLOW_GRAPH_REDO, 'y', gtk.gdk.CONTROL_MASK),
	(ELEMENT_DELETE, 'Delete', 0),
	(BLOCK_ROTATE_CCW, 'Left', 0),
	(BLOCK_ROTATE_CW, 'Right', 0),
	(BLOCK_DEC_TYPE, 'Up', 0),
	(BLOCK_INC_TYPE, 'Down', 0),
	(BLOCK_PARAM_MODIFY, 'Return', 0),
	(BLOCK_ENABLE, 'e', 0),
	(BLOCK_DISABLE, 'd', 0),
	(BLOCK_CUT, 'x', gtk.gdk.CONTROL_MASK),
	(BLOCK_COPY, 'c', gtk.gdk.CONTROL_MASK),
	(BLOCK_PASTE, 'v', gtk.gdk.CONTROL_MASK),
	(FLOW_GRAPH_GEN, 'F5', 0),
	(FLOW_GRAPH_EXEC, 'F6', 0),
	(FLOW_GRAPH_KILL, 'F7', 0),
	(FLOW_GRAPH_SCREEN_CAPTURE, 'Print', 0),
	(HELP_WINDOW_DISPLAY, 'F1', 0),
	#the following have no associated gtk.Action
	(PORT_CONTROLLER_INC, 'equal', 0),
	(PORT_CONTROLLER_INC, 'plus', 0),
	(PORT_CONTROLLER_INC, 'KP_Add', 0),
	(PORT_CONTROLLER_DEC, 'minus', 0),
	(PORT_CONTROLLER_DEC, 'KP_Subtract', 0),
)

_actions_key_dict = dict(((key_name, mod_mask), action_name) for action_name, key_name, mod_mask in _actions_key_list)
_all_mods_mask = reduce(lambda x, y: x | y, [mod_mask for action_name, key_name, mod_mask in _actions_key_list], 0)
def get_action_name_from_key_name(key_name, mod_mask=0):
	"""
	Get the action name associated with the key name and mask.
	Both keyname and mask have to match.
	@param key_name the name of the key
	@param mod_mask the key press mask (shift, ctrl) 0 for none
	@return the action name or blank string
	"""
	mod_mask &= _all_mods_mask #ignore irrelevant modifiers
	key_name_mod_mask = (key_name, mod_mask)
	if key_name_mod_mask in _actions_key_dict: return _actions_key_dict[key_name_mod_mask]
	return ''

######################################################################################################
# Actions
######################################################################################################
_actions_list = (
	gtk.Action(FLOW_GRAPH_NEW, '_New', 'Create a new flow graph', gtk.STOCK_NEW),
	gtk.Action(FLOW_GRAPH_OPEN, '_Open', 'Open an existing flow graph', gtk.STOCK_OPEN),
	gtk.Action(FLOW_GRAPH_SAVE, '_Save', 'Save the current flow graph', gtk.STOCK_SAVE),
	gtk.Action(FLOW_GRAPH_SAVE_AS, 'Save _As', 'Save the current flow graph as...', gtk.STOCK_SAVE_AS),
	gtk.Action(FLOW_GRAPH_CLOSE, '_Close', 'Close the current flow graph', gtk.STOCK_CLOSE),
	gtk.Action(APPLICATION_QUIT, '_Quit', 'Quit program', gtk.STOCK_QUIT),
	gtk.Action(FLOW_GRAPH_UNDO, '_Undo', 'Undo a change to the flow graph', gtk.STOCK_UNDO),
	gtk.Action(FLOW_GRAPH_REDO, '_Redo', 'Redo a change to the flow graph', gtk.STOCK_REDO),
	gtk.Action(ELEMENT_DELETE, '_Delete', 'Delete the selected blocks', gtk.STOCK_DELETE),
	gtk.Action(BLOCK_ROTATE_CCW, 'Rotate Counterclockwise', 'Rotate the selected blocks 90 degrees to the left', gtk.STOCK_GO_BACK),
	gtk.Action(BLOCK_ROTATE_CW, 'Rotate Clockwise', 'Rotate the selected blocks 90 degrees to the right', gtk.STOCK_GO_FORWARD),
	gtk.Action(BLOCK_PARAM_MODIFY, '_Properties', 'Modify params for the selected block', gtk.STOCK_PROPERTIES),
	gtk.Action(BLOCK_ENABLE, 'E_nable', 'Enable the selected blocks', gtk.STOCK_CONNECT),
	gtk.Action(BLOCK_DISABLE, 'D_isable', 'Disable the selected blocks', gtk.STOCK_DISCONNECT),
	gtk.Action(BLOCK_CUT, 'Cu_t', 'Cut', gtk.STOCK_CUT),
	gtk.Action(BLOCK_COPY, '_Copy', 'Copy', gtk.STOCK_COPY),
	gtk.Action(BLOCK_PASTE, '_Paste', 'Paste', gtk.STOCK_PASTE),
	gtk.Action(ABOUT_WINDOW_DISPLAY, '_About', 'About this program', gtk.STOCK_ABOUT),
	gtk.Action(HELP_WINDOW_DISPLAY, '_Help', 'Usage Tips', gtk.STOCK_HELP),
	gtk.Action(TYPES_WINDOW_DISPLAY, '_Types', 'Types Color Mapping', gtk.STOCK_DIALOG_INFO),
	gtk.Action(FLOW_GRAPH_GEN, '_Generate', 'Generate the flow graph', gtk.STOCK_CONVERT),
	gtk.Action(FLOW_GRAPH_EXEC, '_Execute', 'Execute the flow graph', gtk.STOCK_EXECUTE),
	gtk.Action(FLOW_GRAPH_KILL, '_Kill', 'Kill the flow graph', gtk.STOCK_STOP),
	gtk.Action(FLOW_GRAPH_SCREEN_CAPTURE, 'S_creen Capture', 'Create a screen capture of the flow graph', gtk.STOCK_PRINT),
)
def get_all_actions(): return _actions_list

_actions_dict = dict((action.get_name(), action) for action in _actions_list)
def get_action_from_name(action_name):
	"""
	Retrieve the action from the action list.
	Search the list and find an action with said name.
	@param action_name the action name(string)
	@throw KeyError bad action name
	@return a gtk action object
	"""
	if action_name in _actions_dict: return _actions_dict[action_name]
	raise KeyError('Action Name: "%s" does not exist'%action_name)

######################################################################################################
# Accelerators
######################################################################################################
_accel_group = gtk.AccelGroup()
def get_accel_group(): return _accel_group

#set the accelerator group, and accelerator path
#register the key name and mod mask with the accelerator path
for action_name, key_name, mod_mask in _actions_key_list:
	try:
		accel_path = '<main>/'+action_name
		get_action_from_name(action_name).set_accel_group(get_accel_group())
		get_action_from_name(action_name).set_accel_path(accel_path)
		gtk.accel_map_add_entry(accel_path, gtk.gdk.keyval_from_name(key_name), mod_mask)
	except KeyError: pass #no action was created for this action name
