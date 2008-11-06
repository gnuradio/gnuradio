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

import pygtk
pygtk.require('2.0')
import gtk
from Constants import MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT
from .. platforms.base.Constants import PACKAGE, VERSION
import Preferences

class TextDisplay(gtk.TextView):
	"""A non editable gtk text view."""

	def __init__(self, text=''):
		"""
		TextDisplay constructor.
		@param text the text to display (string)
		"""
		text_buffer = gtk.TextBuffer()
		text_buffer.set_text(text)
		self.set_text = text_buffer.set_text
		self.insert = lambda line: text_buffer.insert(text_buffer.get_end_iter(), line)
		gtk.TextView.__init__(self, text_buffer)
		self.set_editable(False)
		self.set_cursor_visible(False)
		self.set_wrap_mode(gtk.WRAP_WORD_CHAR)

class PreferencesDialog(gtk.Dialog):
	"""A dialog box to display the preferences."""

	def __init__(self):
		"""PreferencesDialog constructor."""
		gtk.Dialog.__init__(self, buttons=('gtk-close', gtk.RESPONSE_CLOSE))
		self.set_title("Preferences")
		self.set_size_request(MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT)
		notebook = gtk.Notebook()
		for title,desc,params in Preferences.get_preferences():
			vbox = gtk.VBox()
			vbox.pack_start(gtk.Label(''), False) #blank label for spacing
			for param in params: vbox.pack_start(param.get_input_object(), False)
			desc = desc.strip('\n')
			if desc: vbox.pack_start(TextDisplay(desc), False, padding=5)
			notebook.append_page(vbox, gtk.Label(title))
		self.vbox.pack_start(notebook, True)
		self.show_all()
		self.run()
		self.destroy()

def MessageDialogHelper(type, buttons, title=None, markup=None):
	"""
	Create a modal message dialog and run it.
	@param type the type of message: gtk.MESSAGE_INFO, gtk.MESSAGE_WARNING, gtk.MESSAGE_QUESTION or gtk.MESSAGE_ERROR
	@param buttons the predefined set of buttons to use:
		gtk.BUTTONS_NONE, gtk.BUTTONS_OK, gtk.BUTTONS_CLOSE, gtk.BUTTONS_CANCEL, gtk.BUTTONS_YES_NO, gtk.BUTTONS_OK_CANCEL
	@param tittle the title of the window (string)
	@param markup the message text with pango markup
	@return the gtk response from run()
	"""
	message_dialog = gtk.MessageDialog(None, gtk.DIALOG_MODAL, type, buttons)
	if title != None: message_dialog.set_title(title)
	if markup != None: message_dialog.set_markup(markup)
	response = message_dialog.run()
	message_dialog.destroy()
	return response

class AboutDialog(gtk.AboutDialog):
	"""A cute little about dialog."""

	def __init__(self):
		"""AboutDialog constructor."""
		gtk.AboutDialog.__init__(self)
		self.set_name(PACKAGE)
		self.set_version(VERSION)
		self.set_license(__doc__)
		self.set_copyright(__doc__.strip().splitlines()[0])
		self.set_website('http://gnuradio.org/trac/wiki/GNURadioCompanion')
		self.set_comments("""\
Thank you to all those from the mailing list who tested GNU Radio Companion and offered advice.
-----
Special Thanks:
A. Brinton Cooper -> starting the project
Patrick Mulligan -> starting the project
CER Technology Fellowship Grant -> initial funding
William R. Kenan Jr. Fund -> usrp & computers
Patrick Strasser -> the GRC icon
Achilleas Anastasopoulos -> trellis support
-----""")
		self.run()
		self.destroy()

class HotKeysDialog(gtk.Dialog):
	"""Display each action with the associated hotkey."""

	def __init__(self):
		"""HotKeysDialog constructor."""
		gtk.Dialog.__init__(self, buttons=('gtk-close', gtk.RESPONSE_CLOSE))
		self.set_title('Hot Keys')
		markup = ''
		for action, hotkey in (
			('New Flow Graph', 'Ctrl + n'),
			('Open Flow Graph', 'Ctrl + o'),
			('Save Flow Graph', 'Ctrl + s'),
			('Close Flow Graph', 'Ctrl + q'),
			('Cut Block', 'Ctrl + x'),
			('Copy Block', 'Ctrl + c'),
			('Paste Block', 'Ctrl + v'),
			('Undo Change', 'Ctrl + z'),
			('Redo Change', 'Ctrl + y'),
			('Delete Element', 'Delete'),
			('Modify Parameters', 'Enter'),
			('Rotate Block', 'Right'),
			('Rotate Block', 'Left'),
			('Enable Block', 'e'),
			('Disable Block', 'd'),
			('Modify Data Type', 'Up'),
			('Modify Data Type', 'Down'),
			('Add a Port', '+'),
			('Remove a Port', '-'),
			('Flow Graph Generate', 'F5'),
			('Flow Graph Execute', 'F6'),
			('Flow Graph Kill', 'F7'),
			('Screen Shot', 'PrintScreen'),
		): markup = '%s\n<b>%s:</b>%s'%(markup, action, hotkey.rjust(25-len(action), ' '))
		label = gtk.Label()
		label.set_markup('<tt>%s</tt>\n'%markup) #append newline
		self.vbox.pack_start(label, False)
		self.show_all()
		self.run()
		self.destroy()
