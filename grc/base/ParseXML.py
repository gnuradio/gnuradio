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

class XMLSyntaxError(Exception):
	def __init__(self, error_log):
		self._error_log = error_log
	def __str__(self):
		return '\n'.join(map(str, self._error_log.filter_from_errors()))

def validate_dtd(xml_file, dtd_file=None):
	"""
	Validate an xml file against its dtd.
	@param xml_file the xml file
	@param dtd_file the optional dtd file
	@throws Exception validation fails
	"""
	#perform parsing, use dtd validation if dtd file is not specified
	parser = etree.XMLParser(dtd_validation=not dtd_file)
	xml = etree.parse(xml_file, parser=parser)
	if parser.error_log: raise XMLSyntaxError(parser.error_log)
	#perform dtd validation if the dtd file is specified
	if not dtd_file: return
	dtd = etree.DTD(dtd_file)
	if not dtd.validate(xml.getroot()): raise XMLSyntaxError(dtd.error_log)

def from_file(xml_file):
	"""
	Create nested data from an xml file using the from xml helper.
	@param xml_file the xml file path
	@return the nested data
	"""
	xml = etree.parse(xml_file).getroot()
	return _from_file(xml)

def _from_file(xml):
	"""
	Recursivly parse the xml tree into nested data format.
	@param xml the xml tree
	@return the nested data
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
	@param nested_data the nested data
	@param xml_file the xml file path
	"""
	xml = _to_file(nested_data)[0]
	open(xml_file, 'w').write(etree.tostring(xml, xml_declaration=True, pretty_print=True))

def _to_file(nested_data):
	"""
	Recursivly parse the nested data into xml tree format.
	@param nested_data the nested data
	@return the xml tree filled with child nodes
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
