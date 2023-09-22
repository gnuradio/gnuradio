#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to read and add special blocktool comments in the public header """


import warnings

from ..core import Constants


def strip_symbols(line):
    """
    helper function to strip symbols
    from blocktool comment syntax
    """
    return line.split(':')[-1].lstrip().rstrip()


def exist_comments(self):
    """
    function to check if blocktool special comments
    already exist in the public header
    """
    _comments = True
    _index = None
    lines = []
    with open(self.target_file, 'r') as header:
        lines = header.readlines()
        for line in lines:
            if Constants.BLOCKTOOL in line:
                _index = lines.index(line)
    return bool(_index)


def validate_message_port(self, message_ports, suppress_input, suppress_output):
    """
    function to solve conflicts if any in the
    *message_port* comments and the implementation information
    """
    if message_ports['input'] != self.parsed_data['message_port']['input']:
        if not suppress_input:
            warnings.warn(
                'Conflict in values input message port Id. Add ! at the start of the key-value line to mandatory use the comment value.')
            self.parsed_data['message_port']['input'] = message_ports['input']
    if message_ports['output'] != self.parsed_data['message_port']['output']:
        if not suppress_output:
            warnings.warn(
                'Conflict in values output message port Id. Add ! at the start of the key-value line to mandatory use the comment value.')
            self.parsed_data['message_port']['output'] = message_ports['output']


def read_comments(self):
    """
    function to read special blocktool comments
    in the public header
    """
    temp_parsed_data = {}
    if self.parsed_data['io_signature'] or self.parsed_data['message_port']:
        temp_parsed_data['io_signature'] = self.parsed_data['io_signature']
        temp_parsed_data['message_port'] = self.parsed_data['message_port']
    self.parsed_data['io_signature'] = {
        "input": {
            "signature": None
        },
        "output": {
            "signature": None
        }
    }
    self.parsed_data['message_port'] = {
        "input": [],
        "output": []
    }
    _suppress_input = False
    _suppress_output = False
    parsed_io = self.parsed_data['io_signature']
    message_port = self.parsed_data['message_port']
    special_comments = []
    _index = None
    lines = []
    with open(self.target_file, 'r') as header:
        lines = header.readlines()
        for line in lines:
            if Constants.BLOCKTOOL in line:
                _index = lines.index(line)

    if _index is not None:
        _index = _index + 1
        for num in range(_index, len(lines)):
            if Constants.END_BLOCKTOOL in lines[num]:
                break
            special_comments.append(lines[num])
        for comment in special_comments:
            if Constants.INPUT_SIG in comment:
                parsed_io['input']['signature'] = strip_symbols(comment)
            if Constants.INPUT_MAX in comment:
                parsed_io['input']['max_streams'] = strip_symbols(comment)
            if Constants.INPUT_MIN in comment:
                parsed_io['input']['min_streams'] = strip_symbols(comment)
            if parsed_io['input']['signature'] is Constants.MAKE and not None:
                if Constants.INPUT_MAKE_SIZE in comment:
                    parsed_io['input']['sizeof_stream_item'] = strip_symbols(
                        comment)
            elif parsed_io['input']['signature'] is Constants.MAKE2 and not None:
                if Constants.INPUT_MAKE_SIZE1 in comment:
                    parsed_io['input']['sizeof_stream_item1'] = strip_symbols(
                        comment)
                if Constants.INPUT_MAKE_SIZE2 in comment:
                    parsed_io['input']['sizeof_stream_item2'] = strip_symbols(
                        comment)
            elif parsed_io['input']['signature'] is Constants.MAKE3 and not None:
                if Constants.INPUT_MAKE_SIZE1 in comment:
                    parsed_io['input']['sizeof_stream_item1'] = strip_symbols(
                        comment)
                if Constants.INPUT_MAKE_SIZE2 in comment:
                    parsed_io['input']['sizeof_stream_item2'] = strip_symbols(
                        comment)
                if Constants.INPUT_MAKE_SIZE3 in comment:
                    parsed_io['input']['sizeof_stream_item3'] = strip_symbols(
                        comment)
            elif parsed_io['input']['signature'] is Constants.MAKEV and not None:
                if Constants.INPUT_MAKEV_SIZE in comment:
                    parsed_io['input']['sizeof_stream_items'] = strip_symbols(
                        comment)

            if Constants.OUTPUT_SIG in comment:
                parsed_io['output']['signature'] = strip_symbols(comment)
            if Constants.OUTPUT_MAX in comment:
                parsed_io['output']['max_streams'] = strip_symbols(comment)
            if Constants.OUTPUT_MIN in comment:
                parsed_io['output']['min_streams'] = strip_symbols(comment)
            if parsed_io['output']['signature'] is Constants.MAKE and not None:
                if Constants.OUTPUT_MAKE_SIZE in comment:
                    parsed_io['output']['sizeof_stream_item'] = strip_symbols(
                        comment)
            elif parsed_io['output']['signature'] is Constants.MAKE2:
                if Constants.OUTPUT_MAKE_SIZE1 in comment:
                    parsed_io['output']['sizeof_stream_item1'] = strip_symbols(
                        comment)
                if Constants.OUTPUT_MAKE_SIZE2 in comment:
                    parsed_io['output']['sizeof_stream_item2'] = strip_symbols(
                        comment)
            elif parsed_io['output']['signature'] is Constants.MAKE3 and not None:
                if Constants.OUTPUT_MAKE_SIZE1 in comment:
                    parsed_io['output']['sizeof_stream_item1'] = strip_symbols(
                        comment)
                if Constants.OUTPUT_MAKE_SIZE2 in comment:
                    parsed_io['output']['sizeof_stream_item2'] = strip_symbols(
                        comment)
                if Constants.OUTPUT_MAKE_SIZE3 in comment:
                    parsed_io['output']['sizeof_stream_item3'] = strip_symbols(
                        comment)
            elif parsed_io['output']['signature'] is Constants.MAKEV and not None:
                if Constants.OUTPUT_MAKEV_SIZE in comment:
                    parsed_io['output']['sizeof_stream_items'] = strip_symbols(
                        comment)

            if Constants.INPUT_PORT in comment:
                if Constants.EXCLAMATION in comment:
                    _suppress_input = True
                if strip_symbols(comment):
                    message_port['input'] = strip_symbols(comment).split(', ')
            if Constants.OUTPUT_PORT in comment:
                if Constants.EXCLAMATION in comment:
                    _suppress_output = True
                if strip_symbols(comment):
                    message_port['output'] = strip_symbols(comment).split(', ')
    validate_message_port(
        self, temp_parsed_data['message_port'], _suppress_input, _suppress_output)
    self.parsed_data['io_signature'] = temp_parsed_data['io_signature']


