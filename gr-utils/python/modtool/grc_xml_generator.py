#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#
import xml.etree.ElementTree as ET
from util_functions import is_number, xml_indent

try:
    import lxml.etree
    LXML_IMPORTED = True
except ImportError:
    LXML_IMPORTED = False

class GRCXMLGenerator(object):
    """ Create and write the XML bindings for a GRC block. """
    def __init__(self, modname=None, blockname=None, doc=None, params=None, iosig=None):
        """docstring for __init__"""
        params_list = ['$'+s['key'] for s in params if s['in_constructor']]
        # Can't make a dict 'cause order matters
        self._header = (('name', blockname.replace('_', ' ').capitalize()),
                        ('key', '%s_%s' % (modname, blockname)),
                        ('category', '[%s]' % modname.upper()),
                        ('import', 'import %s' % modname),
                        ('make', '%s.%s(%s)' % (modname, blockname, ', '.join(params_list)))
                       )
        self.params = params
        self.iosig = iosig
        self.doc = doc
        self.root = None
        if LXML_IMPORTED:
            self._prettyprint = self._lxml_prettyprint
        else:
            self._prettyprint = self._manual_prettyprint

    def _lxml_prettyprint(self):
        """ XML pretty printer using lxml """
        return lxml.etree.tostring(
                   lxml.etree.fromstring(ET.tostring(self.root, encoding="UTF-8")),
                   pretty_print=True
               )

    def _manual_prettyprint(self):
        """ XML pretty printer using xml_indent """
        xml_indent(self.root)
        return ET.tostring(self.root, encoding="UTF-8")

    def make_xml(self):
        """ Create the actual tag tree """
        root = ET.Element("block")
        iosig = self.iosig
        for tag, value in self._header:
            this_tag = ET.SubElement(root, tag)
            this_tag.text = value
        for param in self.params:
            param_tag = ET.SubElement(root, 'param')
            ET.SubElement(param_tag, 'name').text = param['key'].capitalize()
            ET.SubElement(param_tag, 'key').text = param['key']
            if len(param['default']):
                ET.SubElement(param_tag, 'value').text = param['default']
            ET.SubElement(param_tag, 'type').text = param['type']
        for inout in sorted(iosig.keys()):
            if iosig[inout]['max_ports'] == '0':
                continue
            for i in range(len(iosig[inout]['type'])):
                s_tag = ET.SubElement(root, {'in': 'sink', 'out': 'source'}[inout])
                ET.SubElement(s_tag, 'name').text = inout
                ET.SubElement(s_tag, 'type').text = iosig[inout]['type'][i]
                if iosig[inout]['vlen'][i] != '1':
                    vlen = iosig[inout]['vlen'][i]
                    if is_number(vlen):
                        ET.SubElement(s_tag, 'vlen').text = vlen
                    else:
                        ET.SubElement(s_tag, 'vlen').text = '$'+vlen
                if i == len(iosig[inout]['type'])-1:
                    if not is_number(iosig[inout]['max_ports']):
                        ET.SubElement(s_tag, 'nports').text = iosig[inout]['max_ports']
                    elif len(iosig[inout]['type']) < int(iosig[inout]['max_ports']):
                        ET.SubElement(s_tag, 'nports').text = str(int(iosig[inout]['max_ports']) -
                                                                  len(iosig[inout]['type'])+1)
        if self.doc is not None:
            ET.SubElement(root, 'doc').text = self.doc
        self.root = root

    def save(self, filename):
        """ Write the XML file """
        self.make_xml()
        open(filename, 'w').write(self._prettyprint())

