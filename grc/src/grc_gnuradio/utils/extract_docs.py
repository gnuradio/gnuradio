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
##@package grc_gnuradio.utils.extract_docs
#Extract documentation from the gnuradio doxygen files.

from grc_gnuradio.Constants import DOCS_DIR
from lxml import etree
import os

DOXYGEN_NAME_XPATH = '/doxygen/compounddef/compoundname'
DOXYGEN_BRIEFDESC_GR_XPATH = '/doxygen/compounddef/briefdescription'
DOXYGEN_DETAILDESC_GR_XPATH = '/doxygen/compounddef/detaileddescription'
DOXYGEN_BRIEFDESC_BLKS2_XPATH = '/doxygen/compounddef/sectiondef[@kind="public-func"]/memberdef/briefdescription'
DOXYGEN_DETAILDESC_BLKS2_XPATH = '/doxygen/compounddef/sectiondef[@kind="public-func"]/memberdef/detaileddescription'

def extract_txt(xml, parent_text=None):
	"""!
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

def is_match(key, file):
	"""!
	Is the block key a match for the given file name?
	@param key block key
	@param file the xml file name
	@return true if matches
	"""
	if not file.endswith('.xml'): return False
	file = file.replace('.xml', '') #remove file ext
	file = file.replace('__', '_') #doxygen xml files have 2 underscores
	if key.startswith('gr_'):
		if not file.startswith('classgr_'): return False
		key = key.replace('gr_', 'classgr_')
	elif key.startswith('trellis_'):
		if not file.startswith('classtrellis_'): return False
		key = key.replace('trellis_', 'classtrellis_')
	elif key.startswith('blks2_'):
		if not file.startswith('classgnuradio_'): return False
		if 'blks2' not in file: return False
		file = file.replace('_1_1', '_') #weird blks2 doxygen syntax
		key = key.replace('blks2_', '')
	else: return False
	for k, f in zip(*map(reversed, map(lambda x: x.split('_'), [key, file]))):
		if k == f: continue
		ks = k.split('x')
		if len(ks) == 2 and f.startswith(ks[0]) and f.endswith(ks[1]): continue
		if len(ks) > 2 and all(ki in ('x', fi) for ki, fi in zip(k, f)): continue
		return False
	return True

def extract(key):
	"""!
	Extract the documentation from the doxygen generated xml files.
	If multiple files match, combine the docs.
	@param key the block key
	@return a string with documentation
	"""
	#get potential xml file matches for the key
	if os.path.exists(DOCS_DIR) and os.path.isdir(DOCS_DIR):
		matches = filter(lambda f: is_match(key, f), os.listdir(DOCS_DIR))
	else: matches = list()
	#combine all matches
	doc_strs = list()
	for match in matches:
		try:
			xml_file = DOCS_DIR + '/' + match
			xml = etree.parse(xml_file)
			#extract descriptions
			comp_name = extract_txt(xml.xpath(DOXYGEN_NAME_XPATH)[0]).strip('\n')
			comp_name = '   ---   ' + comp_name + '   ---   '
			if key.startswith('gr_') or key.startswith('trellis_'):
				brief_desc = extract_txt(xml.xpath(DOXYGEN_BRIEFDESC_GR_XPATH)[0]).strip('\n')
				detailed_desc = extract_txt(xml.xpath(DOXYGEN_DETAILDESC_GR_XPATH)[0]).strip('\n')
			elif key.startswith('blks2_'):
				brief_desc = extract_txt(xml.xpath(DOXYGEN_BRIEFDESC_BLKS2_XPATH)[0]).strip('\n')
				detailed_desc = extract_txt(xml.xpath(DOXYGEN_DETAILDESC_BLKS2_XPATH)[0]).strip('\n')
			else:
				brief_desc = ''
				detailed_desc = ''
			#combine
			doc_strs.append('\n'.join([comp_name, brief_desc, detailed_desc]).strip('\n'))
		except IndexError: pass #bad format
	return '\n\n'.join(doc_strs)

if __name__ == '__main__':
	import sys
	print extract(sys.argv[1])
