#
# Copyright 2003-2012 Free Software Foundation, Inc.
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
""" Conversion tools between stream tags and Python objects """

try: import pmt
except: from gruel import pmt

from gnuradio_core import gr_tag_t

class PythonTag(object):
    " Python container for tags "
    def __init__(self):
        self.offset = None
        self.key    = None
        self.value  = None
        self.srcid  = None

def tag_to_python(tag):
    """ Convert a stream tag to a Python-readable object """
    newtag = PythonTag()
    newtag.offset = tag.offset
    newtag.key = pmt.to_python(tag.key)
    newtag.value = pmt.to_python(tag.value)
    newtag.srcid = pmt.to_python(tag.srcid)
    return newtag

def tag_to_pmt(tag):
    """ Convert a Python-readable object to a stream tag """
    newtag = gr_tag_t()
    newtag.offset = tag.offset
    newtag.key = pmt.to_python(tag.key)
    newtag.value = pmt.from_python(tag.value)
    newtag.srcid = pmt.from_python(tag.srcid)
    return newtag


