"""
Copyright 2008-2011 Free Software Foundation, Inc.
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

import re

def _extract(key):
	"""
	Extract the documentation from the python __doc__ strings.
	If multiple modules match, combine the docs.
	@param key the block key
	@return a string with documentation
	"""
	#extract matches
	try:
		module_name, constructor_name = key.split('_', 1)
		module = __import__('gnuradio.'+module_name)
		module = getattr(module, module_name)
	except: return ''
	pattern = constructor_name.replace('_', '_*').replace('x', '\w')
	pattern_matcher = re.compile('^%s\w*$'%pattern)
	matches = filter(lambda x: pattern_matcher.match(x), dir(module))
	#combine all matches
	doc_strs = list()
	for match in matches:
		try:
			title = '   ---   ' + match + '   ---   '
			doc_strs.append('\n\n'.join([title, getattr(module, match).__doc__]).strip())
		except: pass
	return '\n\n'.join(doc_strs)

_docs_cache = dict()
def extract(key):
	"""
	Call the private extract and cache the result.
	@param key the block key
	@return a string with documentation
	"""
	if not _docs_cache.has_key(key):
		_docs_cache[key] = _extract(key)
	return _docs_cache[key]

if __name__ == '__main__':
	import sys
	print extract(sys.argv[1])
