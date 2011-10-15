__doc__ = """
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
from gnuradio import gr
from .. base.Platform import Platform as _Platform
from .. gui.Platform import Platform as _GUIPlatform
from FlowGraph import FlowGraph as _FlowGraph
from Connection import Connection as _Connection
from Block import Block as _Block
from Port import Port as _Port
from Param import Param as _Param
from Generator import Generator
from Constants import \
	HIER_BLOCKS_LIB_DIR, BLOCK_DTD, \
	DEFAULT_FLOW_GRAPH, BLOCKS_DIRS
import Constants

COLORS = [(name, color) for name, key, sizeof, color in Constants.CORE_TYPES]

class Platform(_Platform, _GUIPlatform):

	def __init__(self):
		"""
		Make a platform for gnuradio.
		"""
		#ensure hier dir
		if not os.path.exists(HIER_BLOCKS_LIB_DIR): os.mkdir(HIER_BLOCKS_LIB_DIR)
		#convert block paths to absolute paths
		block_paths = set(map(os.path.abspath, BLOCKS_DIRS))
		#init
		_Platform.__init__(
			self,
			name='GNU Radio Companion',
			version=gr.version(),
			key='grc',
			license=__doc__.strip(),
			website='http://gnuradio.org/redmine/wiki/gnuradio/GNURadioCompanion',
			block_paths=block_paths,
			block_dtd=BLOCK_DTD,
			default_flow_graph=DEFAULT_FLOW_GRAPH,
			generator=Generator,
			colors=COLORS,
		)
		_GUIPlatform.__init__(self)

	##############################################
	# Constructors
	##############################################
	FlowGraph = _FlowGraph
	Connection = _Connection
	Block = _Block
	Port = _Port
	Param = _Param
