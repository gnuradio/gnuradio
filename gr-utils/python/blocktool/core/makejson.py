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
""" Module to generate JSON file from the parsed data """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import json


def json_generator(parsed_data, header_data):
    """
    Generate JSON file for the block header
    """
    header_file = header_data['filename'].split('.')[0]
    block_module = header_data['modname'].split('-')[-1]
    json_file = os.path.join('.', block_module+'_'+header_file + '.json')
    with open(json_file, 'w') as _file:
        json.dump(parsed_data, _file, indent=4)
    _file.close()
