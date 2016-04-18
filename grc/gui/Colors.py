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
    import gi
    gi.require_version('Gtk', '3.0')
    from gi.repository import Gdk

    # Not gtk3?
    #COLORMAP = Gdk.colormap_get_system() #create all of the colors
    #def get_color(color_code): return _COLORMAP.alloc_color(color_code, True, True)

    def get_color(color_code):
        chars_per_color = 2 if len(color_code) > 4 else 1
        offsets = range(1, 3 * chars_per_color + 1, chars_per_color)
        return tuple(int(color_code[o:o + 2], 16) / 255.0 for o in offsets)

    HIGHLIGHT_COLOR = get_color('#00FFFF')
    BORDER_COLOR = get_color('#444444')

    # Missing blocks stuff
    MISSING_BLOCK_BACKGROUND_COLOR = get_color('#FFF2F2')
    MISSING_BLOCK_BORDER_COLOR = get_color('#FF0000')

    # Param entry boxes
    PARAM_ENTRY_TEXT_COLOR = get_color('#000000')
    ENTRYENUM_CUSTOM_COLOR = get_color('#EEEEEE')

    # Flow graph color constants
    FLOWGRAPH_BACKGROUND_COLOR = get_color('#FFFFFF')
    COMMENT_BACKGROUND_COLOR = get_color('#F3F3F3')
    FLOWGRAPH_EDGE_COLOR = COMMENT_BACKGROUND_COLOR

    # Block color constants
    BLOCK_ENABLED_COLOR = get_color('#F1ECFF')
    BLOCK_DISABLED_COLOR = get_color('#CCCCCC')
    BLOCK_BYPASSED_COLOR = get_color('#F4FF81')

    # Connection color constants
    CONNECTION_ENABLED_COLOR = get_color('#000000')
    CONNECTION_DISABLED_COLOR = get_color('#BBBBBB')
    CONNECTION_ERROR_COLOR = get_color('#FF0000')

except Exception as e:
    print 'Unable to import Colors', e
