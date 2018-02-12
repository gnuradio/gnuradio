#
# Copyright 2003-2012, 2018 Free Software Foundation, Inc.
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

# The presence of this file turns this directory into a Python package

"""
Core contents.
"""

# This is the main GNU Radio python module.
# We pull the swig output and the other modules into the gnuradio.gr namespace

# If gnuradio is installed then the swig output will be in this directory.
# Otherwise it will reside in ../../../swig.

import os

try:
    from runtime_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "..", "swig"))
    from runtime_swig import *

from exceptions import *
from top_block import *
from hier_block2 import *
from tag_utils import *
from gateway import basic_block, sync_block, decim_block, interp_block, py_io_signature

# Force the preference database to be initialized
prefs = prefs.singleton

log = gr.logger("log")
log.set_level(prefs().get_string("LOG", "log_level", "notset"))
log_file = gr.prefs().get_string("LOG", "log_file", "");
if(log_file == "stderr" or log_file == "stdout"):
    log.add_console_appender(log_file, "gr::log %d :%p: %m%n")
else:
    log.add_file_appender(log_file, True, "%r :%p: %c{1} - %m%n")

log_debug = gr.logger("log_debug")
log_debug.set_level(prefs().get_string("LOG", "debug_level", "notset"))
log_debug_file = gr.prefs().get_string("LOG", "debug_file", "");
if(log_debug_file == "stderr" or log_file == "stdout"):
    log_debug.add_console_appender(log_debug_file, "gr::log %d :%p: %m%n")
else:
    log_debug.add_file_appender(log_debug_file, True, "%r :%p: %c{1} - %m%n")
