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

import tempfile

from .. import load_category_tree_xml


category_tree_test_data = """
<cat>
    <name></name>
    <cat>
        <name>c</name>
        <cat>
            <name>d</name>
            <block>key11</block>
        </cat>
        <block>key1</block>
    </cat>
    <cat>
        <name>b</name>
        <block>key2</block>
        <block>key3</block>
    </cat>
    <block>key4</block>
</cat>"""


def test_category_tree_xml():
    with tempfile.TemporaryFile() as fp:
        fp.write(category_tree_test_data)
        fp.seek(0)
        category_iter = load_category_tree_xml(fp)
        assert ('key11', ['c', 'd']) == category_iter.next()
        assert ('key1', ['c']) == category_iter.next()
        assert ('key2', ['b']) == category_iter.next()
        assert ('key3', ['b']) == category_iter.next()
        assert ('key4', []) == category_iter.next()
        try:
            category_iter.next()
        except Exception as e:
            assert isinstance(e, StopIteration)