def add_comments(self):
    """
    function to add special blocktool comments
    in the public header
    """
    _index = None
    lines = []
    parsed_io = self.parsed_data['io_signature']
    message_port = self.parsed_data['message_port']
    with open(self.target_file, 'r') as header:
        lines = header.readlines()
        for line in lines:
            if Constants.BLOCKTOOL in line:
                _index = lines.index(line)
    if _index is None:
        with open(self.target_file, 'a') as header:
            header.write('\n')
            header.write('/* ' + Constants.BLOCKTOOL + '\n')
            header.write('input_signature: ' +
                         parsed_io['input']['signature'] + '\n')
            header.write('input_min_streams: ' +
                         parsed_io['input']['min_streams'] + '\n')
            header.write('input_max_streams: ' +
                         parsed_io['input']['max_streams'] + '\n')
            if parsed_io['input']['signature'] is Constants.MAKE:
                header.write('input_sizeof_stream_item: ' +
                             parsed_io['input']['sizeof_stream_item'] + '\n')
            elif parsed_io['input']['signature'] is Constants.MAKE2:
                header.write('input_sizeof_stream_item1: ' +
                             parsed_io['input']['sizeof_stream_item1'] + '\n')
                header.write('input_sizeof_stream_item2: ' +
                             parsed_io['input']['sizeof_stream_item2'] + '\n')
            elif parsed_io['input']['signature'] is Constants.MAKE3:
                header.write('input_sizeof_stream_item1: ' +
                             parsed_io['input']['sizeof_stream_item1'] + '\n')
                header.write('input_sizeof_stream_item2: ' +
                             parsed_io['input']['sizeof_stream_item2'] + '\n')
                header.write('input_sizeof_stream_item3: ' +
                             parsed_io['input']['sizeof_stream_item3'] + '\n')
            elif parsed_io['input']['signature'] is Constants.MAKEV:
                header.write('input_sizeof_stream_item: ' +
                             parsed_io['input']['sizeof_stream_items'] + '\n')
            header.write('output_signature: ' +
                         parsed_io['output']['signature'] + '\n')
            header.write('output_min_streams: ' +
                         parsed_io['output']['min_streams'] + '\n')
            header.write('output_max_streams: ' +
                         parsed_io['output']['max_streams'] + '\n')
            if parsed_io['output']['signature'] is Constants.MAKE:
                header.write('output_sizeof_stream_item: ' +
                             parsed_io['output']['sizeof_stream_item'] + '\n')
            elif parsed_io['output']['signature'] is Constants.MAKE2:
                header.write('output_sizeof_stream_item1: ' +
                             parsed_io['output']['sizeof_stream_item1'] + '\n')
                header.write('output_sizeof_stream_item2: ' +
                             parsed_io['output']['sizeof_stream_item2'] + '\n')
            elif parsed_io['output']['signature'] is Constants.MAKE3:
                header.write('output_sizeof_stream_item1: ' +
                             parsed_io['output']['sizeof_stream_item1'] + '\n')
                header.write('output_sizeof_stream_item2: ' +
                             parsed_io['output']['sizeof_stream_item2'] + '\n')
                header.write('output_sizeof_stream_item3: ' +
                             parsed_io['output']['sizeof_stream_item3'] + '\n')
            elif parsed_io['output']['signature'] is Constants.MAKEV:
                header.write('output_sizeof_stream_item: ' +
                             parsed_io['output']['sizeof_stream_items'] + '\n')

            if message_port['input']:
                header.write('message_input: ' +
                             ', '.join(message_port['input']) + '\n')
            else:
                header.write('message_input: ' + '\n')
            if message_port['output']:
                header.write('message_output: ' +
                             ', '.join(message_port['output']) + '\n')
            else:
                header.write('message_output: ' + '\n')
            header.write(Constants.END_BLOCKTOOL + '*/' + '\n')
