"""
Copyright 2008,2013 Free Software Foundation, Inc.
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
try:
    import pygtk
    pygtk.require('2.0')
    import gtk

    _COLORMAP = gtk.gdk.colormap_get_system() #create all of the colors
    def get_color(color_code): return _COLORMAP.alloc_color(color_code, True, True)

    HIGHLIGHT_COLOR = get_color('#00FFFF')
    BORDER_COLOR = get_color('#444444')
    # missing blocks stuff
    MISSING_BLOCK_BACKGROUND_COLOR = get_color('#FFF2F2')
    MISSING_BLOCK_BORDER_COLOR = get_color('red')
    #param entry boxes
    ENTRYENUM_CUSTOM_COLOR = get_color('#EEEEEE')
    #flow graph color constants
    FLOWGRAPH_BACKGROUND_COLOR = get_color('#FFFFFF')
    COMMENT_BACKGROUND_COLOR = get_color('#F3F3F3')
    FLOWGRAPH_EDGE_COLOR = COMMENT_BACKGROUND_COLOR
    #block color constants
    BLOCK_ENABLED_COLOR = get_color('#F1ECFF')
    BLOCK_DISABLED_COLOR = get_color('#CCCCCC')
    BLOCK_BYPASSED_COLOR = get_color('#F4FF81')
    #connection color constants
    CONNECTION_ENABLED_COLOR = get_color('black')
    CONNECTION_DISABLED_COLOR = get_color('#BBBBBB')
    CONNECTION_ERROR_COLOR = get_color('red')
except:
    print 'Unable to import Colors'

DEFAULT_DOMAIN_COLOR_CODE = '#777777'
