"""
Copyright 2008 Free Software Foundation, Inc.
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

from Actions import FLOW_GRAPH_CLOSE
import pygtk
pygtk.require('2.0')
import gtk
from .. utils import ParseXML
from StateCache import StateCache
from .. platforms.base.Constants import FLOW_GRAPH_DTD
import os

############################################################
## Notebook Page
############################################################

class NotebookPage(gtk.HBox):
	"""A page in the notebook."""

	def __init__(self, main_window, flow_graph, file_path=''):
		"""
		Page constructor.
		@param main_window main window
		@param file_path path to a flow graph file
		"""
		self._flow_graph = flow_graph
		self.set_pid(None)
		#import the file
		self.main_window = main_window
		self.set_file_path(file_path)
		file_path = file_path or flow_graph.get_parent().get_default_flow_graph()
		############################################################
		from .. utils import converter
		converter.convert(file_path, flow_graph.get_parent())
		############################################################
		ParseXML.validate_dtd(file_path, FLOW_GRAPH_DTD)
		initial_state = ParseXML.from_file(file_path)
		self.state_cache = StateCache(initial_state)
		self.set_saved(True)
		#import the data to the flow graph
		self.get_flow_graph().import_data(initial_state)
		self.get_flow_graph().update()
		#initialize page gui
		gtk.HBox.__init__(self, False, 0)
		self.show()
		#tab box to hold label and close button
		self.tab = gtk.HBox(False, 0)
		#setup tab label
		self.label = gtk.Label()
		self.tab.pack_start(self.label, False)
		#setup button image
		image = gtk.Image()
		image.set_from_stock('gtk-close', gtk.ICON_SIZE_MENU)
		#setup image box
		image_box = gtk.HBox(False, 0)
		image_box.pack_start(image, True, False, 0)
		#setup the button
		button = gtk.Button()
		button.connect("clicked", self._handle_button)
		button.set_relief(gtk.RELIEF_NONE)
		button.add(image_box)
		#button size
		w, h = gtk.icon_size_lookup_for_settings(button.get_settings(), gtk.ICON_SIZE_MENU)
		button.set_size_request(w+6, h+6)
		self.tab.pack_start(button, False)
		self.tab.show_all()

	def get_generator(self):
		"""
		Get the generator object for this flow graph.
		@return generator
		"""
		return self.get_flow_graph().get_parent().get_generator()(
			self.get_flow_graph(),
			self.get_file_path(),
		)

	def _handle_button(self, button):
		"""
		The button was clicked.
		Make the current page selected, then close.
		@param the button
		"""
		self.main_window.page_to_be_closed = self
		self.main_window.handle_states(FLOW_GRAPH_CLOSE)

	def set_text(self, text):
		"""
		Set the text in this label.
		@param text the new text
		"""
		self.label.set_text(text)

	def get_tab(self):
		"""
		Get the gtk widget for this page's tab.
		@return gtk widget
		"""
		return self.tab

	def get_pid(self):
		"""
		Get the pid for the flow graph.
		@return the pid number
		"""
		return self.pid

	def set_pid(self, pid):
		"""
		Set the pid number.
		@param pid the new pid number
		"""
		self.pid = pid

	def get_flow_graph(self):
		"""
		Get the flow graph.
		@return the flow graph
		"""
		return self._flow_graph

	def get_read_only(self):
		"""
		Get the read-only state of the file.
		Always false for empty path.
		@return true for read-only
		"""
		if not self.get_file_path(): return False
		return not os.access(self.get_file_path(), os.W_OK)

	def get_file_path(self):
		"""
		Get the file path for the flow graph.
		@return the file path or ''
		"""
		return self.file_path

	def set_file_path(self, file_path=''):
		"""
		Set the file path, '' for no file path.
		@param file_path file path string
		"""
		if file_path: self.file_path = os.path.abspath(file_path)
		else: self.file_path = ''

	def get_saved(self):
		"""
		Get the saved status for the flow graph.
		@return true if saved
		"""
		return self.saved

	def set_saved(self, saved=True):
		"""
		Set the saved status.
		@param saved boolean status
		"""
		self.saved = saved

	def get_state_cache(self):
		"""
		Get the state cache for the flow graph.
		@return the state cache
		"""
		return self.state_cache
