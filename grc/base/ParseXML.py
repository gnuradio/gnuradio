"""
Copyright 2008 Free Software Foundation, Inc.
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
from . import odict

xml_failures = {}


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
    #perform parsing, use dtd validation if dtd file is not specified
    try:
        parser = etree.XMLParser(dtd_validation=not dtd_file)
        xml = etree.parse(xml_file, parser=parser)
    except etree.LxmlError:
        pass
    if parser.error_log:
        raise XMLSyntaxError(parser.error_log)

    # perform dtd validation if the dtd file is specified
    if not dtd_file:
        return
    try:
        dtd = etree.DTD(dtd_file)
        if not dtd.validate(xml.getroot()):
            raise XMLSyntaxError(dtd.error_log)
    except etree.LxmlError:
        raise XMLSyntaxError(dtd.error_log)


# Parse the file and also return any GRC processing instructions
def from_file_with_instructions(xml_file):
    """
    Create nested data from an xml file using the from xml helper.
    Also get the grc version information. 
    
    Args:
        xml_file: the xml file path
    
    Returns:
        the nested data, grc version information
    """
    xml = etree.parse(xml_file)

    # Get the embedded instructions and build a dictionary item
    instructions = None
    xml_instructions = xml.xpath('/processing-instruction()')
    for inst in xml_instructions:
        if (inst.target == 'grc'):
            instructions = inst.attrib

    nested_data = _from_file(xml.getroot())
    return (nested_data, instructions)


def from_file(xml_file):
    """
    Create nested data from an xml file using the from xml helper.

    Args:
        xml_file: the xml file path

    Returns:
        the nested data
    """
    xml = etree.parse(xml_file).getroot()
    return _from_file(xml)


def _from_file(xml):
    """
    Recursivly parse the xml tree into nested data format.

    Args:
        xml: the xml tree

    Returns:
        the nested data
    """
    tag = xml.tag
    if not len(xml):
        return odict({tag: xml.text or ''}) #store empty tags (text is None) as empty string
    nested_data = odict()
    for elem in xml:
        key, value = _from_file(elem).items()[0]
        if nested_data.has_key(key): nested_data[key].append(value)
        else: nested_data[key] = [value]
    #delistify if the length of values is 1
    for key, values in nested_data.iteritems():
        if len(values) == 1: nested_data[key] = values[0]

    return odict({tag: nested_data})


def to_file(nested_data, xml_file):
    """
    Write an xml file and use the to xml helper method to load it.

    Args:
        nested_data: the nested data
        xml_file: the xml file path
    """
    xml = _to_file(nested_data)[0]
    open(xml_file, 'w').write(etree.tostring(xml, xml_declaration=True, pretty_print=True))


def to_file_with_instructions(nested_data, file_path, instructions):
    """
    Write to an xml file and insert processing instructions for versioning

    Args:
        nested_data: the nested data
        xml_file: the xml file path
        instructions: array of instructions to add to the xml
    """
    xml = _to_file(nested_data)[0]
        
    # Create the processing instruction from the array
    pi_data = ""
    for key, value in instructions.iteritems():
        pi_data += str(key) + "=\'" + str(value) + "\' "
    pi = etree.ProcessingInstruction('grc', pi_data)
    
    xml_data = etree.tostring(pi, xml_declaration=True, pretty_print=True)
    xml_data += etree.tostring(xml, pretty_print=True)
    open(file_path, 'w').write(xml_data)
    
    
def _to_file(nested_data):
    """
    Recursivly parse the nested data into xml tree format.

    Args:
        nested_data: the nested data

    Returns:
        the xml tree filled with child nodes
    """
    nodes = list()
    for key, values in nested_data.iteritems():
        #listify the values if not a list
        if not isinstance(values, (list, set, tuple)):
            values = [values]
        for value in values:
            node = etree.Element(key)
            if isinstance(value, (str, unicode)): node.text = value
            else: node.extend(_to_file(value))
            nodes.append(node)
    return nodes

if __name__ == '__main__':
    """Use the main method to test parse xml's functions."""
    pass
