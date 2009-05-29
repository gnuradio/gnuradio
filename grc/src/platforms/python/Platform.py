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
from .. base.Platform import Platform as _Platform
from FlowGraph import FlowGraph as _FlowGraph
from Connection import Connection as _Connection
from Block import Block as _Block
from Port import Source,Sink
from Param import Param as _Param
from Generator import Generator
from Constants import \
	HIER_BLOCKS_LIB_DIR, BLOCK_DTD, \
	BLOCK_TREE, DEFAULT_FLOW_GRAPH, \
	BLOCKS_DIR

_critical_blocks_only = map(lambda b: os.path.join(BLOCKS_DIR, b), ['options.xml', 'usrp_probe.xml', 'usrp2_probe.xml'])

class Platform(_Platform):

	def __init__(self, extra_blocks=[], critical_only=False):
		"""
		Make a platform for gnuradio.
		@param extra_blocks a list of block paths to load in addition to main block library
		@param critical_only only load critical blocks (used only for usrp probe scripts to speed up load time)
		"""
		#ensure hier dir
		if not os.path.exists(HIER_BLOCKS_LIB_DIR): os.mkdir(HIER_BLOCKS_LIB_DIR)
		if critical_only: block_paths = _critical_blocks_only
		else: block_paths = extra_blocks + [HIER_BLOCKS_LIB_DIR, BLOCKS_DIR]
		#convert block paths to absolute paths, ensure uniqueness
		block_paths = set(map(os.path.abspath, block_paths))
		#init
		_Platform.__init__(
			self,
			name='GRC',
			key='grc',
			block_paths=block_paths,
			block_dtd=BLOCK_DTD,
			block_tree=BLOCK_TREE,
			default_flow_graph=DEFAULT_FLOW_GRAPH,
			generator=Generator,
		)

	##############################################
	# Constructors
	##############################################
	FlowGraph = _FlowGraph
	Connection = _Connection
	Block = _Block
	Source = Source
	Sink = Sink
	Param = _Param
