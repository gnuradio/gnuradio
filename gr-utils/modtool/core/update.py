#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to convert XML bindings to YAML bindings """


import os
import re
import glob
import logging

from .base import ModTool, ModToolException

logger = logging.getLogger(__name__)


class ModToolUpdate(ModTool):
    """ Update the grc bindings for a block """
    name = 'update'
    description = 'Update the grc bindings for a block'

    def __init__(self, blockname=None, complete=False, include_blacklisted=False, **kwargs):
        ModTool.__init__(self, blockname, **kwargs)
        self.info['complete'] = complete
        self.info['include_blacklisted'] = include_blacklisted

    def validate(self):
        """ Validates the arguments """
        ModTool._validate(self)
        if self.info['complete']:
            return
        if not self.info['blockname'] or self.info['blockname'].isspace():
            raise ModToolException('Block name not specified!')
        block_candidates = self.get_xml_candidates()
        if self.info['blockname'] not in block_candidates:
            choices = [
                x for x in block_candidates if self.info['blockname'] in x]
            if len(choices) > 0:
                print("Suggested alternatives: " + str(choices))
            raise ModToolException("The XML bindings does not exists!")

    def get_xml_candidates(self):
        return [
            os.path.splitext(candidate)[0].removeprefix(f"{self.info["modname"]}_")
            for candidate in glob.glob1("grc", "*.xml")
        ]

    def run(self):
        from gnuradio.grc.converter import Converter
        if not self.cli:
            self.validate()
        logger.warning(
            "Warning: This is an experimental feature. Please verify the bindings.")
        module_name = self.info['modname']
        path = './grc/'
        conv = Converter(path, path)
        if self.info['complete']:
            # Get a list of XML block candidates for update
            blocks = self.get_xml_candidates()
        else:
            blocks = [self.info['blockname']]
        for blockname in blocks:
            xml_file = f"{module_name}_{blockname}.xml"
            yml_file = f"{module_name}_{blockname}.block.yml"
            if not conv.load_block_xml(path + xml_file, self.info["include_blacklisted"]):
                continue
            logger.info(f"Converted {xml_file} to {yml_file}")
            os.remove(path + xml_file)
            nsubs = self._run_cmakelists(xml_file, yml_file)
            if nsubs > 1:
                logger.warning("Changed more than expected for the block '%s' in the CMakeLists.txt. "
                               "Please verify the CMakeLists manually.", blockname)
            elif nsubs == 0:
                logger.warning("No entry found for the block '%s' in the CMakeLists.txt. "
                               'Please verify the CMakeLists manually.', blockname)
            else:
                logger.info('Updated the CMakeLists.txt')

    def _run_cmakelists(self, to_remove, to_add):
        """ Changes in the CMakeLists """
        filename = './grc/CMakeLists.txt'
        with open(filename) as f:
            cfile = f.read()
        (cfile, nsubs) = re.subn(to_remove, to_add, cfile)
        with open(filename, 'w') as f:
            f.write(cfile)
        self.scm.mark_file_updated(filename)
        return nsubs
