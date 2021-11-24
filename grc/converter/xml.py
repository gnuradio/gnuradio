# Copyright 2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import re
from os import path

try:
    # raise ImportError()
    from lxml import etree
    HAVE_LXML = True
except ImportError:
    import xml.etree.ElementTree as etree
    HAVE_LXML = False


_validator_cache = {None: lambda xml: True}


def _get_validator(dtd=None):
    validator = _validator_cache.get(dtd)
    if not validator:
        if not path.isabs(dtd):
            dtd = path.join(path.dirname(__file__), dtd)
        validator = _validator_cache[dtd] = etree.DTD(dtd).validate
    return validator


def load_lxml(filename, document_type_def=None):
    """Load block description from xml file"""

    try:
        xml_tree = etree.parse(filename)
        _get_validator(document_type_def)
        element = xml_tree.getroot()
    except etree.LxmlError:
        raise ValueError("Failed to parse or validate {}".format(filename))

    version_info = {}
    for inst in xml_tree.xpath('/processing-instruction()'):
        if inst.target == 'grc':
            version_info.update(inst.attrib)

    return element, version_info


def load_stdlib(filename, document_type_def=None):
    """Load block description from xml file"""

    if isinstance(filename, str):
        with open(filename, 'rb') as xml_file:
            data = xml_file.read().decode('utf-8')
    else:  # Already opened
        data = filename.read().decode('utf-8')

    try:
        element = etree.fromstring(data)
    except etree.ParseError:
        raise ValueError("Failed to parse {}".format(filename))

    version_info = {}
    for body in re.findall(r'<\?(.*?)\?>', data):
        inst = etree.fromstring('<' + body + '/>')
        if inst.tag == 'grc':
            version_info.update(inst.attrib)

    return element, version_info


load = load_lxml if HAVE_LXML else load_stdlib
