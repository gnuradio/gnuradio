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

import re
import itertools


def docstring_guess_from_key(key):
    """Extract the documentation from the python __doc__ strings

    By guessing module and constructor names from key

    Args:
        key: the block key

    Returns:
        a dict (block_name --> doc string)
    """
    doc_strings = dict()

    in_tree = [key.partition('_')[::2] + (
        lambda package: getattr(__import__('gnuradio.' + package), package),
    )]

    key_parts = key.split('_')
    oot = [
        ('_'.join(key_parts[:i]), '_'.join(key_parts[i:]), __import__)
        for i in range(1, len(key_parts))
    ]

    for module_name, init_name, importer in itertools.chain(in_tree, oot):
        if not module_name or not init_name:
            continue
        try:
            module = importer(module_name)
            break
        except ImportError:
            continue
    else:
        return doc_strings

    pattern = re.compile(
        '^' + init_name.replace('_', '_*').replace('x', r'\w') + r'\w*$'
    )
    for match in filter(pattern.match, dir(module)):
        try:
            doc_strings[match] = getattr(module, match).__doc__.strip()
        except AttributeError:
            continue

    return doc_strings


_docs_cache = dict()


def extract(key):
    """
    Call the private extract and cache the result.

    Args:
        key: the block key

    Returns:
        a string with documentation
    """
    if not _docs_cache.has_key(key):
        docstrings = docstring_guess_from_key(key)
        _docs_cache[key] = '\n\n'.join(
            '   ---   {0}   ---   \n\n{1}'.format(match, docstring)
            for match, docstring in docstrings.iteritems()
        )
    return _docs_cache[key]


if __name__ == '__main__':
    import sys
    print extract(sys.argv[1])
