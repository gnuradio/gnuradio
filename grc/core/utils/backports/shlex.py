# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


import re
import shlex

# back port from python3

_find_unsafe = re.compile(r'[^\w@%+=:,./-]').search


def _shlex_quote(s):
    """Return a shell-escaped version of the string *s*."""
    if not s:
        return "''"
    if _find_unsafe(s) is None:
        return s

    # use single quotes, and put single quotes into double quotes
    # the string $'b is then quoted as '$'"'"'b'
    return "'" + s.replace("'", "'\"'\"'") + "'"


if not hasattr(shlex, 'quote'):
    quote = _shlex_quote
else:
    quote = shlex.quote

split = shlex.split
