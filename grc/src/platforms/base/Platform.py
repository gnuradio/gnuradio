"""
Copyright 2008 Free Software Foundation, Inc.
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
from ... utils import ParseXML
from ... import utils
from Element import Element as _Element
from FlowGraph import FlowGraph as _FlowGraph
from Connection import Connection as _Connection
from Block import Block as _Block
from Port import Port as _Port
from Param import Param as _Param
from Constants import BLOCK_TREE_DTD

class Platform(_Element):

	def __init__(self, name, key, block_paths, block_dtd, block_tree, default_flow_graph, generator):
		"""
		Make a platform from the arguments.
		@param name the platform name
		@param key the unique platform key
		@param block_paths the file paths to blocks in this platform
		@param block_dtd the dtd validator for xml block wrappers
		@param block_tree the nested tree of block keys and categories
		@param default_flow_graph the default flow graph file path
		@param load_one a single file to load into this platform or None
		@return a platform object
		"""
		_Element.__init__(self)
		self._name = name
		self._key = key
		self._block_paths = block_paths
		self._block_dtd = block_dtd
		self._block_tree = block_tree
		self._default_flow_graph = default_flow_graph
		self._generator = generator
		#create a dummy flow graph for the blocks
		self._flow_graph = _Element(self)
		#load the blocks
		self._blocks = dict()
		self._blocks_n = dict()
		for block_path in self._block_paths:
			if os.path.isfile(block_path): self._load_blocks(block_path)
			elif os.path.isdir(block_path):
				for dirpath, dirnames, filenames in os.walk(block_path):
					for filename in filter(lambda f: f.endswith('.xml'), filenames):
						self._load_blocks(os.path.join(dirpath, filename))

	def get_prefs_block(self): return self.get_new_flow_graph().get_new_block('preferences')

	def _load_blocks(self, f):
		"""
		Load the block wrappers from the file path.
		The block wrapper must pass validation.
		If any of the checks fail, exit with error.
		@param f the file path
		"""
		try: ParseXML.validate_dtd(f, self._block_dtd)
		except ParseXML.XMLSyntaxError, e: self._exit_with_error('Block definition "%s" failed: \n\t%s'%(f, e))
		for n in utils.listify(ParseXML.from_file(f), 'block'):
			#inject block wrapper path
			n['block_wrapper_path'] = f
			block = self.Block(self._flow_graph, n)
			key = block.get_key()
			#test against repeated keys
			try: assert(key not in self.get_block_keys())
			except AssertionError: self._exit_with_error('Key "%s" already exists in blocks'%key)
			#store the block
			self._blocks[key] = block
			self._blocks_n[key] = n

	def load_block_tree(self, block_tree):
		"""
		Load a block tree with categories and blocks.
		Step 1: Load all blocks from the xml specification.
		Step 2: Load blocks with builtin category specifications.
		@param block_tree the block tree object
		"""
		#recursive function to load categories and blocks
		def load_category(cat_n, parent=''):
			#add this category
			parent = '%s/%s'%(parent, cat_n['name'])
			block_tree.add_block(parent)
			#recursive call to load sub categories
			map(lambda c: load_category(c, parent), utils.listify(cat_n, 'cat'))
			#add blocks in this category
			for block_key in utils.listify(cat_n, 'block'):
				block_tree.add_block(parent, self.get_block(block_key))
		#load the block tree
		f = self._block_tree
		try: ParseXML.validate_dtd(f, BLOCK_TREE_DTD)
		except ParseXML.XMLSyntaxError, e: self._exit_with_error('Block tree "%s" failed: \n\t%s'%(f, e))
		#add all blocks in the tree
		load_category(ParseXML.from_file(f)['cat'])
		#add all other blocks, use the catgory
		for block in self.get_blocks():
			#blocks with empty categories are in the xml block tree or hidden
			if block.get_category(): block_tree.add_block(block.get_category(), block)

	def __str__(self): return 'Platform - %s(%s)'%(self.get_key(), self.get_name())

	def is_platform(self): return True

	def get_new_flow_graph(self): return self.FlowGraph(self)

	def get_default_flow_graph(self): return self._default_flow_graph

	def get_generator(self): return self._generator

	##############################################
	# Access Blocks
	##############################################
	def get_block_keys(self): return self._blocks.keys()
	def get_block(self, key): return self._blocks[key]
	def get_blocks(self): return self._blocks.values()
	def get_new_block(self, flow_graph, key): return self.Block(flow_graph, n=self._blocks_n[key])

	def get_name(self): return self._name

	def get_key(self): return self._key

	##############################################
	# Constructors
	##############################################
	FlowGraph = _FlowGraph
	Connection = _Connection
	Block = _Block
	Source = _Port
	Sink = _Port
	Param = _Param
