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

import os
import signal 
from Constants import DIR_LEFT, DIR_RIGHT, IMAGE_FILE_EXTENSION
import Actions
import pygtk
pygtk.require('2.0')
import gtk
import gobject
import Preferences
from threading import Thread
import Messages
from .. utils import ParseXML
import random
from .. platforms.gui.Platform import Platform
from MainWindow import MainWindow
from Dialogs import PreferencesDialog, AboutDialog, HotKeysDialog
from FileDialogs import OpenFlowGraphFileDialog, SaveFlowGraphFileDialog, SaveImageFileDialog

gobject.threads_init()

class ActionHandler:
	"""
	The action handler will setup all the major window components,
	and handle button presses and flow graph operations from the GUI.
	"""

	def __init__(self, file_paths, platform):
		"""
		ActionHandler constructor.
		Create the main window, setup the message handler, import the preferences,
		and connect all of the action handlers. Finally, enter the gtk main loop and block.
		@param file_paths a list of flow graph file passed from command line
		@param platform platform module
		"""
		self.clipboard = None
		platform = Platform(platform)
		#setup icon using icon theme
		try: gtk.window_set_default_icon(gtk.IconTheme().load_icon('gnuradio-grc', 256, 0))
		except: pass
		for action in Actions.ACTIONS_LIST: action.connect('activate', self._handle_actions)
		#setup the main window
		self.main_window = MainWindow(self.handle_states, platform)
		self.main_window.connect('delete_event', self._quit)
		self.main_window.connect('key_press_event', self._handle_key_press)
		self.get_page = self.main_window.get_page
		self.get_flow_graph = self.main_window.get_flow_graph
		self.get_focus_flag = self.main_window.drawing_area.get_focus_flag
		#setup the messages
		Messages.register_messenger(self.main_window.add_report_line)
		Messages.send_init()
		#initialize
		self.init_file_paths = file_paths
		self.handle_states(Actions.APPLICATION_INITIALIZE)
		#enter the mainloop
		gtk.main()

	def _handle_key_press(self, widget, event):
		"""
		Handle key presses from the keyboard.
		Translate key combos into actions.
		Key combinations that do not include special keys, such as ctrl or Fcn*,
		Also, require that the flow graph has mouse focus when choosing to handle keys.
		@return true if the flow graph is in active use
		"""
		keyname = gtk.gdk.keyval_name(event.keyval)
		ctrl = event.state & gtk.gdk.CONTROL_MASK
		alt = event.state & gtk.gdk.MOD1_MASK
		shift = event.state & gtk.gdk.SHIFT_MASK
		#################### save/open/new/close ###############################
		if ctrl and keyname == 's':
			self.handle_states(Actions.FLOW_GRAPH_SAVE)
		elif ctrl and keyname == 'o':
			self.handle_states(Actions.FLOW_GRAPH_OPEN)
		elif ctrl and keyname == 'n':
			self.handle_states(Actions.FLOW_GRAPH_NEW)
		elif ctrl and keyname == 'q':
			self.handle_states(Actions.FLOW_GRAPH_CLOSE)
		#################### Cut/Copy/Paste ###############################
		elif self.get_focus_flag() and ctrl and keyname == 'x': #mouse focus
			self.handle_states(Actions.BLOCK_CUT)
		elif self.get_focus_flag() and ctrl and keyname == 'c': #mouse focus
			self.handle_states(Actions.BLOCK_COPY)
		elif self.get_focus_flag() and ctrl and keyname == 'v': #mouse focus
			self.handle_states(Actions.BLOCK_PASTE)
		#################### Undo/Redo ###############################
		elif ctrl and keyname == 'z':
			self.handle_states(Actions.FLOW_GRAPH_UNDO)
		elif ctrl and keyname == 'y':
			self.handle_states(Actions.FLOW_GRAPH_REDO)
		#################### Delete ###############################
		elif self.get_focus_flag() and keyname == 'Delete':	#mouse focus
			self.handle_states(Actions.ELEMENT_DELETE)
		#################### Params	###############################
		elif self.get_focus_flag() and keyname == 'Return':	#mouse focus
			self.handle_states(Actions.BLOCK_PARAM_MODIFY)
		#################### Rotate ###############################
		elif self.get_focus_flag() and keyname == 'Right': #mouse focus
			self.handle_states(Actions.BLOCK_ROTATE_RIGHT)
		elif self.get_focus_flag() and keyname == 'Left': #mouse focus
			self.handle_states(Actions.BLOCK_ROTATE_LEFT)
		#################### Enable/Disable ###############################
		elif self.get_focus_flag() and keyname == 'e': #mouse focus
			self.handle_states(Actions.BLOCK_ENABLE)
		elif self.get_focus_flag() and keyname == 'd': #mouse focus
			self.handle_states(Actions.BLOCK_DISABLE)
		#################### Data Type ###############################
		elif self.get_focus_flag() and keyname == 'Down': #mouse focus
			self.handle_states(Actions.BLOCK_INC_TYPE)
		elif self.get_focus_flag() and keyname == 'Up': #mouse focus
			self.handle_states(Actions.BLOCK_DEC_TYPE)
		#################### Port Controllers ###############################
		elif self.get_focus_flag() and keyname in ('equal','plus', 'KP_Add'): #mouse focus
			self.handle_states(Actions.PORT_CONTROLLER_INC)
		elif self.get_focus_flag() and keyname in ('minus', 'KP_Subtract'): #mouse focus
			self.handle_states(Actions.PORT_CONTROLLER_DEC)
		#################### Gen/Exec/Stop/Print ###############################
		elif keyname == 'F5':
			self.handle_states(Actions.FLOW_GRAPH_GEN)
		elif keyname == 'F6':
			self.handle_states(Actions.FLOW_GRAPH_EXEC)
		elif keyname == 'F7':
			self.handle_states(Actions.FLOW_GRAPH_KILL)
		elif keyname == 'Print':
			self.handle_states(Actions.FLOW_GRAPH_SCREEN_CAPTURE)
		#propagate this if the fg is not in focus or nothing is selected
		return self.get_focus_flag() and self.get_flow_graph().is_selected()

	def _quit(self, window, event):
		"""
		Handle the delete event from the main window.
		Generated by pressing X to close, alt+f4, or right click+close.
		This method in turns calls the state handler to quit.
		@return true
		"""
		self.handle_states(Actions.APPLICATION_QUIT)
		return True

	def _handle_actions(self, event):
		"""
		Handle all of the activate signals from the gtk actions.
		The action signals derive from clicking on a toolbar or menu bar button.
		Forward the action to the state handler.
		"""
		self.handle_states(event.get_name())

	def handle_states(self, state=''):
		"""
		Handle the state changes in the GUI.
		Handle all of the state changes that arise from the action handler or other gui and
		inputs in the application. The state passed to the handle_states method is a string descriping
		the change. A series of if/elif statements handle the state by greying out action buttons, causing
		changes in the flow graph, saving/opening files... The handle_states method is passed to the
		contructors of many of the classes used in this application enabling them to report any state change.
		@param state a string describing the state change
		"""
		#print state
		##################################################
		# Initalize/Quit
		##################################################
		if state == Actions.APPLICATION_INITIALIZE:
			for action in Actions.ACTIONS_LIST: action.set_sensitive(False) #set all actions disabled
			# enable a select few actions
			for action in (
				Actions.APPLICATION_QUIT, Actions.FLOW_GRAPH_NEW,
				Actions.FLOW_GRAPH_OPEN, Actions.FLOW_GRAPH_SAVE_AS,
				Actions.FLOW_GRAPH_CLOSE, Actions.ABOUT_WINDOW_DISPLAY,
				Actions.HOTKEYS_WINDOW_DISPLAY, Actions.PREFS_WINDOW_DISPLAY,
				Actions.FLOW_GRAPH_SCREEN_CAPTURE,
			): Actions.get_action_from_name(action).set_sensitive(True)
			if not self.init_file_paths and Preferences.restore_files():
				self.init_file_paths = Preferences.files_open()
			if not self.init_file_paths: self.init_file_paths = ['']
			for file_path in self.init_file_paths:
				if file_path: self.main_window.new_page(file_path) #load pages from file paths
			if Preferences.file_open() in self.init_file_paths:
				self.main_window.new_page(Preferences.file_open(), show=True)
			if not self.get_page():	self.main_window.new_page() #ensure that at least a blank page exists
		elif state == Actions.APPLICATION_QUIT:
			if self.main_window.close_pages():
				gtk.main_quit()
				exit(0)
		##################################################
		# Selections
		##################################################
		elif state == Actions.ELEMENT_SELECT:
			self.get_flow_graph().update()
		elif state == Actions.NOTHING_SELECT:
			self.get_flow_graph().unselect()
			self.get_flow_graph().update()
		##################################################
		# Enable/Disable
		##################################################
		elif state == Actions.BLOCK_ENABLE:
			if self.get_flow_graph().enable_selected(True):
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		elif state == Actions.BLOCK_DISABLE:
			if self.get_flow_graph().enable_selected(False):
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		##################################################
		# Cut/Copy/Paste
		##################################################
		elif state == Actions.BLOCK_CUT:
			self.handle_states(BLOCK_COPY)
			self.handle_states(ELEMENT_DELETE)
		elif state == Actions.BLOCK_COPY:
			self.clipboard = self.get_flow_graph().copy_to_clipboard()
		elif state == Actions.BLOCK_PASTE:
			if self.clipboard:
				self.get_flow_graph().paste_from_clipboard(self.clipboard)
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		##################################################
		# Move/Rotate/Delete/Create
		##################################################
		elif state == Actions.BLOCK_MOVE:
			self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
			self.get_page().set_saved(False)
		elif state == Actions.BLOCK_ROTATE_LEFT:
			if self.get_flow_graph().rotate_selected(DIR_LEFT):
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		elif state == Actions.BLOCK_ROTATE_RIGHT:
			if self.get_flow_graph().rotate_selected(DIR_RIGHT):
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		elif state == Actions.ELEMENT_DELETE:
			if self.get_flow_graph().remove_selected():
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.handle_states(Actions.NOTHING_SELECT)
				self.get_page().set_saved(False)
		elif state == Actions.ELEMENT_CREATE:
			self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
			self.handle_states(Actions.NOTHING_SELECT)
			self.get_page().set_saved(False)
		elif state == Actions.BLOCK_INC_TYPE:
			if self.get_flow_graph().type_controller_modify_selected(1):
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		elif state == Actions.BLOCK_DEC_TYPE:
			if self.get_flow_graph().type_controller_modify_selected(-1):
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		elif state == Actions.PORT_CONTROLLER_INC:
			if self.get_flow_graph().port_controller_modify_selected(1):
				self.get_flow_graph().update()
				self.get_flow_graph().update() #2 times
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		elif state == Actions.PORT_CONTROLLER_DEC:
			if self.get_flow_graph().port_controller_modify_selected(-1):
				self.get_flow_graph().update()
				self.get_flow_graph().update() #2 times
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		##################################################
		# Window stuff
		##################################################
		elif state == Actions.PREFS_WINDOW_DISPLAY:
			PreferencesDialog()
			self.get_flow_graph().update()
		elif state == Actions.ABOUT_WINDOW_DISPLAY:
			AboutDialog()
		elif state == Actions.HOTKEYS_WINDOW_DISPLAY:
			HotKeysDialog()
		##################################################
		# Param Modifications
		##################################################
		elif state == Actions.BLOCK_PARAM_MODIFY:
			if self.get_flow_graph().param_modify_selected():
				self.get_flow_graph().update()
				self.get_page().get_state_cache().save_new_state(self.get_flow_graph().export_data())
				self.get_page().set_saved(False)
		##################################################
		# Undo/Redo
		##################################################
		elif state == Actions.FLOW_GRAPH_UNDO:
			n = self.get_page().get_state_cache().get_prev_state()
			if n:
				self.get_flow_graph().unselect()
				self.get_flow_graph().import_data(n)
				self.get_flow_graph().update()
				self.get_page().set_saved(False)
		elif state == Actions.FLOW_GRAPH_REDO:
			n = self.get_page().get_state_cache().get_next_state()
			if n:
				self.get_flow_graph().unselect()
				self.get_flow_graph().import_data(n)
				self.get_flow_graph().update()
				self.get_page().set_saved(False)
		##################################################
		# New/Open/Save/Close
		##################################################
		elif state == Actions.FLOW_GRAPH_NEW:
			self.main_window.new_page()
		elif state == Actions.FLOW_GRAPH_OPEN:
			file_paths = OpenFlowGraphFileDialog(self.get_page().get_file_path()).run()
			if file_paths: #open a new page for each file, show only the first
				for i,file_path in enumerate(file_paths):
					self.main_window.new_page(file_path, show=(i==0))
		elif state == Actions.FLOW_GRAPH_CLOSE:
			self.main_window.close_page()
		elif state == Actions.FLOW_GRAPH_SAVE:
			if not self.get_page().get_file_path(): self.handle_states(Actions.FLOW_GRAPH_SAVE_AS)
			else:
				try:
					ParseXML.to_file(self.get_flow_graph().export_data(), self.get_page().get_file_path())
					self.get_page().set_saved(True)
				except IOError:
					Messages.send_fail_save(self.get_page().get_file_path())
					self.get_page().set_saved(False)
		elif state == Actions.FLOW_GRAPH_SAVE_AS:
			file_path = SaveFlowGraphFileDialog(self.get_page().get_file_path()).run()
			if file_path != None:
				self.get_page().set_file_path(file_path)
				self.handle_states(Actions.FLOW_GRAPH_SAVE)
		elif state == Actions.FLOW_GRAPH_SCREEN_CAPTURE:
			file_path = SaveImageFileDialog(self.get_page().get_file_path()).run()
			if file_path != None:
				pixmap = self.get_flow_graph().get_drawing_area().pixmap
				width, height = pixmap.get_size()
				pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, 0, 8, width, height)
				pixbuf.get_from_drawable(pixmap, pixmap.get_colormap(), 0, 0, 0, 0, width, height)
				pixbuf.save(file_path, IMAGE_FILE_EXTENSION[1:])
		##################################################
		# Gen/Exec/Stop
		##################################################
		elif state == Actions.FLOW_GRAPH_GEN:
			if not self.get_page().get_pid():
				if not self.get_page().get_saved() or not self.get_page().get_file_path():
					self.handle_states(Actions.FLOW_GRAPH_SAVE) #only save if file path missing or not saved
				if self.get_page().get_saved() and self.get_page().get_file_path():
					generator = self.get_page().get_generator()
					try:
						Messages.send_start_gen(generator.get_file_path())
						generator.write()
					except Exception,e: Messages.send_fail_gen(e)
				else: self.generator = None
		elif state == Actions.FLOW_GRAPH_EXEC:
			if not self.get_page().get_pid():
				self.handle_states(Actions.FLOW_GRAPH_GEN)
				if self.get_page().get_saved() and self.get_page().get_file_path():
					ExecFlowGraphThread(self)
		elif state == Actions.FLOW_GRAPH_KILL:
			if self.get_page().get_pid():
				try: os.kill(self.get_page().get_pid(), signal.SIGKILL)
				except: print "could not kill pid: %s"%self.get_page().get_pid()
		elif state == '': #pass and run the global actions
			pass
		else: print '!!! State "%s" not handled !!!'%state
		##################################################
		# Global Actions for all States
		##################################################
		#update general buttons
		Actions.get_action_from_name(Actions.ELEMENT_DELETE).set_sensitive(bool(self.get_flow_graph().get_selected_elements()))
		Actions.get_action_from_name(Actions.BLOCK_PARAM_MODIFY).set_sensitive(bool(self.get_flow_graph().get_selected_block()))
		Actions.get_action_from_name(Actions.BLOCK_ROTATE_RIGHT).set_sensitive(bool(self.get_flow_graph().get_selected_blocks()))
		Actions.get_action_from_name(Actions.BLOCK_ROTATE_LEFT).set_sensitive(bool(self.get_flow_graph().get_selected_blocks()))
		#update cut/copy/paste
		Actions.get_action_from_name(Actions.BLOCK_CUT).set_sensitive(bool(self.get_flow_graph().get_selected_blocks()))
		Actions.get_action_from_name(Actions.BLOCK_COPY).set_sensitive(bool(self.get_flow_graph().get_selected_blocks()))
		Actions.get_action_from_name(Actions.BLOCK_PASTE).set_sensitive(bool(self.clipboard))
		#update enable/disable
		Actions.get_action_from_name(Actions.BLOCK_ENABLE).set_sensitive(bool(self.get_flow_graph().get_selected_blocks()))
		Actions.get_action_from_name(Actions.BLOCK_DISABLE).set_sensitive(bool(self.get_flow_graph().get_selected_blocks()))
		#set the exec and stop buttons
		self.update_exec_stop()
		#saved status
		Actions.get_action_from_name(Actions.FLOW_GRAPH_SAVE).set_sensitive(not self.get_page().get_saved())
		self.main_window.update()
		#draw the flow graph
		self.get_flow_graph().draw()

	def update_exec_stop(self):
		"""
		Update the exec and stop buttons.
		Lock and unlock the mutex for race conditions with exec flow graph threads.
		"""
		sensitive = self.get_flow_graph().is_valid() and not self.get_page().get_pid()
		Actions.get_action_from_name(Actions.FLOW_GRAPH_GEN).set_sensitive(sensitive)
		Actions.get_action_from_name(Actions.FLOW_GRAPH_EXEC).set_sensitive(sensitive)
		Actions.get_action_from_name(Actions.FLOW_GRAPH_KILL).set_sensitive(self.get_page().get_pid() != None)

