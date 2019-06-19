#
# Copyright 2019 Free Software Foundation, Inc.
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
""" Module to get io_signature of the header block """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import itertools
import logging
import string

from blocktool.core import Constants

LOGGER = logging.getLogger(__name__)


def io_signature(io_file):
    """
    function to generate the io_signature of the block
    : returns the io parmaters
    """
    parsed_io = {
        "input": {
            "signature": None
        },
        "output": {
            "signature": None
        }
    }
    with open(io_file, 'r') as _impl:
        lines = []
        for line in _impl:
            if Constants.IO_SIGNATURE in line:
                lines.append(line)
    _impl.close()
    if len(lines) > 2:
        lines = lines[0:2]
    _io_sig = []
    for line in lines:
        if Constants.IO_SIGNATURE in line:
            line = line.lstrip().rstrip().split(Constants.IO_SIGNATURE)
            _io_sig.append(line)
    _io_sig = list(itertools.chain.from_iterable(_io_sig))
    for index, _element in enumerate(_io_sig):
        _io_sig[index] = _element.lstrip().rstrip()
        if all(i in string.punctuation for i in _element):
            _io_sig.remove(_element)
    _io_sig = list(filter(None, _io_sig))
    io_func = []
    for _io in _io_sig:
        io_func.append(_io.lstrip().rstrip(' ,:)'))
    for _signature in Constants.SIGNATURE_LIST:
        if _signature in _io_sig[0] and parsed_io['input']['signature'] is None:
            parsed_io['input']['signature'] = _signature
            io_func[0] = io_func[0].lstrip(_signature+' (')
        if _signature in _io_sig[1] and parsed_io['output']['signature'] is None:
            parsed_io['output']['signature'] = _signature
            io_func[1] = io_func[1].lstrip(_signature+' (')
    io_elements = []
    for _io in io_func:
        _io = _io.split(',')
        io_elements.append(_io)
    io_elements = list(itertools.chain.from_iterable(io_elements))
    for index, _io in enumerate(io_elements):
        _io = _io.lstrip(' (').rstrip(' )')
        if '(' in _io:
            _io = _io + ')'
        io_elements[index] = _io
    if parsed_io['input']['signature'] is Constants.MAKE:
        parsed_io['input']['min_streams'] = io_elements[0]
        parsed_io['input']['max_streams'] = io_elements[1]
        parsed_io['input']['sizeof_stream_item'] = io_elements[2]
        del io_elements[0:3]
    elif parsed_io['input']['signature'] is Constants.MAKE2:
        parsed_io['input']['min_streams'] = io_elements[0]
        parsed_io['input']['max_streams'] = io_elements[1]
        parsed_io['input']['sizeof_stream_item1'] = io_elements[2]
        parsed_io['input']['sizeof_stream_item2'] = io_elements[3]
        del io_elements[0:4]
    elif parsed_io['input']['signature'] is Constants.MAKE3:
        parsed_io['input']['min_streams'] = io_elements[0]
        parsed_io['input']['max_streams'] = io_elements[1]
        parsed_io['input']['sizeof_stream_item1'] = io_elements[2]
        parsed_io['input']['sizeof_stream_item2'] = io_elements[3]
        parsed_io['input']['sizeof_stream_item3'] = io_elements[4]
        del io_elements[0:5]
    elif parsed_io['input']['signature'] is Constants.MAKEV:
        parsed_io['input']['min_streams'] = io_elements[0]
        parsed_io['input']['max_streams'] = io_elements[1]
        parsed_io['input']['sizeof_stream_items'] = io_elements[2]
        del io_elements[0:3]

    if parsed_io['output']['signature'] is Constants.MAKE:
        parsed_io['output']['min_streams'] = io_elements[0]
        parsed_io['output']['max_streams'] = io_elements[1]
        parsed_io['output']['sizeof_stream_item'] = io_elements[2]
        del io_elements[0:3]
    elif parsed_io['output']['signature'] is Constants.MAKE2:
        parsed_io['output']['min_streams'] = io_elements[0]
        parsed_io['output']['max_streams'] = io_elements[1]
        parsed_io['output']['sizeof_stream_item1'] = io_elements[2]
        parsed_io['output']['sizeof_stream_item2'] = io_elements[3]
        del io_elements[0:4]
    elif parsed_io['output']['signature'] is Constants.MAKE3:
        parsed_io['output']['min_streams'] = io_elements[0]
        parsed_io['output']['max_streams'] = io_elements[1]
        parsed_io['output']['sizeof_stream_item1'] = io_elements[2]
        parsed_io['output']['sizeof_stream_item2'] = io_elements[3]
        parsed_io['output']['sizeof_stream_item3'] = io_elements[4]
        del io_elements[0:5]
    elif parsed_io['output']['signature'] is Constants.MAKEV:
        parsed_io['output']['min_streams'] = io_elements[0]
        parsed_io['output']['max_streams'] = io_elements[1]
        parsed_io['output']['sizeof_stream_items'] = io_elements[2]
        del io_elements[0:3]

    return parsed_io
