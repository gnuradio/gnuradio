"""
Copyright 2007 Free Software Foundation, Inc.
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
##@package grc.gui.Bars
#Create the GUI's toolbar and menubar
#@author Josh Blum

from grc.Actions import *
import pygtk
pygtk.require('2.0')
import gtk

##The list of actions for the toolbar.
TOOLBAR_LIST = (
	FLOW_GRAPH_NEW,
	FLOW_GRAPH_OPEN,
	FLOW_GRAPH_SAVE,
	FLOW_GRAPH_CLOSE,
	None,
	FLOW_GRAPH_SCREEN_CAPTURE,
	None,
	BLOCK_CUT,
	BLOCK_COPY,
	BLOCK_PASTE,
	ELEMENT_DELETE,
	None,
	FLOW_GRAPH_UNDO,
	FLOW_GRAPH_REDO,
	None,
	FLOW_GRAPH_GEN,
	FLOW_GRAPH_EXEC,
	FLOW_GRAPH_KILL,
	None,
	BLOCK_ROTATE_LEFT,
	BLOCK_ROTATE_RIGHT,
	None,
	BLOCK_ENABLE,
	BLOCK_DISABLE,
)

##The list of actions and categories for the menu bar.
MENU_BAR_LIST = (
	(gtk.Action('File', '_File', None, None), [
		FLOW_GRAPH_NEW,
		FLOW_GRAPH_OPEN,
		None,
		FLOW_GRAPH_SAVE,
		FLOW_GRAPH_SAVE_AS,
		None,
		FLOW_GRAPH_SCREEN_CAPTURE,
		None,
		FLOW_GRAPH_CLOSE,
		APPLICATION_QUIT,
	]),
	(gtk.Action('Edit', '_Edit', None, None), [
		FLOW_GRAPH_UNDO,
		FLOW_GRAPH_REDO,
		None,
		BLOCK_CUT,
		BLOCK_COPY,
		BLOCK_PASTE,
		ELEMENT_DELETE,
		None,
		BLOCK_ROTATE_LEFT,
		BLOCK_ROTATE_RIGHT,
		None,
		BLOCK_ENABLE,
		BLOCK_DISABLE,
		None,
		BLOCK_PARAM_MODIFY,
	]),
	(gtk.Action('Build', '_Build', None, None), [
		FLOW_GRAPH_GEN,
		FLOW_GRAPH_EXEC,
		FLOW_GRAPH_KILL,
	]),
	(gtk.Action('Options', '_Options', None, None), [
		PREFS_WINDOW_DISPLAY,
	]),
	(gtk.Action('Help', '_Help', None, None), [
		ABOUT_WINDOW_DISPLAY,
		HOTKEYS_WINDOW_DISPLAY,
	]),
)

class Toolbar(gtk.Toolbar):
	"""The gtk toolbar with actions added from the toolbar list."""

	def __init__(self):
		"""
		Parse the list of action names in the toolbar list.
		Look up the action for each name in the action list and add it to the toolbar.
		"""
		gtk.Toolbar.__init__(self)
		self.set_style(gtk.TOOLBAR_ICONS)
		for action_name in TOOLBAR_LIST:
			if action_name:	#add a tool item
				action = get_action_from_name(action_name)
				self.add(action.create_tool_item())
				#this reset of the tooltip property is required (after creating the tool item) for the tooltip to show
				action.set_property('tooltip', action.get_property('tooltip'))
			else: self.add(gtk.SeparatorToolItem())

class MenuBar(gtk.MenuBar):
	"""The gtk menu bar with actions added from the menu bar list."""

	def __init__(self):
		"""
		Parse the list of submenus from the menubar list.
		For each submenu, get a list of action names.
		Look up the action for each name in the action list and add it to the submenu.
		Add the submenu to the menu bar.
		"""
		gtk.MenuBar.__init__(self)
		for main_action,action_names in MENU_BAR_LIST:
			#create the main menu item
			main_menu_item = main_action.create_menu_item()
			self.append(main_menu_item)
			#create the menu
			main_menu = gtk.Menu()
			main_menu_item.set_submenu(main_menu)
			for action_name in action_names:
				if action_name: #append a menu item
					action = get_action_from_name(action_name)
					main_menu.append(action.create_menu_item())
				else: main_menu.append(gtk.SeparatorMenuItem())
			main_menu.show_all() #this show all is required for the separators to show

