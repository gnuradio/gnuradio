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
import stat
from gnuradio import gr

_gr_prefs = gr.prefs()

PYEXEC = os.environ.get('PYTHONW', _gr_prefs.get_string('grc', 'pythonw', ''))

#setup paths
PATH_SEP = ':'
DOCS_DIR = os.environ.get('GR_DOC_DIR', _gr_prefs.get_string('grc', 'doc_dir', ''))
HIER_BLOCKS_LIB_DIR = os.path.join(os.path.expanduser('~'), '.grc_gnuradio')
BLOCKS_DIRS = filter( #filter blank strings
	lambda x: x, PATH_SEP.join([
		os.environ.get('GRC_BLOCKS_PATH', ''),
		_gr_prefs.get_string('grc', 'local_blocks_path', ''),
		_gr_prefs.get_string('grc', 'global_blocks_path', ''),
	]).split(PATH_SEP),
) + [HIER_BLOCKS_LIB_DIR]

#file creation modes
TOP_BLOCK_FILE_MODE = stat.S_IRUSR | stat.S_IWUSR | stat.S_IXUSR | stat.S_IRGRP | stat.S_IWGRP | stat.S_IXGRP | stat.S_IROTH
HIER_BLOCK_FILE_MODE = stat.S_IRUSR | stat.S_IWUSR | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH

#data files
DATA_DIR = os.path.dirname(__file__)
FLOW_GRAPH_TEMPLATE = os.path.join(DATA_DIR, 'flow_graph.tmpl')
BLOCK_DTD = os.path.join(DATA_DIR, 'block.dtd')
DEFAULT_FLOW_GRAPH = os.path.join(DATA_DIR, 'default_flow_graph.grc')

#coloring
COMPLEX_COLOR_SPEC = '#3399FF'
FLOAT_COLOR_SPEC = '#FF8C69'
INT_COLOR_SPEC = '#00FF99'
SHORT_COLOR_SPEC = '#FFFF66'
BYTE_COLOR_SPEC = '#FF66FF'
COMPLEX_VECTOR_COLOR_SPEC = '#3399AA'
FLOAT_VECTOR_COLOR_SPEC = '#CC8C69'
INT_VECTOR_COLOR_SPEC = '#00CC99'
SHORT_VECTOR_COLOR_SPEC = '#CCCC33'
BYTE_VECTOR_COLOR_SPEC = '#CC66CC'
ID_COLOR_SPEC = '#DDDDDD'
WILDCARD_COLOR_SPEC = '#FFFFFF'
MSG_COLOR_SPEC = '#777777'