class ExecFlowGraphThread(Thread):
	"""Execute the flow graph as a new process and wait on it to finish."""

	def __init__ (self, action_handler):
		"""
		ExecFlowGraphThread constructor.
		@param action_handler an instance of an ActionHandler
		"""
		Thread.__init__(self)
		self.update_exec_stop = action_handler.update_exec_stop
		self.flow_graph = action_handler.get_flow_graph()
		#store page and dont use main window calls in run
		self.page = action_handler.get_page()
		Messages.send_start_exec(self.page.get_generator().get_file_path())
		#get the popen
		try:
			self.p = self.page.get_generator().get_popen()
			self.page.set_pid(self.p.pid)
			#update
			self.update_exec_stop()
			self.start()
		except Exception, e:
			Messages.send_verbose_exec(str(e))
			Messages.send_end_exec()

	def run(self):
		"""
		Wait on the executing process by reading from its stdout.
		Use gobject.idle_add when calling functions that modify gtk objects.
		"""
		#handle completion
		r = "\n"
		while(r):
			gobject.idle_add(Messages.send_verbose_exec, r)
			r = os.read(self.p.stdout.fileno(), 1024)
		gobject.idle_add(self.done)

	def done(self):
		"""Perform end of execution tasks."""
		Messages.send_end_exec()
		self.page.set_pid(None)
		self.update_exec_stop()
