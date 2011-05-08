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
from Constants import BLOCK_TREE_DTD, FLOW_GRAPH_DTD

class Platform(_Element):

	def __init__(self, name, version, key,
		block_paths, block_dtd, default_flow_graph, generator,
		license='', website=None, colors=[]):
		"""
		Make a platform from the arguments.
		@param name the platform name
		@param version the version string
		@param key the unique platform key
		@param block_paths the file paths to blocks in this platform
		@param block_dtd the dtd validator for xml block wrappers
		@param default_flow_graph the default flow graph file path
		@param generator the generator class for this platform
		@param colors a list of title, color_spec tuples
		@param license a multi-line license (first line is copyright)
		@param website the website url for this platform
		@return a platform object
		"""
		_Element.__init__(self)
		self._name = name
		self._version = version
		self._key = key
		self._license = license
		self._website = website
		self._block_paths = block_paths
		self._block_dtd = block_dtd
		self._default_flow_graph = default_flow_graph
		self._generator = generator
		self._colors = colors
		#create a dummy flow graph for the blocks
		self._flow_graph = _Element(self)
		#search for *.xml files in the given search path
		xml_files = list()
		for block_path in self._block_paths:
			if os.path.isfile(block_path): xml_files.append(block_path)
			elif os.path.isdir(block_path):
				for dirpath, dirnames, filenames in os.walk(block_path):
					for filename in sorted(filter(lambda f: f.endswith('.xml'), filenames)):
						xml_files.append(os.path.join(dirpath, filename))
		#load the blocks
		self._blocks = odict()
		self._blocks_n = odict()
		self._block_tree_files = list()
		for xml_file in xml_files:
			try: #try to add the xml file as a block wrapper
				ParseXML.validate_dtd(xml_file, self._block_dtd)
				n = ParseXML.from_file(xml_file).find('block')
				#inject block wrapper path
				n['block_wrapper_path'] = xml_file
				block = self.Block(self._flow_graph, n)
				key = block.get_key()
				#test against repeated keys
				if key in self.get_block_keys():
					print >> sys.stderr, 'Warning: Block with key "%s" already exists.\n\tIgnoring: %s'%(key, xml_file)
				#store the block
				else:
					self._blocks[key] = block
					self._blocks_n[key] = n
			except ParseXML.XMLSyntaxError, e: 
				try: #try to add the xml file as a block tree
					ParseXML.validate_dtd(xml_file, BLOCK_TREE_DTD)
					self._block_tree_files.append(xml_file)
				except ParseXML.XMLSyntaxError, e:
					print >> sys.stderr, 'Warning: Block validation failed:\n\t%s\n\tIgnoring: %s'%(e, xml_file)
			except Exception, e:
				print >> sys.stderr, 'Warning: Block loading failed:\n\t%s\n\tIgnoring: %s'%(e, xml_file)

	def parse_flow_graph(self, flow_graph_file):
		"""
		Parse a saved flow graph file.
		Ensure that the file exists, and passes the dtd check.
		@param flow_graph_file the flow graph file
		@return nested data
		@throws exception if the validation fails
		"""
		flow_graph_file = flow_graph_file or self._default_flow_graph
		open(flow_graph_file, 'r') #test open
		ParseXML.validate_dtd(flow_graph_file, FLOW_GRAPH_DTD)
		return ParseXML.from_file(flow_graph_file)

	def load_block_tree(self, block_tree):
		"""
		Load a block tree with categories and blocks.
		Step 1: Load all blocks from the xml specification.
		Step 2: Load blocks with builtin category specifications.
		@param block_tree the block tree object
		"""
		#recursive function to load categories and blocks
		def load_category(cat_n, parent=[]):
			#add this category
			parent = parent + [cat_n.find('name')]
			block_tree.add_block(parent)
			#recursive call to load sub categories
			map(lambda c: load_category(c, parent), cat_n.findall('cat'))
			#add blocks in this category
			for block_key in cat_n.findall('block'):
				if block_key not in self.get_block_keys():
					print >> sys.stderr, 'Warning: Block key "%s" not found when loading category tree.'%(block_key)
					continue
				block = self.get_block(block_key)
				#if it exists, the block's category overrides the block tree
				if not block.get_category(): block_tree.add_block(parent, block)
		#load the block tree
		for block_tree_file in self._block_tree_files:
			#recursivly add all blocks in the tree
			load_category(ParseXML.from_file(block_tree_file).find('cat'))
		#add all other blocks, use the catgory tag
		for block in self.get_blocks():
			#blocks with empty categories are in the xml block tree or hidden
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

	def get_name(self): return self._name
	def get_version(self): return self._version
	def get_key(self): return self._key
	def get_license(self): return self._license
	def get_website(self): return self._website
	def get_colors(self): return self._colors

	##############################################
	# Constructors
	##############################################
	FlowGraph = _FlowGraph
	Connection = _Connection
	Block = _Block
	Port = _Port
	Param = _Param
