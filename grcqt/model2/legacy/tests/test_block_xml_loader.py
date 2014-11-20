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

import pytest
from os import path
import glob

from .. import load_block_xml
from .. block_xml_loader import Resolver


def iter_resource_files():
    test_file_dir = path.join(path.dirname(__file__), 'resources')
    for filename in glob.iglob(path.join(test_file_dir, '*.xml')):
        with open(filename) as fp:
            yield fp


def test_category_tree_xml():
    for fp in iter_resource_files():
        BlockClass = load_block_xml(fp)
        print BlockClass


@pytest.fixture
def resolver():
    return Resolver({'param': [
        dict(key=('t1',), value=('t11',)),
        dict(key=('t2',), value=('t21',)),
        dict(key=('t3',), value=('t31',)),
    ]})


def test_resolver_fixed_value(resolver):
    assert resolver.eval('p1', 'no dollar sign in here') == 'no dollar sign in here'


def test_resolver_simple_template(resolver):
    assert resolver.eval('test', "$t1") == 't11'
    # pop the rewrite action
    assert resolver.pop_rewrites() == dict(test='t1')
    # make sure its gone
    assert resolver.pop_rewrites() == dict()


def test_resolver_get_raw():
    assert Resolver.get_raw(dict(test=('a',)), 'test') == 'a'
    assert Resolver.get_raw(dict(test='a'), 'test') == 'a'

def test_resolver_get(resolver):
    assert resolver.get(dict(test='$t1'), 'test', 't') == 't11'
    assert resolver.pop_rewrites() == dict(t='t1')

