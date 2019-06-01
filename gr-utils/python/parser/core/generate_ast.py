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
""" Module to generate AST for the headers """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import logging

from pygccxml import parser
from pygccxml import declarations
from pygccxml import utils

from parser.core import ParserException, Parser

logger = logging.getLogger(__name__)


class ParserGenerateAst(Parser):
    """ Select a GNU Radio module. """
    name = 'module'
    description = 'Select a GNU Radio module'
    module_types = ('gr-analog', 'gr-audio', 'gr-blocks', 'gr-channels', 'gr-comedi',
                'gr-digital', 'gr-dtv', 'gr-fec', 'gr-fft', 'gr-filter', 'gr-qtgui',
                'gr-trellis', 'gr-uhd', 'gr-video-sdl', 'gr-vocoder', 'gr-wavelet',
                'gr-zeromq')

    def __init__(self, header_name=None, file_name=None,
                **kwargs):
        Parser.__init__(self, file_name, **kwargs)
        self.info['filename'] = file_name
        pass

    def validate(self):
        """ Validates the arguments """
        Parser._validate(self)
        pass

    def ast_generator(self):
        """
        Abstract Syntax Tree text file generator
        magic happens here!
        """
        generator_path, generator_name = utils.find_xml_generator()
        xml_generator_config = parser.xml_generator_configuration_t(
            xml_generator_path=generator_path,
            xml_generator=generator_name,
            compiler="gcc")

        gr = self.info['module_name'].split("-")[0]
        module = self.info['module_name'].split("-")[-1]
        decls = parser.parse([self.info['filename']], xml_generator_config)
        global_namespace = declarations.get_global_namespace(decls)
        ns = global_namespace.namespace(gr)
        main_namespace = ns.namespace(module)
        return declarations.print_declarations(main_namespace)

    def run(self):
        """ Go, go, go. """
        pass
        # self.ast_generator()
