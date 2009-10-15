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
import common
from gnuradio import gr

class termsink(gr.hier_block2, common.wxgui_hb):
	def __init__(self,parent):
		gr.hier_block2.__init__(
			self,
			"termsink",
			gr.io_signature(0, 0, 0),
			gr.io_signature(0, 0, 0),
		)

		self.win = term_window.term_window(
			parent=parent,
		)

