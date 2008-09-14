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

from .. platforms.base.Constants import FLOW_GRAPH_DTD
import ParseXML
from .. import utils
from .. utils import odict
from lxml import etree
import difflib
import os

def _make_param(key, value):
	"""
	Make a paramater dict from the key/value pair.
	@param key the key
	@param value the value
	@return a dictionary object
	"""
	param = odict()
	param['key'] = key
	param['value'] = value
	return param

def _get_blocks(blocks, tag):
	"""
	Get a list of blocks with the tag.
	@param blocks the old block list
	@param tag the tag name
	@retun a list of matching blocks
	"""
	return filter(lambda b: b['tag'] == tag, blocks)

def _get_params(block):
	"""
	Get a list of params.
	@param block the old block
	@retun a list of params
	"""
	params = utils.exists_or_else(block, 'params', {}) or {}
	params = utils.listify(params, 'param')
	return params

def _convert_id(id):
	"""
	Convert an old id to a new safe id.
	Replace spaces with underscores.
	Lower case the odl id.
	@return the reformatted id
	"""
	return id.lower().replace(' ', '_')

def convert(file_path, platform):
	"""
	Convert the flow graph to the new format.
	Make a backup of the old file.
	Save a reformated flow graph to the file path.
	If this is a new format flow graph, do nothing.
	@param file_path the path to the saved flow graph
	@param platform the grc gnuradio platform
	"""
	try: #return if file passes validation
		ParseXML.validate_dtd(file_path, FLOW_GRAPH_DTD)
		try:
			changed = False
			#convert instances of gui_coordinate and gui_rotation
			xml = etree.parse(file_path)
			for find, replace in (
				('gui_coordinate', '_coordinate'),
				('gui_rotation', '_rotation'),
			):
				keys = xml.xpath('/flow_graph/block/param[key="%s"]/key'%find)
				for key in keys:
					key.text = replace
					changed = True
			if not changed: return
			#backup after successful conversion
			os.rename(file_path, file_path+'.bak')
			#save new flow graph to file path
			xml.write(file_path, xml_declaration=True, pretty_print=True)
		except Exception, e: print e
		return
	except: pass #convert
	############################################################
	# extract window size, variables, blocks, and connections
	############################################################
	old_n = ParseXML.from_file(file_path)['flow_graph']
	try: window_width = min(3*int(old_n['window_width'])/2, 2048)
	except: window_width = 2048
	try: window_height = min(3*int(old_n['window_height'])/2, 2048)
	except: window_height = 2048
	window_size = '%d, %d'%(window_width, window_height)
	variables = utils.exists_or_else(old_n, 'vars', {}) or {}
	variables = utils.listify(variables, 'var')
	blocks = utils.exists_or_else(old_n, 'signal_blocks', {}) or {}
	blocks = utils.listify(blocks, 'signal_block')
	connections = utils.exists_or_else(old_n, 'connections', {}) or {}
	connections = utils.listify(connections, 'connection')
	#initialize new nested data
	new_n = odict()
	new_n['block'] = list()
	new_n['connection'] = list()
	############################################################
	# conversion - options block
	############################################################
	#get name
	about_blocks = _get_blocks(blocks, 'About')
	if about_blocks: title = _get_params(about_blocks[0])[0]
	else: title = 'Untitled'
	#get author
	if about_blocks: author = _get_params(about_blocks[0])[1]
	else: author = ''
	#get desc
	note_blocks = _get_blocks(blocks, 'Note')
	if note_blocks: desc = _get_params(note_blocks[0])[0]
	else: desc = ''
	#create options block
	options_block = odict()
	options_block['key'] = 'options'
	options_block['param'] = [
		_make_param('id', 'top_block'),
		_make_param('title', title),
		_make_param('author', author),
		_make_param('description', desc),
		_make_param('window_size', window_size),
		_make_param('_coordinate', '(10, 10)'),
	]
	#append options block
	new_n['block'].append(options_block)
	############################################################
	# conversion - variables
	############################################################
	x = 100
	for variable in variables:
		key = variable['key']
		value = variable['value']
		minimum = utils.exists_or_else(variable, 'min', '')
		maximum = utils.exists_or_else(variable, 'max', '')
		step = utils.exists_or_else(variable, 'step', '')
		x = x + 150
		coor = '(%d, %d)'%(x, 10)
		var_block = odict()
		if minimum and maximum: #slider varible
			#determine num steps
			try: num_steps = str(int((float(maximum) - float(minimum))/float(step)))
			except: num_steps = '100'
			var_block['key'] = 'variable_slider'
			var_block['param'] = [
				_make_param('id', key),
				_make_param('value', value),
				_make_param('min', minimum),
				_make_param('max', maximum),
				_make_param('num_steps', num_steps),
				_make_param('_coordinate', coor),
			]
		else: #regular variable
			var_block['key'] = 'variable'
			var_block['param'] = [
				_make_param('id', key),
				_make_param('value', value),
				_make_param('_coordinate', coor),
			]
		#append variable block
		new_n['block'].append(var_block)
	############################################################
	# conversion - blocks
	############################################################
	#create name to key map for all blocks in platform
	name_to_key = dict((b.get_name(), b.get_key()) for b in platform.get_blocks())
	for block in blocks:
		#extract info
		tag = block['tag']
		#ignore list
		if tag in ('Note', 'About'): continue
		id = _convert_id(block['id'])
		coor = '(%s, %s + 100)'%(
			utils.exists_or_else(block, 'x_coordinate', '0'),
			utils.exists_or_else(block, 'y_coordinate', '0'),
		)
		rot = utils.exists_or_else(block, 'rotation', '0')
		params = _get_params(block)
		#new block
		new_block = odict()
		matches = difflib.get_close_matches(tag, name_to_key.keys(), 1)
		if not matches: continue
		#match found
		key = name_to_key[matches[0]]
		new_block['key'] = key
		new_block['param'] = [
			_make_param('id', id),
			_make_param('_coordinate', coor),
			_make_param('_rotation', rot),
		]
		#handle specific blocks
		if key == 'wxgui_fftsink2':
			params = params[0:3] + ['0'] + params[3:4] + ['8'] + params[4:]
		#append params
		for i, param in enumerate(params):
			platform_block = platform.get_block(key)
			try: platform_param = platform_block.get_params()[i+2]
			except IndexError: break
			if platform_param.is_enum():
				try: param_value = platform_param.get_option_keys()[int(param)]
				except: param_value = platform_param.get_option_keys()[0]
			else:
				param_value = param.replace('$', '').replace('^', '**')
			new_block['param'].append(_make_param(platform_param.get_key(), param_value))
		#append block
		new_n['block'].append(new_block)
	############################################################
	# conversion - connections
	############################################################
	for connection in connections:
		#extract info
		input_signal_block_id = connection['input_signal_block_id']
		input_socket_index = connection['input_socket_index']
		output_signal_block_id = connection['output_signal_block_id']
		output_socket_index = connection['output_socket_index']
		#new connection
		new_conn = odict()
		new_conn['source_block_id'] = _convert_id(output_signal_block_id)
		new_conn['sink_block_id'] = _convert_id(input_signal_block_id)
		new_conn['source_key'] = output_socket_index
		new_conn['sink_key'] = input_socket_index
		#append connection
		new_n['connection'].append(new_conn)
	############################################################
	# backup and replace
	############################################################
	#backup after successful conversion
	os.rename(file_path, file_path+'.bak')
	#save new flow graph to file path
	ParseXML.to_file({'flow_graph': new_n}, file_path)
