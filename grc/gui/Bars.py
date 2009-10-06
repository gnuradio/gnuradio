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

import Actions
import pygtk
pygtk.require('2.0')
import gtk

##The list of actions for the toolbar.
TOOLBAR_LIST = (
	Actions.FLOW_GRAPH_NEW,
	Actions.FLOW_GRAPH_OPEN,
	Actions.FLOW_GRAPH_SAVE,
	Actions.FLOW_GRAPH_CLOSE,
	None,
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
)

##The list of actions and categories for the menu bar.
MENU_BAR_LIST = (
	(gtk.Action('File', '_File', None, None), [
		Actions.FLOW_GRAPH_NEW,
		Actions.FLOW_GRAPH_OPEN,
		None,
		Actions.FLOW_GRAPH_SAVE,
		Actions.FLOW_GRAPH_SAVE_AS,
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
		None,
		Actions.BLOCK_ROTATE_CCW,
		Actions.BLOCK_ROTATE_CW,
		None,
		Actions.BLOCK_ENABLE,
		Actions.BLOCK_DISABLE,
		None,
		Actions.BLOCK_PARAM_MODIFY,
	]),
	(gtk.Action('View', '_View', None, None), [
		Actions.ERRORS_WINDOW_DISPLAY,
	]),
	(gtk.Action('Build', '_Build', None, None), [
		Actions.FLOW_GRAPH_GEN,
		Actions.FLOW_GRAPH_EXEC,
		Actions.FLOW_GRAPH_KILL,
	]),
	(gtk.Action('Help', '_Help', None, None), [
		Actions.HELP_WINDOW_DISPLAY,
		Actions.TYPES_WINDOW_DISPLAY,
		None,
		Actions.ABOUT_WINDOW_DISPLAY,
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
		for action in TOOLBAR_LIST:
			if action: #add a tool item
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
		for main_action, actions in MENU_BAR_LIST:
			#create the main menu item
			main_menu_item = main_action.create_menu_item()
			self.append(main_menu_item)
			#create the menu
			main_menu = gtk.Menu()
			main_menu_item.set_submenu(main_menu)
			for action in actions:
				if action: #append a menu item
					main_menu.append(action.create_menu_item())
				else: main_menu.append(gtk.SeparatorMenuItem())
			main_menu.show_all() #this show all is required for the separators to show
