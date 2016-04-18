"""
Copyright 2008-2016 Free Software Foundation, Inc.
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

from . import ParseXML, Messages, Constants

from .Config import Config
from .Element import Element
from .generator import Generator
from .FlowGraph import FlowGraph
from .Connection import Connection
from .Block import Block
from .Port import Port
from .Param import Param

from .utils import odict, extract_docs


class Platform(Element):

    Config = Config
    Generator = Generator
    FlowGraph = FlowGraph
    Connection = Connection
    Block = Block
    Port = Port
    Param = Param

    is_platform = True

    def __init__(self, *args, **kwargs):
        """ Make a platform for GNU Radio """
        Element.__init__(self)

        self.config = self.Config(*args, **kwargs)

        self.block_docstrings = {}
        self.block_docstrings_loaded_callback = lambda: None  # dummy to be replaced by BlockTreeWindow

        self._docstring_extractor = extract_docs.SubprocessLoader(
            callback_query_result=self._save_docstring_extraction_result,
            callback_finished=lambda: self.block_docstrings_loaded_callback()
        )

        self._block_dtd = Constants.BLOCK_DTD
        self._default_flow_graph = Constants.DEFAULT_FLOW_GRAPH

        # Create a dummy flow graph for the blocks
        self._flow_graph = Element(self)
        self._flow_graph.connections = []

        self.blocks = None
        self._blocks_n = None
        self._category_trees_n = None
        self.domains = {}
        self.connection_templates = {}

        self._auto_hier_block_generate_chain = set()

        self.load_blocks()

    def __str__(self):
        return 'Platform - {}({})'.format(self.config.key, self.config.name)

    @staticmethod
    def find_file_in_paths(filename, paths, cwd):
        """Checks the provided paths relative to cwd for a certain filename"""
        if not os.path.isdir(cwd):
            cwd = os.path.dirname(cwd)
        if isinstance(paths, str):
            paths = (p for p in paths.split(':') if p)

        for path in paths:
            path = os.path.expanduser(path)
            if not os.path.isabs(path):
                path = os.path.normpath(os.path.join(cwd, path))
            file_path = os.path.join(path, filename)
            if os.path.exists(os.path.normpath(file_path)):
                return file_path

    def load_and_generate_flow_graph(self, file_path):
        """Loads a flow graph from file and generates it"""
        Messages.set_indent(len(self._auto_hier_block_generate_chain))
        Messages.send('>>> Loading: %r\n' % file_path)
        if file_path in self._auto_hier_block_generate_chain:
            Messages.send('    >>> Warning: cyclic hier_block dependency\n')
            return False
        self._auto_hier_block_generate_chain.add(file_path)
        try:
            flow_graph = self.get_new_flow_graph()
            flow_graph.grc_file_path = file_path
            # Other, nested higiter_blocks might be auto-loaded here
            flow_graph.import_data(self.parse_flow_graph(file_path))
            flow_graph.rewrite()
            flow_graph.validate()
            if not flow_graph.is_valid():
                raise Exception('Flowgraph invalid')
            if not flow_graph.get_option('generate_options').startswith('hb'):
                raise Exception('Not a hier block')
        except Exception as e:
            Messages.send('>>> Load Error: {}: {}\n'.format(file_path, str(e)))
            return False
        finally:
            self._auto_hier_block_generate_chain.discard(file_path)
            Messages.set_indent(len(self._auto_hier_block_generate_chain))

        try:
            Messages.send('>>> Generating: {}\n'.format(file_path))
            generator = self.Generator(flow_graph, file_path)
            generator.write()
        except Exception as e:
            Messages.send('>>> Generate Error: {}: {}\n'.format(file_path, str(e)))
            return False

        self.load_block_xml(generator.get_file_path_xml())
        return True

    def load_blocks(self):
        """load the blocks and block tree from the search paths"""
        self._docstring_extractor.start()
        # Reset
        self.blocks = odict()
        self._blocks_n = odict()
        self._category_trees_n = list()
        self.domains.clear()
        self.connection_templates.clear()
        ParseXML.xml_failures.clear()
        # Try to parse and load blocks
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
                print >> sys.stderr, 'Warning: XML parsing failed:\n\t%r\n\tIgnoring: %s' % (e, xml_file)

        self._docstring_extractor.finish()
        # self._docstring_extractor.wait()

    def iter_xml_files(self):
        """Iterator for block descriptions and category trees"""
        for block_path in self.config.block_paths:
            if os.path.isfile(block_path):
                yield block_path
            elif os.path.isdir(block_path):
                for dirpath, dirnames, filenames in os.walk(block_path):
                    for filename in sorted(filter(lambda f: f.endswith('.xml'), filenames)):
                        yield os.path.join(dirpath, filename)

    def load_block_xml(self, xml_file):
        """Load block description from xml file"""
        # Validate and import
        ParseXML.validate_dtd(xml_file, self._block_dtd)
        n = ParseXML.from_file(xml_file).find('block')
        n['block_wrapper_path'] = xml_file  # inject block wrapper path
        # Get block instance and add it to the list of blocks
        block = self.Block(self._flow_graph, n)
        key = block.get_key()
        if key in self.blocks:
            print >> sys.stderr, 'Warning: Block with key "{}" already exists.\n\tIgnoring: {}'.format(key, xml_file)
        else:  # Store the block
            self.blocks[key] = block
            self._blocks_n[key] = n

        self._docstring_extractor.query(
            block.get_key(),
            block.get_imports(raw=True),
            block.get_make(raw=True)
        )

    def load_category_tree_xml(self, xml_file):
        """Validate and parse category tree file and add it to list"""
        ParseXML.validate_dtd(xml_file, Constants.BLOCK_TREE_DTD)
        n = ParseXML.from_file(xml_file).find('cat')
        self._category_trees_n.append(n)

    def load_domain_xml(self, xml_file):
        """Load a domain properties and connection templates from XML"""
        ParseXML.validate_dtd(xml_file, Constants.DOMAIN_DTD)
        n = ParseXML.from_file(xml_file).find('domain')

        key = n.find('key')
        if not key:
            print >> sys.stderr, 'Warning: Domain with emtpy key.\n\tIgnoring: {}'.format(xml_file)
            return
        if key in self.domains:  # test against repeated keys
            print >> sys.stderr, 'Warning: Domain with key "{}" already exists.\n\tIgnoring: {}'.format(key, xml_file)
            return

        #to_bool = lambda s, d: d if s is None else s.lower() not in ('false', 'off', '0', '')
        def to_bool(s, d):
            if s is not None:
                return s.lower() not in ('false', 'off', '0', '')
            return d

        color = n.find('color') or ''
        try:
            chars_per_color = 2 if len(color) > 4 else 1
            tuple(int(color[o:o + 2], 16) / 255.0 for o in range(1, 3 * chars_per_color, chars_per_color))
        except ValueError:
            if color:  # no color is okay, default set in GUI
                print >> sys.stderr, 'Warning: Can\'t parse color code "{}" for domain "{}" '.format(color, key)
                color = None

        self.domains[key] = dict(
            name=n.find('name') or key,
            multiple_sinks=to_bool(n.find('multiple_sinks'), True),
            multiple_sources=to_bool(n.find('multiple_sources'), False),
            color=color
        )
        for connection_n in n.findall('connection'):
            key = (connection_n.find('source_domain'), connection_n.find('sink_domain'))
            if not all(key):
                print >> sys.stderr, 'Warning: Empty domain key(s) in connection template.\n\t{}'.format(xml_file)
            elif key in self.connection_templates:
                print >> sys.stderr, 'Warning: Connection template "{}" already exists.\n\t{}'.format(key, xml_file)
            else:
                self.connection_templates[key] = connection_n.find('make') or ''

    def load_block_tree(self, block_tree):
        """
        Load a block tree with categories and blocks.
        Step 1: Load all blocks from the xml specification.
        Step 2: Load blocks with builtin category specifications.

        Args:
            block_tree: the block tree object
        """
        # Recursive function to load categories and blocks
        def load_category(cat_n, parent=None):
            # Add this category
            parent = (parent or []) + [cat_n.find('name')]
            block_tree.add_block(parent)
            # Recursive call to load sub categories
            map(lambda c: load_category(c, parent), cat_n.findall('cat'))
            # Add blocks in this category
            for block_key in cat_n.findall('block'):
                if block_key not in self.blocks:
                    print >> sys.stderr, 'Warning: Block key "{}" not found when loading category tree.'.format(block_key)
                    continue
                block = self.blocks[block_key]
                # If it exists, the block's category shall not be overridden by the xml tree
                if not block.get_category():
                    block.set_category(parent)

        # Recursively load the category trees and update the categories for each block
        for category_tree_n in self._category_trees_n:
            load_category(category_tree_n)

        # Add blocks to block tree
        for block in self.blocks.itervalues():
            # Blocks with empty categories are hidden
            if not block.get_category():
                continue
            block_tree.add_block(block.get_category(), block)

    def _save_docstring_extraction_result(self, key, docstrings):
        docs = {}
        for match, docstring in docstrings.iteritems():
            if not docstring or match.endswith('_sptr'):
                continue
            docstring = docstring.replace('\n\n', '\n').strip()
            docs[match] = docstring
        self.block_docstrings[key] = docs

    ##############################################
    # Access
    ##############################################

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
        open(flow_graph_file, 'r')  # Test open
        ParseXML.validate_dtd(flow_graph_file, Constants.FLOW_GRAPH_DTD)
        return ParseXML.from_file(flow_graph_file)

    def get_new_flow_graph(self):
        return self.FlowGraph(platform=self)

    def get_blocks(self):
        return self.blocks.values()

    def get_new_block(self, flow_graph, key):
        return self.Block(flow_graph, n=self._blocks_n[key])

    def get_colors(self):
        return [(name, color) for name, key, sizeof, color in Constants.CORE_TYPES]
