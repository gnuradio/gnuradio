# Copyright 2016 Free Software Foundation, Inc.
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

from __future__ import print_function, absolute_import

import argparse
import os
import subprocess

from gnuradio import gr

from .core import Messages
from .core.Platform import Platform


def argument_parser():
    parser = argparse.ArgumentParser(description=(
        "Compiles a GRC file (.grc) into a GNU Radio Python program. "
        "The program is stored in ~/.grc_gnuradio by default, "
        "but this location can be changed with the -d option."
    ))
    parser.add_argument("-d", "--directory", dest='out_dir', default='',
                        help="Specify the directory to output the compiled program "
                             "(default is the hier_block library)")
    parser.add_argument("-e", "--execute", action="store_true", default=False,
                        help="Run the program after compiling [default=%(default)s]")
    parser.add_argument(metavar="GRC_FILE", dest='grc_files', nargs='+',
                        help=".grc file to compile")
    return parser


def main(args=None):
    args = args or argument_parser().parse_args()

    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs_file=gr.prefs(),
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(), gr.minor_version())
    )
    out_dir = args.out_dir or platform.config.hier_block_lib_dir
    if not os.path.exists(out_dir):
        exit('Error: Invalid output directory.')

    Messages.send_init(platform)
    flow_graph = file_path = None
    for grc_file in args.grc_files:
        os.path.exists(grc_file) or exit('Error: missing ' + grc_file)
        Messages.send('\n')

        flow_graph, file_path = platform.load_and_generate_flow_graph(
            os.path.abspath(grc_file), os.path.abspath(out_dir))
        if not file_path:
            exit('Compilation error. Aborting.')
    if file_path and args.execute:
        run_command_args = flow_graph.get_run_command(file_path, split=True)
        subprocess.call(run_command_args)
