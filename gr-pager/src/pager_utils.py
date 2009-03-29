#
# Copyright 2008,2009 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from gnuradio import gr
import gnuradio.gr.gr_threading as _threading
from string import split, join, printable
import time

def make_trans_table():
    table = 256 * ['.']
    for i in range(256):
        if (i < 32):
            table[i] = '.'
        else:
            table[i] = chr(i)
    return ''.join(table)

_trans_table = make_trans_table()

def make_printable(s):
    return s.translate(_trans_table)


class queue_runner(_threading.Thread):
    def __init__(self, msgq):
        _threading.Thread.__init__(self)
        self.msgq = msgq
        self.done = False
        self.start()

    def run(self):
        while 1:
            msg = self.msgq.delete_head() # Blocking read
            if msg.type() != 0:
                break
            
            page = join(split(msg.to_string(), chr(128)), '|')
            s = make_printable(page)
            print msg.type(), s
                
    def end(self):
        self.msgq.insert_tail(gr.message(1))
        self.done = True
