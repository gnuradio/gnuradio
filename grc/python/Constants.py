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
import stat
from gnuradio import gr

_gr_prefs = gr.prefs()

#setup paths
PATH_SEP = {'/':':', '\\':';'}[os.path.sep]
HIER_BLOCKS_LIB_DIR = os.environ.get('GRC_HIER_PATH',
                                     os.path.expanduser('~/.grc_gnuradio'))
PREFS_FILE = os.environ.get('GRC_PREFS_PATH',
                            os.path.join(os.path.expanduser('~/.grc')))
BLOCKS_DIRS = filter( #filter blank strings
    lambda x: x, PATH_SEP.join([
        os.environ.get('GRC_BLOCKS_PATH', ''),
        _gr_prefs.get_string('grc', 'local_blocks_path', ''),
        _gr_prefs.get_string('grc', 'global_blocks_path', ''),
    ]).split(PATH_SEP),
) + [HIER_BLOCKS_LIB_DIR]

#user settings
XTERM_EXECUTABLE = _gr_prefs.get_string('grc', 'xterm_executable', 'xterm')

#file creation modes
TOP_BLOCK_FILE_MODE = stat.S_IRUSR | stat.S_IWUSR | stat.S_IXUSR | stat.S_IRGRP | stat.S_IWGRP | stat.S_IXGRP | stat.S_IROTH
HIER_BLOCK_FILE_MODE = stat.S_IRUSR | stat.S_IWUSR | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH

#data files
DATA_DIR = os.path.dirname(__file__)
FLOW_GRAPH_TEMPLATE = os.path.join(DATA_DIR, 'flow_graph.tmpl')
BLOCK_DTD = os.path.join(DATA_DIR, 'block.dtd')
DEFAULT_FLOW_GRAPH = os.path.join(DATA_DIR, 'default_flow_graph.grc')


# Updating colors. Using the standard color pallette from:
#  http://www.google.com/design/spec/style/color.html#color-color-palette
# Most are based on the main, primary color standard. Some are within
# that color's spectrum when it was deemed necessary.
GRC_COLOR_BROWN = '#795548'
GRC_COLOR_BLUE = '#2196F3'
GRC_COLOR_LIGHT_GREEN = '#8BC34A'
GRC_COLOR_GREEN = '#4CAF50'
GRC_COLOR_AMBER = '#FFC107'
GRC_COLOR_PURPLE = '#9C27B0'
GRC_COLOR_CYAN = '#00BCD4'
GRC_COLOR_GR_ORANGE = '#FF6905'
GRC_COLOR_ORANGE = '#F57C00'
GRC_COLOR_LIME = '#CDDC39'
GRC_COLOR_TEAL = '#009688'
GRC_COLOR_YELLOW = '#FFEB3B'
GRC_COLOR_PINK = '#F50057'
GRC_COLOR_LIGHT_PURPLE = '#E040FB'
GRC_COLOR_DARK_GREY = '#72706F'
GRC_COLOR_GREY = '#BDBDBD'
GRC_COLOR_WHITE = '#FFFFFF'


CORE_TYPES = ( #name, key, sizeof, color
    ('Complex Float 64', 'fc64', 16, GRC_COLOR_BROWN),
    ('Complex Float 32', 'fc32', 8, GRC_COLOR_BLUE),
    ('Complex Integer 64', 'sc64', 16, GRC_COLOR_LIGHT_GREEN),
    ('Complex Integer 32', 'sc32', 8, GRC_COLOR_GREEN),
    ('Complex Integer 16', 'sc16', 4, GRC_COLOR_AMBER),
    ('Complex Integer 8', 'sc8', 2, GRC_COLOR_PURPLE),
    ('Float 64', 'f64', 8, GRC_COLOR_CYAN),
    ('Float 32', 'f32', 4, GRC_COLOR_ORANGE),
    ('Integer 64', 's64', 8, GRC_COLOR_LIME),
    ('Integer 32', 's32', 4, GRC_COLOR_TEAL),
    ('Integer 16', 's16', 2, GRC_COLOR_YELLOW),
    ('Integer 8', 's8', 1, GRC_COLOR_LIGHT_PURPLE),
    ('Message Queue', 'msg', 0, GRC_COLOR_DARK_GREY),
    ('Async Message', 'message', 0, GRC_COLOR_GREY),
    ('Bus Connection', 'bus', 0, GRC_COLOR_WHITE),
    ('Wildcard', '', 0, GRC_COLOR_WHITE),
)

ALIAS_TYPES = {
    'complex' : (8, GRC_COLOR_BLUE),
    'float'   : (4, GRC_COLOR_ORANGE),
    'int'     : (4, GRC_COLOR_TEAL),
    'short'   : (2, GRC_COLOR_YELLOW),
    'byte'    : (1, GRC_COLOR_LIGHT_PURPLE),
}

TYPE_TO_COLOR = dict()
TYPE_TO_SIZEOF = dict()
for name, key, sizeof, color in CORE_TYPES:
    TYPE_TO_COLOR[key] = color
    TYPE_TO_SIZEOF[key] = sizeof
for key, (sizeof, color) in ALIAS_TYPES.iteritems():
    TYPE_TO_COLOR[key] = color
    TYPE_TO_SIZEOF[key] = sizeof

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
