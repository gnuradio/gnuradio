"""
Copyright 2014 Free Software Foundation, Inc.
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

from __future__ import absolute_import, division, print_function

BLOCK_CLASS_FILE_EXTENSION = '.grc.py'
BLOCK_XML_EXTENSION = '.xml'
BLOCK_TREE_EXTENSION = 'block_tree.xml'


BLOCK_ID_BLACK_LIST = ['self', 'options', 'gr', 'blks2', 'wxgui', 'wx', 'math', 'forms', 'firdes'] #+ filter(lambda x: not x.startswith('_'), dir(gr.top_block())) + dir(__builtin__)

