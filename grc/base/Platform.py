"""
Copyright 2008-2011 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import os
import sys
from .. base import ParseXML, odict
from Element import Element as _Element
from FlowGraph import FlowGraph as _FlowGraph
from Connection import Connection as _Connection
from Block import Block as _Block
from Port import Port as _Port
from Param import Param as _Param
from Constants import BLOCK_TREE_DTD, FLOW_GRAPH_DTD, DOMAIN_DTD


class Platform(_Element):
    def __init__(self, name, version, key,
                 block_paths, block_dtd, default_flow_graph, generator,
                 license='', website=None, colors=None):
        """
        Make a platform from the arguments.

        Args:
            name: the platform name
            version: the version string
            key: the unique platform key
            block_paths: the file paths to blocks in this platform
            block_dtd: the dtd validator for xml block wrappers
            default_flow_graph: the default flow graph file path
            generator: the generator class for this platform
            colors: a list of title, color_spec tuples
            license: a multi-line license (first line is copyright)
            website: the website url for this platform

        Returns:
            a platform object
        """
        _Element.__init__(self)
        self._name = name
        # Save the verion string to the first
        self._version = version[0]
        self._version_major = version[1]
        self._version_api = version[2]
        self._version_minor = version[3]
        self._version_short = version[1] + "." + version[2] + "." + version[3]

        self._key = key
        self._license = license
        self._website = website
        self._block_paths = list(set(block_paths))
        self._block_dtd = block_dtd
        self._default_flow_graph = default_flow_graph
        self._generator = generator
        self._colors = colors or []
        #create a dummy flow graph for the blocks
        self._flow_graph = _Element(self)

        self._blocks = None
        self._blocks_n = None
        self._category_trees_n = None
        self._domains = dict()
        self._connection_templates = dict()
        self.load_blocks()

    def load_blocks(self):
        """load the blocks and block tree from the search paths"""
        # reset
        self._blocks = odict()
        self._blocks_n = odict()
        self._category_trees_n = list()
        self._domains.clear()
        self._connection_templates.clear()
        ParseXML.xml_failures.clear()
        # try to parse and load blocks
        for xml_file in self.iter_xml_files():
            try:
                if xml_file.endswith("block_tree.xml"):
                    self.load_category_tree_xml(xml_file)
                elif xml_file.endswith('domain.xml'):
                    self.load_domain_xml(xml_file)
                else:
                    self.load_block_xml(xml_file)
            except ParseXML.XMLSyntaxError as e:
                # print >> sys.stderr, 'Warning: Block validation failed:\n\t%s\n\tIgnoring: %s' % (e, xml_file)
                pass
            except Exception as e:
                print >> sys.stderr, 'Warning: XML parsing failed:\n\t%s\n\tIgnoring: %s' % (e, xml_file)

    def iter_xml_files(self):
        """Iterator for block descriptions and category trees"""
        get_path = lambda x: os.path.abspath(os.path.expanduser(x))
        for block_path in map(get_path, self._block_paths):
            if os.path.isfile(block_path):
                yield block_path
            elif os.path.isdir(block_path):
                for dirpath, dirnames, filenames in os.walk(block_path):
                    for filename in sorted(filter(lambda f: f.endswith('.xml'), filenames)):
                        yield os.path.join(dirpath, filename)

    def load_block_xml(self, xml_file):
        """Load block description from xml file"""
        # validate and import
        ParseXML.validate_dtd(xml_file, self._block_dtd)
        n = ParseXML.from_file(xml_file).find('block')
        n['block_wrapper_path'] = xml_file  # inject block wrapper path
        # get block instance and add it to the list of blocks
        block = self.Block(self._flow_graph, n)
        key = block.get_key()
        if key in self.get_block_keys():  # test against repeated keys
            print >> sys.stderr, 'Warning: Block with key "%s" already exists.\n\tIgnoring: %s' % (key, xml_file)
        else:  # store the block
            self._blocks[key] = block
            self._blocks_n[key] = n

    def load_category_tree_xml(self, xml_file):
        """Validate and parse category tree file and add it to list"""
        ParseXML.validate_dtd(xml_file, BLOCK_TREE_DTD)
        n = ParseXML.from_file(xml_file).find('cat')
        self._category_trees_n.append(n)

    def load_domain_xml(self, xml_file):
        """Load a domain properties and connection templates from XML"""
        ParseXML.validate_dtd(xml_file, DOMAIN_DTD)
        n = ParseXML.from_file(xml_file).find('domain')

        key = n.find('key')
        if not key:
            print >> sys.stderr, 'Warning: Domain with emtpy key.\n\tIgnoring: %s' % xml_file
            return
        if key in self.get_domains():  # test against repeated keys
            print >> sys.stderr, 'Warning: Domain with key "%s" already exists.\n\tIgnoring: %s' % (key, xml_file)
            return

        to_bool = lambda s, d: d if s is None else \
            s.lower() not in ('false', 'off', '0', '')

        color = n.find('color') or ''
        try:
            import gtk  # ugly but handy
            gtk.gdk.color_parse(color)
        except (ValueError, ImportError):
            if color:  # no color is okay, default set in GUI
                print >> sys.stderr, 'Warning: Can\'t parse color code "%s" for domain "%s" ' % (color, key)
                color = None

        self._domains[key] = dict(
            name=n.find('name') or key,
            multiple_sinks=to_bool(n.find('multiple_sinks'), True),
            multiple_sources=to_bool(n.find('multiple_sources'), False),
            color=color
        )
        for connection_n in n.findall('connection'):
            key = (connection_n.find('source_domain'), connection_n.find('sink_domain'))
            if not all(key):
                print >> sys.stderr, 'Warning: Empty domain key(s) in connection template.\n\t%s' % xml_file
            elif key in self._connection_templates:
                print >> sys.stderr, 'Warning: Connection template "%s" already exists.\n\t%s' % (key, xml_file)
            else:
                self._connection_templates[key] = connection_n.find('make') or ''

    def parse_flow_graph(self, flow_graph_file):
        """
        Parse a saved flow graph file.
        Ensure that the file exists, and passes the dtd check.

        Args:
            flow_graph_file: the flow graph file

        Returns:
            nested data
        @throws exception if the validation fails
        """
        flow_graph_file = flow_graph_file or self._default_flow_graph
        open(flow_graph_file, 'r')  # test open
        ParseXML.validate_dtd(flow_graph_file, FLOW_GRAPH_DTD)
        return ParseXML.from_file(flow_graph_file)

    def load_block_tree(self, block_tree):
        """
        Load a block tree with categories and blocks.
        Step 1: Load all blocks from the xml specification.
        Step 2: Load blocks with builtin category specifications.

        Args:
            block_tree: the block tree object
        """
        #recursive function to load categories and blocks
        def load_category(cat_n, parent=None):
            #add this category
            parent = (parent or []) + [cat_n.find('name')]
            block_tree.add_block(parent)
            #recursive call to load sub categories
            map(lambda c: load_category(c, parent), cat_n.findall('cat'))
            #add blocks in this category
            for block_key in cat_n.findall('block'):
                if block_key not in self.get_block_keys():
                    print >> sys.stderr, 'Warning: Block key "%s" not found when loading category tree.' % (block_key)
                    continue
                block = self.get_block(block_key)
                #if it exists, the block's category shall not be overridden by the xml tree
                if not block.get_category():
                    block.set_category(parent)

        # recursively load the category trees and update the categories for each block
        for category_tree_n in self._category_trees_n:
            load_category(category_tree_n)

        #add blocks to block tree
        for block in self.get_blocks():
            #blocks with empty categories are hidden
            if not block.get_category(): continue
            block_tree.add_block(block.get_category(), block)

    def __str__(self): return 'Platform - %s(%s)'%(self.get_key(), self.get_name())

    def is_platform(self): return True

    def get_new_flow_graph(self): return self.FlowGraph(platform=self)

    def get_generator(self): return self._generator

    ##############################################
    # Access Blocks
    ##############################################
    def get_block_keys(self): return self._blocks.keys()
    def get_block(self, key): return self._blocks[key]
    def get_blocks(self): return self._blocks.values()
    def get_new_block(self, flow_graph, key): return self.Block(flow_graph, n=self._blocks_n[key])

    def get_domains(self): return self._domains
    def get_domain(self, key): return self._domains.get(key)
    def get_connection_templates(self): return self._connection_templates

    def get_name(self): return self._name
    def get_version(self): return self._version
    def get_version_major(self): return self._version_major
    def get_version_api(self): return self._version_api
    def get_version_minor(self): return self._version_minor
    def get_version_short(self): return self._version_short

    def get_key(self): return self._key
    def get_license(self): return self._license
    def get_website(self): return self._website
    def get_colors(self): return self._colors
    def get_block_paths(self): return self._block_paths

    ##############################################
    # Constructors
    ##############################################
    FlowGraph = _FlowGraph
    Connection = _Connection
    Block = _Block
    Port = _Port
    Param = _Param
