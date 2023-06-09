"""
Copyright 2008,2013 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from gi.repository import Gtk, Gdk, cairo
# import pycairo

from .. import Constants


def get_color(color_code):
    color = Gdk.RGBA()
    color.parse(color_code)
    return color.red, color.green, color.blue, color.alpha
    # chars_per_color = 2 if len(color_code) > 4 else 1
    # offsets = range(1, 3 * chars_per_color + 1, chars_per_color)
    # return tuple(int(color_code[o:o + 2], 16) / 255.0 for o in offsets)

#################################################################################
# fg colors
#################################################################################

HIGHLIGHT_COLOR = get_color('#00FFFF')
BORDER_COLOR = get_color('#616161')
BORDER_COLOR_DISABLED = get_color('#888888')
FONT_COLOR = get_color('#000000')

# Missing blocks stuff
MISSING_BLOCK_BACKGROUND_COLOR = get_color('#FFF2F2')
MISSING_BLOCK_BORDER_COLOR = get_color('#FF0000')

# Deprecated blocks
# a light warm yellow
BLOCK_DEPRECATED_BACKGROUND_COLOR = get_color('#FED86B')
# orange
BLOCK_DEPRECATED_BORDER_COLOR = get_color('#FF540B')

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

DEFAULT_DOMAIN_COLOR = get_color('#777777')


#################################################################################
# port colors
#################################################################################

PORT_TYPE_TO_COLOR = {key: get_color(
    color) for name, key, sizeof, color in Constants.CORE_TYPES}
PORT_TYPE_TO_COLOR.update((key, get_color(color))
                          for key, (_, color) in Constants.ALIAS_TYPES.items())


#################################################################################
# param box colors
#################################################################################

DARK_THEME_STYLES = b"""
                         #dtype_complex         { background-color: #CC6600; }
                         #dtype_real            { background-color: #007396; }
                         #dtype_float           { background-color: #007396; }
                         #dtype_int             { background-color: #FF0066; }

                         #dtype_complex_vector  { background-color: #CC6655; }
                         #dtype_real_vector     { background-color: #337396; }
                         #dtype_float_vector    { background-color: #337396; }
                         #dtype_int_vector      { background-color: #FF3366; }

                         #dtype_bool            { background-color: #FF0066; }
                         #dtype_hex             { background-color: #FF0066; }
                         #dtype_string          { background-color: #339933; }
                         #dtype_id              { background-color: #343434; }
                         #dtype_stream_id       { background-color: #343434; }
                         #dtype_raw             { background-color: #343434; }

                         #enum_custom           { background-color: #111111; }
                     """
LIGHT_THEME_STYLES = b"""
                        #dtype_complex         { background-color: #3399FF; }
                        #dtype_real            { background-color: #FF8C69; }
                        #dtype_float           { background-color: #FF8C69; }
                        #dtype_int             { background-color: #00FF99; }

                        #dtype_complex_vector  { background-color: #3399AA; }
                        #dtype_real_vector     { background-color: #CC8C69; }
                        #dtype_float_vector    { background-color: #CC8C69; }
                        #dtype_int_vector      { background-color: #00CC99; }

                        #dtype_bool            { background-color: #00FF99; }
                        #dtype_hex             { background-color: #00FF99; }
                        #dtype_string          { background-color: #CC66CC; }
                        #dtype_id              { background-color: #DDDDDD; }
                        #dtype_stream_id       { background-color: #DDDDDD; }
                        #dtype_raw             { background-color: #FFFFFF; }

                        #enum_custom           { background-color: #EEEEEE; }
                    """
