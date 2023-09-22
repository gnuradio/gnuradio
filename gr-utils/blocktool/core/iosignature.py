#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to get io_signature of the header block """


import re
import itertools
import logging
import string

from ..core import Constants

LOGGER = logging.getLogger(__name__)


def io_signature(impl_file):
    """
    function to generate the io_signature of the block
    : returns the io parameters
    """
    parsed_io = {
        "input": {
            "signature": None
        },
        "output": {
            "signature": None
        }
    }
    with open(impl_file, 'r') as impl:
        io_lines = []
        for line in impl:
            if Constants.IO_SIGNATURE in line:
                io_lines.append(line)
    if len(io_lines) > 2:
        io_lines = io_lines[0:2]
    _io_sig = []
    for line in io_lines:
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
        if Constants.MAKE in _io:
            io_func.append(_io.lstrip().rstrip(Constants.STRIP_SYMBOLS))
    for signature in Constants.SIGNATURE_LIST:
        if signature in io_func[0] and parsed_io['input']['signature'] is None:
            parsed_io['input']['signature'] = signature
            io_func[0] = io_func[0].lstrip(signature + ' (')
        if signature in io_func[1] and parsed_io['output']['signature'] is None:
            parsed_io['output']['signature'] = signature
            io_func[1] = io_func[1].lstrip(signature + ' (')
    io_elements = []
    for _io in io_func:
        _io = _io.split(',')
        io_elements.append(_io)
    io_elements = list(itertools.chain.from_iterable(io_elements))
    for index, _io in enumerate(io_elements):
        _io = _io.lstrip(' (').rstrip(' )')
        if Constants.OPEN_BRACKET in _io:
            _io = _io + Constants.CLOSE_BRACKET
        io_elements[index] = _io

    # Because of any possible combination of I/O signature and different number
    # of arguments, manual if else loop is required
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


def message_port(impl_file):
    """
    parses message ports from implementation file
    """
    parsed_message_port = {
        "input": [],
        "output": []
    }
    with open(impl_file, 'r') as impl:
        _input = []
        _output = []
        for line in impl:
            if Constants.MESSAGE_INPUT in line:
                _input.append(line)
            if Constants.MESSAGE_OUTPUT in line:
                _output.append(line)

    input_port = []
    output_port = []
    if _input:
        for port in _input:
            port = port.lstrip().rstrip().strip(Constants.MESSAGE_INPUT)
            pattern = port.find('\"')
            if pattern != -1:
                if re.findall(r'"([^"]*)"', port)[0]:
                    input_port.append(re.findall(r'"([^"]*)"', port)[0])
            else:
                input_port.append(port[port.find('(') + 1:port.rfind(')')])
                _temp_port = ''.join(map(str, input_port))
                input_port.clear()
                input_port.append(_temp_port)

    if _output:
        for port in _output:
            port = port.lstrip().rstrip().strip(Constants.MESSAGE_OUTPUT)
            pattern = port.find('\"')
            if pattern != -1:
                if re.findall(r'"([^"]*)"', port)[0]:
                    output_port.append(re.findall(r'"([^"]*)"', port)[0])
            else:
                output_port.append(port[port.find('(') + 1:port.rfind(')')])
                _temp_port = ''.join(map(str, output_port))
                output_port.clear()
                output_port.append(_temp_port)

    if input_port:
        for port in input_port:
            parsed_message_port['input'].append(port)

    if output_port:
        for port in output_port:
            parsed_message_port['output'].append(port)
    return parsed_message_port
