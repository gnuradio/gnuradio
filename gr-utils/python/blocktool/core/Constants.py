#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" constants file """

# Kernel Namespace
KERNEL = 'kernel'

# I/O Signature (Symbols and constants)
IO_SIGNATURE = 'io_signature::'
SIGNATURE_LIST = ['makev', 'make3', 'make2', 'make']
MAKE = 'make'
MAKE2 = 'make2'
MAKE3 = 'make3'
MAKEV = 'makev'


# message ports id
MESSAGE_INPUT = 'message_port_register_in'
MESSAGE_OUTPUT = 'message_port_register_out'

# Symbols and constants required for parsing
GR = 'gr-'
UTILS = 'utils'
OPEN_BRACKET = '('
CLOSE_BRACKET = ')'
STRIP_SYMBOLS = ' ,:)'
EXCLAMATION = '!'

# Blocktool special comments
BLOCKTOOL = '! BlockTool'
END_BLOCKTOOL = 'EndTool !'
INPUT_SIG = 'input_signature'
OUTPUT_SIG = 'output_signature'
INPUT_MIN = 'input_min_streams'
INPUT_MAX = 'input_max_streams'
OUTPUT_MIN = 'output_min_streams'
OUTPUT_MAX = 'output_max_streams'
INPUT_MAKE_SIZE = 'input_sizeof_stream_item'
INPUT_MAKEV_SIZE = 'input_sizeof_stream_items'
INPUT_MAKE_SIZE1 = 'input_sizeof_stream_item1'
INPUT_MAKE_SIZE2 = 'input_sizeof_stream_item2'
INPUT_MAKE_SIZE3 = 'input_sizeof_stream_item3'
OUTPUT_MAKE_SIZE = 'output_sizeof_stream_item'
OUTPUT_MAKEV_SIZE = 'output_sizeof_stream_items'
OUTPUT_MAKE_SIZE1 = 'output_sizeof_stream_item1'
OUTPUT_MAKE_SIZE2 = 'output_sizeof_stream_item2'
OUTPUT_MAKE_SIZE3 = 'output_sizeof_stream_item3'
INPUT_PORT = 'message_input'
OUTPUT_PORT = 'message_output'
