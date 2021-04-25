"""
Copyright 2008,2013 Free Software Foundation, Inc.
Copyright 2021 Marcus Müller

This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from gi.repository import Gdk

from .. import Constants

from typing import Tuple


def get_color(color_code: str):
    if not color_code.startswith("#"):
        color = Gdk.RGBA()
        color.parse(color_code)
        return color.red, color.green, color.blue, color.alpha

    color_code = color_code[1:]
    a = 0xFF
    if len(color_code) == 8:
        a = int(color_code[-2], 16)
        color_code = color_code[:-2]
    if len(color_code) == 6:
        r = int(color_code[0:2], 16)
        g = int(color_code[2:4], 16)
        b = int(color_code[4:6], 16)
        return r / 255, g / 255, b / 255, a / 255
    raise Exception(f"invalid color: {color_code}")


def get_hexcolor(color_tuple: Tuple[float, float, float]):
    """
    Converts a tuple (r,g,b)∈[0,1]³ to an RGB hexcode #000000…#FFFFFF
    """
    r, g, b, *rest = (int(val * 0xFF) for val in color_tuple)
    assert len(rest) <= 1
    return f"#{r:02X}{b:02X}{g:02X}"


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

PORT_TYPE_TO_COLOR = {key: get_color(color) for name, key, sizeof, color in Constants.CORE_TYPES}
PORT_TYPE_TO_COLOR.update((key, get_color(color)) for key, (_, color) in Constants.ALIAS_TYPES.items())


#################################################################################
# param box colors
#################################################################################
DARK_THEME_STYLES = b"""
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
                         #dtype_raw             { background-color: #DDDDDD; }

                         #enum_custom           { background-color: #EEEEEE; }
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
