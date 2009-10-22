#
# Copyright 2009 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import term_window
from gnuradio import gru

class termsink(object):
	def __init__(self,
		     parent,
		     msgq,
		     size=term_window.DEFAULT_WIN_SIZE,
		     ):
		
		self.win = term_window.term_window(
			parent=parent,
			size=size,
		)

		self.runner = gru.msgq_runner(msgq, self.handle_msg)

	def handle_msg(self, msg):
		# Just append text for now
		text = msg.to_string()
		print "handle_msg: received", len(text), "bytes"
		self.win.append_text(text)
