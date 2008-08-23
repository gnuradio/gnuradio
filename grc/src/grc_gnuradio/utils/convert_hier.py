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
##@package grc_gnuradio.utils.convert_hier
#Utility functions to convert a grc hier block to an xml wrapper
#@author Josh Blum

from grc_gnuradio.Constants import BLOCK_DTD
from grc import ParseXML
from grc.Utils import odict

def convert_hier(flow_graph, python_file):
	#extract info from the flow graph
	input_sig = flow_graph.get_input_signature()
	output_sig = flow_graph.get_output_signature()
	parameters = flow_graph.get_parameters()
	block_key = flow_graph.get_option('id')
	block_name = flow_graph.get_option('title')
	block_category = flow_graph.get_option('category')
	block_desc = flow_graph.get_option('description')
	block_author = flow_graph.get_option('author')
	#build the nested data
	block_n = odict()
	block_n['name'] = block_name
	block_n['key'] = block_key
	block_n['category'] = block_category
	block_n['import'] = 'execfile("%s")'%python_file
	#make data
	block_n['make'] = '%s(\n\t%s,\n)'%(
		block_key,
		',\n\t'.join(['%s=$%s'%(param.get_id(), param.get_id()) for param in parameters]),
	)
	#callback data
	block_n['callback'] = ['set_%s($%s)'%(param.get_id(), param.get_id()) for param in parameters]
	#param data
	params_n = list()
	for param in parameters:
		param_n = odict()
		param_n['name'] = param.get_param('label').get_value() or param.get_id()
		param_n['key'] = param.get_id()
		param_n['value'] = param.get_param('value').get_value()
		param_n['type'] = 'raw'
		params_n.append(param_n)
	block_n['param'] = params_n
	#sink data
	if int(input_sig['nports']):
		sink_n = odict()
		sink_n['name'] = 'in'
		sink_n['type'] = input_sig['type']
		sink_n['vlen'] = input_sig['vlen']
		sink_n['nports'] = input_sig['nports']
		block_n['sink'] = sink_n
	#source data
	if int(output_sig['nports']):
		source_n = odict()
		source_n['name'] = 'out'
		source_n['type'] = output_sig['type']
		source_n['vlen'] = output_sig['vlen']
		source_n['nports'] = output_sig['nports']
		block_n['source'] = source_n
	#doc data
	block_n['doc'] = "%s\n%s\n%s"%(block_author, block_desc, python_file)
	#write the block_n to file
	xml_file = python_file + '.xml'
	ParseXML.to_file({'block': block_n}, xml_file)
	ParseXML.validate_dtd(xml_file, BLOCK_DTD)
