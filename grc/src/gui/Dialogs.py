"""
Copyright 2008, 2009 Free Software Foundation, Inc.
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

def HelpDialog():
	MessageDialogHelper(
		type=gtk.MESSAGE_INFO,
		buttons=gtk.BUTTONS_CLOSE,
		title='Help',
		markup="""\
<b>Usage Tips</b>

<u>Add block</u>: drag and drop or double click a block in the block selection window.
<u>Rotate block</u>: Select a block, press left/right on the keyboard.
<u>Change type</u>: Select a block, press up/down on the keyboard.
<u>Edit parameters</u>: double click on a block in the flow graph.
<u>Make connection</u>: click on the source port of one block, then click on the sink port of another block.
<u>Remove connection</u>: select the connection and press delete, or drag the connection.

* See the menu for other keyboard shortcuts.""")
