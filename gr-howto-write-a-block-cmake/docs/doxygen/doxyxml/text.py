#
# Copyright 2010 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 
"""
Utilities for extracting text from generated classes.
"""

def is_string(txt):
    if isinstance(txt, str):
        return True
    try:
        if isinstance(txt, unicode):
            return True
    except NameError:
        pass
    return False

def description(obj):
    if obj is None:
        return None
    return description_bit(obj).strip()

def description_bit(obj):
    if hasattr(obj, 'content'):
        contents = [description_bit(item) for item in obj.content]
        result = ''.join(contents)
    elif hasattr(obj, 'content_'):
        contents = [description_bit(item) for item in obj.content_]
        result = ''.join(contents)
    elif hasattr(obj, 'value'):
        result = description_bit(obj.value)
    elif is_string(obj):
        return obj
    else:
        raise StandardError('Expecting a string or something with content, content_ or value attribute')
    # If this bit is a paragraph then add one some line breaks.
    if hasattr(obj, 'name') and obj.name == 'para':
        result += "\n\n"
    return result
