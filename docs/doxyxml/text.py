#
# Copyright 2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
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
        raise Exception(
            'Expecting a string or something with content, content_ or value attribute')
    # If this bit is a paragraph then add one some line breaks.
    if hasattr(obj, 'name') and obj.name == 'para':
        result += "\n\n"
    return result
