"""
Copyright 2009 Free Software Foundation, Inc.
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

from Constants import DOCS_DIR
from lxml import etree
import os
import re

DOXYGEN_TITLE_XPATH = '/doxygen/compounddef/title'
DOXYGEN_CLASS_XPATH = '/doxygen/compounddef/innerclass'

#map a group/category to a list of blocks
_category_map = dict()

#extract the group/category information
docs_dir = os.path.join(DOCS_DIR, 'xml')
if os.path.exists(docs_dir):
	group_file_matcher = re.compile('^group__\w*\..*$') #xml or xml.gz
	matches = filter(lambda f: group_file_matcher.match(f), os.listdir(docs_dir))
	for match in matches:
		try:
			xml_file = os.path.join(docs_dir, match)
			xml = etree.parse(xml_file)
			category = xml.xpath(DOXYGEN_TITLE_XPATH)[0].text
			blocks = map(lambda x: x.text, xml.xpath(DOXYGEN_CLASS_XPATH))
			_category_map[category] = blocks
		except: pass

def extract(key):
	"""
	Match the given key to a key in an existing category.
	If no match can be made, return an empty string.
	@param key the block key
	@return the category or empty string
	"""
	pattern = key.replace('_', '_*').replace('x', '\w')
	class_name_matcher = re.compile('^%s$'%pattern)
	for category, blocks in _category_map.iteritems():
		for block in blocks:
			if class_name_matcher.match(block): return category
	return ''

if __name__ == '__main__':
	import sys
	print extract(sys.argv[1])
