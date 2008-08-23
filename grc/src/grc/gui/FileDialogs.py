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
##@package grc.gui.FileDialogs
#The open/save dialog for flow graph fFileDialogiles and screen shots.
#@author Josh Blum

import pygtk
pygtk.require('2.0')
import gtk
from Dialogs import MessageDialogHelper
from grc.Constants import DEFAULT_FILE_PATH,FLOW_GRAPH_FILE_EXTENSION,IMAGE_FILE_EXTENSION,NEW_FLOGRAPH_TITLE
from os import path

OPEN_FLOW_GRAPH = 'open flow graph'
SAVE_FLOW_GRAPH = 'save flow graph'
SAVE_IMAGE = 'save image'

##the filter for flow graph files
FLOW_GRAPH_FILE_FILTER = gtk.FileFilter()
FLOW_GRAPH_FILE_FILTER.set_name('GRC Files')
FLOW_GRAPH_FILE_FILTER.add_pattern('*'+FLOW_GRAPH_FILE_EXTENSION)
FLOW_GRAPH_FILE_FILTER.add_pattern('*.xml') #TEMP

##the filter for image files
IMAGE_FILE_FILTER = gtk.FileFilter()
IMAGE_FILE_FILTER.set_name('Image Files')
IMAGE_FILE_FILTER.add_pattern('*'+IMAGE_FILE_EXTENSION)

##the filter for all files
ALL_FILE_FILTER = gtk.FileFilter()
ALL_FILE_FILTER.set_name('All Files')
ALL_FILE_FILTER.add_pattern('*')

class FileDialogHelper(gtk.FileChooserDialog):
	"""
	A wrapper class for the gtk file chooser dialog.
	Implement a file chooser dialog with only necessary parameters.
	"""

	def __init__(self, action, title):
		"""!
		FileDialogHelper contructor.
		Create a save or open dialog with cancel and ok buttons.
		Use standard settings: no multiple selection, local files only, and the * filter.
		@param action gtk.FILE_CHOOSER_ACTION_OPEN or gtk.FILE_CHOOSER_ACTION_SAVE
		@param title the title of the dialog (string)
		"""
		ok_stock = {gtk.FILE_CHOOSER_ACTION_OPEN : 'gtk-open', gtk.FILE_CHOOSER_ACTION_SAVE : 'gtk-save'}[action]
		gtk.FileChooserDialog.__init__(self, title, None, action, ('gtk-cancel', gtk.RESPONSE_CANCEL, ok_stock, gtk.RESPONSE_OK))
		self.set_select_multiple(False)
		self.set_local_only(True)
		self.add_filter(ALL_FILE_FILTER)

class FileDialog(FileDialogHelper):
	"""A dialog box to save or open flow graph files. This is a base class, do not use."""

	def __init__(self, current_file_path=''):
		"""!
		FileDialog constructor.
		@param current_file_path the current directory or path to the open flow graph
		"""
		if not current_file_path: current_file_path = path.join(DEFAULT_FILE_PATH, NEW_FLOGRAPH_TITLE + FLOW_GRAPH_FILE_EXTENSION)
		if self.type == OPEN_FLOW_GRAPH:
			FileDialogHelper.__init__(self, gtk.FILE_CHOOSER_ACTION_OPEN, 'Open a Flow Graph from a File...')
			self.add_and_set_filter(FLOW_GRAPH_FILE_FILTER)
			self.set_select_multiple(True)
		elif self.type == SAVE_FLOW_GRAPH:
			FileDialogHelper.__init__(self, gtk.FILE_CHOOSER_ACTION_SAVE, 'Save a Flow Graph to a File...')
			self.add_and_set_filter(FLOW_GRAPH_FILE_FILTER)
			self.set_current_name(path.basename(current_file_path)) #show the current filename
		elif self.type == SAVE_IMAGE:
			FileDialogHelper.__init__(self, gtk.FILE_CHOOSER_ACTION_SAVE, 'Save a Flow Graph Screen Shot...')
			self.add_and_set_filter(IMAGE_FILE_FILTER)
			current_file_path = current_file_path + IMAGE_FILE_EXTENSION
			self.set_current_name(path.basename(current_file_path)) #show the current filename
		self.set_current_folder(path.dirname(current_file_path)) #current directory

	def add_and_set_filter(self, filter):
		"""!
		Add the gtk file filter to the list of filters and set it as the default file filter.
		@param filter a gtk file filter.
		"""
		self.add_filter(filter)
		self.set_filter(filter)

	def get_rectified_filename(self):
		"""!
		Run the dialog and get the filename.
		If this is a save dialog and the file name is missing the extension, append the file extension.
		If the file name with the extension already exists, show a overwrite dialog.
		If this is an open dialog, return a list of filenames.
		@return the complete file path
		"""
		if gtk.FileChooserDialog.run(self) != gtk.RESPONSE_OK: return None #response was cancel
		#############################################
		# Handle Save Dialogs
		#############################################
		if self.type in (SAVE_FLOW_GRAPH, SAVE_IMAGE):
			filename = self.get_filename()
			for extension, filter in (
				(FLOW_GRAPH_FILE_EXTENSION, FLOW_GRAPH_FILE_FILTER),
				(IMAGE_FILE_EXTENSION, IMAGE_FILE_FILTER),
			): #append the missing file extension if the filter matches
				if filename[len(filename)-len(extension):] != extension \
					and filter == self.get_filter(): filename += extension
			self.set_current_name(path.basename(filename)) #show the filename with extension
			if path.exists(filename): #ask the user to confirm overwrite
				if MessageDialogHelper(
					gtk.MESSAGE_QUESTION, gtk.BUTTONS_YES_NO, 'Confirm Overwrite!',
					'File <b>"%s"</b> Exists!\nWould you like to overwrite the existing file?'%filename,
				) == gtk.RESPONSE_NO: return self.get_rectified_filename()
			return filename
		#############################################
		# Handle Open Dialogs
		#############################################
		elif self.type in (OPEN_FLOW_GRAPH,):
			filenames = self.get_filenames()
			for filename in filenames:
				if not path.exists(filename): #show a warning and re-run
					MessageDialogHelper(
						gtk.MESSAGE_WARNING, gtk.BUTTONS_CLOSE, 'Cannot Open!',
						'File <b>"%s"</b> Does not Exist!'%filename,
					)
					return self.get_rectified_filename()
			return filenames

	def run(self):
		"""!
		Get the filename and destroy the dialog.
		@return the filename or None if a close/cancel occured.
		"""
		filename = self.get_rectified_filename()
		self.destroy()
		return filename

class OpenFlowGraphFileDialog(FileDialog): type = OPEN_FLOW_GRAPH
class SaveFlowGraphFileDialog(FileDialog): type = SAVE_FLOW_GRAPH
class SaveImageFileDialog(FileDialog): type = SAVE_IMAGE

