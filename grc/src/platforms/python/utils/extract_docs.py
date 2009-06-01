"""
Copyright 2008, 2009 Free Software Foundation, Inc.
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

from .. Constants import DOCS_DIR
from lxml import etree
import os
import re

DOXYGEN_NAME_XPATH = '/doxygen/compounddef/compoundname'
DOXYGEN_BRIEFDESC_GR_XPATH = '/doxygen/compounddef/briefdescription'
DOXYGEN_DETAILDESC_GR_XPATH = '/doxygen/compounddef/detaileddescription'

def extract_txt(xml, parent_text=None):
	"""
	Recursivly pull the text out of an xml tree.
	@param xml the xml tree
	@param parent_text the text of the parent element
	@return a string
	"""
	text = xml.text or ''
	tail = parent_text and xml.tail or ''
	return text + ''.join(
		map(lambda x: extract_txt(x, text), xml)
	) + tail

def _extract(key):
	"""
	Extract the documentation from the doxygen generated xml files.
	If multiple files match, combine the docs.
	@param key the block key
	@return a string with documentation
	"""
	UBUNTU_DOCS_DIR = '/usr/share/doc/gnuradio-doc/xml'
	if os.path.exists(DOCS_DIR): docs_dir = DOCS_DIR
	elif os.path.exists(UBUNTU_DOCS_DIR): docs_dir = UBUNTU_DOCS_DIR
	else: return ''
	#extract matches
	pattern = key.replace('_', '_*').replace('x', '\w')
	prog = re.compile('^class%s\..*$'%pattern)
	matches = filter(lambda f: prog.match(f), os.listdir(docs_dir))
	#combine all matches
	doc_strs = list()
	for match in matches:
		try:
			xml_file = os.path.join(docs_dir, match)
			xml = etree.parse(xml_file)
			#extract descriptions
			comp_name = extract_txt(xml.xpath(DOXYGEN_NAME_XPATH)[0]).strip('\n')
			comp_name = '   ---   ' + comp_name + '   ---   '
			if re.match('(gr|usrp2|trellis)_.*', key):
				brief_desc = extract_txt(xml.xpath(DOXYGEN_BRIEFDESC_GR_XPATH)[0]).strip('\n')
				detailed_desc = extract_txt(xml.xpath(DOXYGEN_DETAILDESC_GR_XPATH)[0]).strip('\n')
			else:
				brief_desc = ''
				detailed_desc = ''
			#combine
			doc_strs.append('\n'.join([comp_name, brief_desc, detailed_desc]).strip('\n'))
		except IndexError: pass #bad format
	return '\n\n'.join(doc_strs)

_docs_cache = dict()
def extract(key):
	"""
	Call the private extract and cache the result.
	@param key the block key
	@return a string with documentation
	"""
	try: assert _docs_cache.has_key(key)
	except: _docs_cache[key] = _extract(key)
	return _docs_cache[key]

if __name__ == '__main__':
	import sys
	print extract(sys.argv[1])
