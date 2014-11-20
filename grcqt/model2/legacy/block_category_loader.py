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

from collections import OrderedDict
from lxml import etree
from cStringIO import StringIO


BLOCK_TREE_DTD = etree.DTD(StringIO("""\
<!ELEMENT cat (name, cat*, block*)>
<!ELEMENT name (#PCDATA)>
<!ELEMENT block (#PCDATA)>
"""))


def load_category_tree_xml(xml_file):
    """Validate and parse category tree file and add yield all category paths"""
    try:
        xml = etree.parse(xml_file).getroot()
        BLOCK_TREE_DTD.validate(xml)
    except etree.LxmlError:
        return

    category_tree_n = xml_to_nested_data(xml)[1]

    # yield from =)
    for block_key, path in _load_category(category_tree_n, []):
        yield block_key, path


def _load_category(n, parent):
    """recursive function to load categories and blocks"""
    name = n['name'][0]
    path = parent + [str(name)] if name else parent
    # load sub-categories
    for sub_n in n.get('cat', []):
        for block_key, sub_categories in _load_category(sub_n, path):
            yield block_key, sub_categories  # yield from =)
    # add blocks in this category
    for block_key in n.get('block', []):
        yield block_key, path


def xml_to_nested_data(xml):
    """recursively parse the xml tree into nested data format"""
    if not len(xml):
        nested_data = xml.text or ''  # store empty tags (text is None) as empty string
    else:
        nested_data = OrderedDict()
        for elem in xml:
            key, value = xml_to_nested_data(elem)
            if key in nested_data:
                nested_data[key].append(value)
            else:
                nested_data[key] = [value]
    return xml.tag, nested_data
