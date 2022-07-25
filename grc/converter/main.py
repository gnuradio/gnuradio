# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


from codecs import open
import json
import logging
import os

from . import block_tree, block

path = os.path
logger = logging.getLogger(__name__)

excludes = [
    'qtgui_',
    '.grc_gnuradio/',
    'blks2',
    'wxgui',
    'epy_block.xml',
    'virtual_sink.xml',
    'virtual_source.xml',
    'dummy.xml',
    'variable_struct.xml',  # todo: re-implement as class
    'digital_constellation',  # todo: fix template
]


class Converter(object):

    def __init__(self, search_path, output_dir='~/.cache/grc_gnuradio'):
        self.search_path = search_path
        self.output_dir = os.path.expanduser(output_dir)
        logger.info("Saving converted files to {}".format(self.output_dir))

        self._force = False

        converter_module_path = path.dirname(__file__)
        self._converter_mtime = max(path.getmtime(path.join(converter_module_path, module))
                                    for module in os.listdir(converter_module_path)
                                    if not module.endswith('flow_graph.py'))

        self.cache_file = os.path.join(self.output_dir, '_cache.json')
        self.cache = {}

    def run(self, force=False):
        self._force = force

        try:
            logger.debug(
                "Loading block cache from: {}".format(self.cache_file))
            with open(self.cache_file, encoding='utf-8') as cache_file:
                self.cache = byteify(json.load(cache_file))
        except (IOError, ValueError):
            self.cache = {}
            self._force = True
        need_cache_write = False

        if not path.isdir(self.output_dir):
            os.makedirs(self.output_dir)
        if self._force:
            for name in os.listdir(self.output_dir):
                os.remove(os.path.join(self.output_dir, name))

        for xml_file in self.iter_files_in_block_path():
            if xml_file.endswith("block_tree.xml"):
                changed = self.load_category_tree_xml(xml_file)
            elif xml_file.endswith('domain.xml'):
                continue
            else:
                changed = self.load_block_xml(xml_file)

            if changed:
                need_cache_write = True

        if need_cache_write:
            logger.debug('Saving %d entries to json cache', len(self.cache))
            with open(self.cache_file, 'w', encoding='utf-8') as cache_file:
                json.dump(self.cache, cache_file)

    def load_block_xml(self, xml_file, force=False):
        """Load block description from xml file"""
        if any(part in xml_file for part in excludes) and not force:
            logger.warn('Skipping {} because name is blacklisted!'
                        .format(xml_file))
            return False
        elif any(part in xml_file for part in excludes) and force:
            logger.warn('Forcing conversion of blacklisted file: {}'
                        .format(xml_file))

        block_id_from_xml = path.basename(xml_file)[:-4]
        yml_file = path.join(self.output_dir, block_id_from_xml + '.block.yml')

        if not self.needs_conversion(xml_file, yml_file):
            return  # yml file up-to-date

        logger.info('Converting block %s', path.basename(xml_file))
        data = block.from_xml(xml_file)
        if block_id_from_xml != data['id']:
            logger.warning('block_id and filename differ')
        self.cache[yml_file] = data

        with open(yml_file, 'w', encoding='utf-8') as yml_file:
            block.dump(data, yml_file)
        return True

    def load_category_tree_xml(self, xml_file):
        """Validate and parse category tree file and add it to list"""
        module_name = path.basename(
            xml_file)[:-len('block_tree.xml')].rstrip('._-')
        yml_file = path.join(self.output_dir, module_name + '.tree.yml')

        if not self.needs_conversion(xml_file, yml_file):
            return  # yml file up-to-date

        logger.info('Converting module %s', path.basename(xml_file))
        data = block_tree.from_xml(xml_file)
        self.cache[yml_file] = data

        with open(yml_file, 'w', encoding='utf-8') as yml_file:
            block_tree.dump(data, yml_file)
        return True

    def needs_conversion(self, source, destination):
        """Check if source has already been converted and destination is up-to-date"""
        if self._force or not path.exists(destination):
            return True
        xml_time = path.getmtime(source)
        yml_time = path.getmtime(destination)

        return yml_time < xml_time or yml_time < self._converter_mtime

    def iter_files_in_block_path(self, suffix='.xml'):
        """Iterator for block descriptions and category trees"""
        for block_path in self.search_path:
            if path.isfile(block_path):
                yield block_path
            elif path.isdir(block_path):
                for root, _, files in os.walk(block_path, followlinks=True):
                    for name in files:
                        if name.endswith(suffix):
                            yield path.join(root, name)
            else:
                logger.warning(
                    'Invalid entry in search path: {}'.format(block_path))


def byteify(data):
    if isinstance(data, dict):
        return {byteify(key): byteify(value) for key, value in data.items()}
    elif isinstance(data, list):
        return [byteify(element) for element in data]
    else:
        return data
