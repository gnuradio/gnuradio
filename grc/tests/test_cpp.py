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

from argparse import Namespace
from os import path
import tempfile
import subprocess

from .test_compiler import *

def test_cpp():
    test_compiler('test_cpp.grc')
    if not os.path.exists('test_cpp/build'):
        os.makedirs('directory')
    return_code = subprocess.Popen('cmake ..', cwd='./build/')
    assert(return_code == 0)
    return_code = subprocess.Popen('make', cwd='./build/')
    assert(return_code == 0)

