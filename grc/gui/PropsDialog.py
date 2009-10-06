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

from Dialogs import TextDisplay
from Constants import MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT

def get_title_label(title):
	"""
	Get a title label for the params window.
	The title will be bold, underlined, and left justified.
	@param title the text of the title
	@return a gtk object
	"""
	label = gtk.Label()
	label.set_markup('\n<b><span underline="low">%s</span>:</b>\n'%title)
	hbox = gtk.HBox()
	hbox.pack_start(label, False, False, padding=11)
	return hbox

class PropsDialog(gtk.Dialog):
	"""
	A dialog to set block parameters, view errors, and view documentation.
	"""

	def __init__(self, block):
		"""
		Properties dialog contructor.
		@param block a block instance
		"""
		self._hash = 0
		LABEL_SPACING = 7
		gtk.Dialog.__init__(self,
			title='Properties: %s'%block.get_name(),
			buttons=(gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT, gtk.STOCK_OK, gtk.RESPONSE_ACCEPT),
		)
		self._block = block
		self.set_size_request(MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT)
		vbox = gtk.VBox()
		#Create the scrolled window to hold all the parameters
		scrolled_window = gtk.ScrolledWindow()
		scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
		scrolled_window.add_with_viewport(vbox)
		self.vbox.pack_start(scrolled_window, True)
		#Params box for block parameters
		self._params_box = gtk.VBox()
		self._params_box.pack_start(get_title_label('Parameters'), False)
		self._input_object_params = list()
		#Error Messages for the block
		self._error_box = gtk.VBox()
		self._error_messages_text_display = TextDisplay()
		self._error_box.pack_start(gtk.Label(), False, False, LABEL_SPACING)
		self._error_box.pack_start(get_title_label('Error Messages'), False)
		self._error_box.pack_start(self._error_messages_text_display, False)
		#Docs for the block
		self._docs_box = err_box = gtk.VBox()
		self._docs_text_display = TextDisplay()
		self._docs_box.pack_start(gtk.Label(), False, False, LABEL_SPACING)
		self._docs_box.pack_start(get_title_label('Documentation'), False)
		self._docs_box.pack_start(self._docs_text_display, False)
		#Add the boxes
		vbox.pack_start(self._params_box, False)
		vbox.pack_start(self._error_box, False)
		vbox.pack_start(self._docs_box, False)
		#connect events
		self.connect('key-press-event', self._handle_key_press)
		self.connect('show', self._update_gui)
		#show all (performs initial gui update)
		self.show_all()

	def _params_changed(self):
		"""
		Have the params in this dialog changed?
		Ex: Added, removed, type change, hide change...
		To the props dialog, the hide setting of 'none' and 'part' are identical.
		Therfore, the props dialog only cares if the hide setting is/not 'all'.
		Make a hash that uniquely represents the params' state.
		@return true if changed
		"""
		old_hash = self._hash
		#create a tuple of things from each param that affects the params box
		self._hash = hash(tuple([(
			hash(param), param.get_type(), param.get_hide() == 'all',
		) for param in self._block.get_params()]))
		return self._hash != old_hash

	def _handle_changed(self, *args):
		"""
		A change occured within a param:
		Rewrite/validate the block and update the gui.
		"""
		#update for the block
		self._block.rewrite()
		self._block.validate()
		self._update_gui()

	def _update_gui(self, *args):
		"""
		Repopulate the parameters box (if changed).
		Update all the input parameters.
		Update the error messages box.
		Hide the box if there are no errors.
		Update the documentation block.
		Hide the box if there are no docs.
		"""
		#update the params box
		if self._params_changed():
			#hide params box before changing
			self._params_box.hide_all()
			#empty the params box
			for io_param in list(self._input_object_params):
				self._params_box.remove(io_param)
				self._input_object_params.remove(io_param)
				io_param.destroy()
			#repopulate the params box
			for param in self._block.get_params():
				if param.get_hide() == 'all': continue
				io_param = param.get_input(self._handle_changed)
				self._input_object_params.append(io_param)
				self._params_box.pack_start(io_param, False)
			#show params box with new params
			self._params_box.show_all()
		#update the errors box
		if self._block.is_valid(): self._error_box.hide()
		else: self._error_box.show()
		messages = '\n\n'.join(self._block.get_error_messages())
		self._error_messages_text_display.set_text(messages)
		#update the docs box
		if self._block.get_doc(): self._docs_box.show()
		else: self._docs_box.hide()
		self._docs_text_display.set_text(self._block.get_doc())

	def _handle_key_press(self, widget, event):
		"""
		Handle key presses from the keyboard.
		Call the ok response when enter is pressed.
		@return false to forward the keypress
		"""
		if event.keyval == gtk.keysyms.Return:
			self.response(gtk.RESPONSE_ACCEPT)
			return True #handled here
		return False #forward the keypress

	def run(self):
		"""
		Run the dialog and get its response.
		@return true if the response was accept
		"""
		response = gtk.Dialog.run(self)
		self.destroy()
		return response == gtk.RESPONSE_ACCEPT
