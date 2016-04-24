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

from lxml import etree

from .utils import odict

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
    nested_data = _from_file(xml.getroot())

    # Get the embedded instructions and build a dictionary item
    nested_data['_instructions'] = {}
    xml_instructions = xml.xpath('/processing-instruction()')
    for inst in filter(lambda i: i.target == 'grc', xml_instructions):
        nested_data['_instructions'] = odict(inst.attrib)
    return nested_data


def _from_file(xml):
    """
    Recursively parse the xml tree into nested data format.

    Args:
        xml: the xml tree

    Returns:
        the nested data
    """
    tag = xml.tag
    if not len(xml):
        return odict({tag: xml.text or ''})  # store empty tags (text is None) as empty string
    nested_data = odict()
    for elem in xml:
        key, value = _from_file(elem).items()[0]
        if key in nested_data:
            nested_data[key].append(value)
        else:
            nested_data[key] = [value]
    # Delistify if the length of values is 1
    for key, values in nested_data.iteritems():
        if len(values) == 1:
            nested_data[key] = values[0]

    return odict({tag: nested_data})


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
                "{0}='{1}'".format(*item) for item in instructions.iteritems())
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
    for key, values in nested_data.iteritems():
        # Listify the values if not a list
        if not isinstance(values, (list, set, tuple)):
            values = [values]
        for value in values:
            node = etree.Element(key)
            if isinstance(value, (str, unicode)):
                node.text = unicode(value)
            else:
                node.extend(_to_file(value))
            nodes.append(node)
    return nodes
