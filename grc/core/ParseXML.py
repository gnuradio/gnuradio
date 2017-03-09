"""
Copyright 2008, 2015 Free Software Foundation, Inc.
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

from __future__ import absolute_import

from lxml import etree

import six
from six.moves import map


xml_failures = {}
etree.set_default_parser(etree.XMLParser(remove_comments=True))


class XMLSyntaxError(Exception):
    def __init__(self, error_log):
        self._error_log = error_log
        xml_failures[error_log.last_error.filename] = error_log

    def __str__(self):
        return '\n'.join(map(str, self._error_log.filter_from_errors()))


def validate_dtd(xml_file, dtd_file=None):
    """
    Validate an xml file against its dtd.

    Args:
        xml_file: the xml file
        dtd_file: the optional dtd file
    @throws Exception validation fails
    """
    # Perform parsing, use dtd validation if dtd file is not specified
    parser = etree.XMLParser(dtd_validation=not dtd_file)
    try:
        xml = etree.parse(xml_file, parser=parser)
    except etree.LxmlError:
        pass
    if parser.error_log:
        raise XMLSyntaxError(parser.error_log)

    # Perform dtd validation if the dtd file is specified
    if not dtd_file:
        return
    try:
        dtd = etree.DTD(dtd_file)
        if not dtd.validate(xml.getroot()):
            raise XMLSyntaxError(dtd.error_log)
    except etree.LxmlError:
        raise XMLSyntaxError(dtd.error_log)


def from_file(xml_file):
    """
    Create nested data from an xml file using the from xml helper.
    Also get the grc version information.

    Args:
        xml_file: the xml file path

    Returns:
        the nested data with grc version information
    """
    xml = etree.parse(xml_file)

    tag, nested_data = _from_file(xml.getroot())
    nested_data = {tag: nested_data, '_instructions': {}}

    # Get the embedded instructions and build a dictionary item
    xml_instructions = xml.xpath('/processing-instruction()')
    for inst in xml_instructions:
        if inst.target != 'grc':
            continue
        nested_data['_instructions'] = dict(inst.attrib)
    return nested_data


WANT_A_LIST = {
    '/block': 'import callback param check sink source'.split(),
    '/block/param_tab_order': 'tab'.split(),
    '/block/param': 'option'.split(),
    '/block/param/option': 'opt'.split(),
    '/flow_graph': 'block connection'.split(),
    '/flow_graph/block': 'param'.split(),
    '/cat': 'cat block'.split(),
    '/cat/cat': 'cat block'.split(),
    '/cat/cat/cat': 'cat block'.split(),
    '/cat/cat/cat/cat': 'cat block'.split(),
    '/domain': 'connection'.split(),
}


def _from_file(xml, parent_tag=''):
    """
    Recursively parse the xml tree into nested data format.

    Args:
        xml: the xml tree

    Returns:
        the nested data
    """
    tag = xml.tag
    tag_path = parent_tag + '/' + tag

    if not len(xml):
        return tag, xml.text or ''  # store empty tags (text is None) as empty string

    nested_data = {}
    for elem in xml:
        key, value = _from_file(elem, tag_path)

        if key in WANT_A_LIST.get(tag_path, []):
            try:
                nested_data[key].append(value)
            except KeyError:
                nested_data[key] = [value]
        else:
            nested_data[key] = value

    return tag, nested_data


def to_file(nested_data, xml_file):
    """
    Write to an xml file and insert processing instructions for versioning

    Args:
        nested_data: the nested data
        xml_file: the xml file path
    """
    xml_data = ""
    instructions = nested_data.pop('_instructions', None)
    # Create the processing instruction from the array
    if instructions:
        xml_data += etree.tostring(etree.ProcessingInstruction(
            'grc', ' '.join(
                "{0}='{1}'".format(*item) for item in six.iteritems(instructions))
        ), xml_declaration=True, pretty_print=True, encoding='utf-8')
    xml_data += etree.tostring(_to_file(nested_data)[0],
                               pretty_print=True, encoding='utf-8')
    with open(xml_file, 'w') as fp:
        fp.write(xml_data)


def _to_file(nested_data):
    """
    Recursively parse the nested data into xml tree format.

    Args:
        nested_data: the nested data

    Returns:
        the xml tree filled with child nodes
    """
    nodes = list()
    for key, values in six.iteritems(nested_data):
        # Listify the values if not a list
        if not isinstance(values, (list, set, tuple)):
            values = [values]
        for value in values:
            node = etree.Element(key)
            if isinstance(value, (str, six.text_type)):
                node.text = six.text_type(value)
            else:
                node.extend(_to_file(value))
            nodes.append(node)
    return nodes
