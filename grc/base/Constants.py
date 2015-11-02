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

#data files
DATA_DIR = os.path.dirname(__file__)
FLOW_GRAPH_DTD = os.path.join(DATA_DIR, 'flow_graph.dtd')
BLOCK_TREE_DTD = os.path.join(DATA_DIR, 'block_tree.dtd')

# file format versions:
#  0: undefined / legacy
#  1: non-numeric message port keys (label is used instead)
FLOW_GRAPH_FILE_FORMAT_VERSION = 1

# Param tabs
DEFAULT_PARAM_TAB = "General"
ADVANCED_PARAM_TAB = "Advanced"

# Port domains
DOMAIN_DTD = os.path.join(DATA_DIR, 'domain.dtd')
GR_STREAM_DOMAIN = "gr_stream"
GR_MESSAGE_DOMAIN = "gr_message"
DEFAULT_DOMAIN = GR_STREAM_DOMAIN

BLOCK_FLAG_THROTTLE = 'throttle'
BLOCK_FLAG_DISABLE_BYPASS = 'disable_bypass'
BLOCK_FLAG_NEED_QT_GUI = 'need_qt_gui'
BLOCK_FLAG_NEED_WX_GUI = 'need_ex_gui'

# Block States
BLOCK_DISABLED = 0
BLOCK_ENABLED = 1
BLOCK_BYPASSED = 2
