"""
Copyright 2008, 2009 Free Software Foundation, Inc.
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
from .. import VERSION #TEMP: until gnuradio has __version__
from .. base.Platform import Platform as _Platform
from FlowGraph import FlowGraph as _FlowGraph
from Connection import Connection as _Connection
from Block import Block as _Block
from Port import Source,Sink
from Param import Param as _Param
from Generator import Generator
from Constants import \
	HIER_BLOCKS_LIB_DIR, BLOCK_DTD, \
	DEFAULT_FLOW_GRAPH, BLOCKS_DIRS
import Constants

COLORS = (#title, #color spec
	('Complex', Constants.COMPLEX_COLOR_SPEC),
	('Float', Constants.FLOAT_COLOR_SPEC),
	('Integer', Constants.INT_COLOR_SPEC),
	('Short', Constants.SHORT_COLOR_SPEC),
	('Byte', Constants.BYTE_COLOR_SPEC),
	('Complex Vector', Constants.COMPLEX_VECTOR_COLOR_SPEC),
	('Float Vector', Constants.FLOAT_VECTOR_COLOR_SPEC),
	('Integer Vector', Constants.INT_VECTOR_COLOR_SPEC),
	('Short Vector', Constants.SHORT_VECTOR_COLOR_SPEC),
	('Byte Vector', Constants.BYTE_VECTOR_COLOR_SPEC),
	('Wildcard Type', Constants.WILDCARD_COLOR_SPEC),
)

class Platform(_Platform):

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
			version=VERSION,
			key='grc',
			license=__doc__.strip(),
			website='http://gnuradio.org/trac/wiki/GNURadioCompanion',
			block_paths=block_paths,
			block_dtd=BLOCK_DTD,
			default_flow_graph=DEFAULT_FLOW_GRAPH,
			generator=Generator,
			colors=COLORS,
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
