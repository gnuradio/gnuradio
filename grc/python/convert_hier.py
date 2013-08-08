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

from Constants import BLOCK_DTD
from .. base import ParseXML
from .. base import odict

def convert_hier(flow_graph, python_file):
    #extract info from the flow graph
    input_sigs = flow_graph.get_io_signaturev('in')
    output_sigs = flow_graph.get_io_signaturev('out')
    input_msgp = flow_graph.get_msg_pad_sources();
    output_msgp = flow_graph.get_msg_pad_sinks();
    parameters = flow_graph.get_parameters()
    bussink = flow_graph.get_bussink()
    bussrc = flow_graph.get_bussrc()
    bus_struct_sink = flow_graph.get_bus_structure_sink()
    bus_struct_src = flow_graph.get_bus_structure_src()
    block_key = flow_graph.get_option('id')
    block_name = flow_graph.get_option('title') or flow_graph.get_option('id').replace('_', ' ').title()
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
    if parameters: block_n['make'] = '%s(\n    %s,\n)'%(
        block_key,
        ',\n    '.join(['%s=$%s'%(param.get_id(), param.get_id()) for param in parameters]),
    )
    else: block_n['make'] = '%s()'%block_key
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
    #sink data stream ports
    if bussink:
        block_n['bus_sink'] = '1';
    if bussrc:
        block_n['bus_source'] = '1';
    block_n['sink'] = list()
    for input_sig in input_sigs:
        sink_n = odict()
        sink_n['name'] = input_sig['label']
        sink_n['type'] = input_sig['type']
        sink_n['vlen'] = input_sig['vlen']
        if input_sig['optional']: sink_n['optional'] = '1'
        block_n['sink'].append(sink_n)
    #sink data msg ports
    for input_sig in input_msgp:
        sink_n = odict()
        sink_n['name'] = input_sig.get_param("label").get_value();
        sink_n['type'] = "message"
        sink_n['optional'] = input_sig.get_param("optional").get_value();
        block_n['sink'].append(sink_n)
    #source data stream ports
    block_n['source'] = list()
    if bus_struct_sink:
        block_n['bus_structure_sink'] = bus_struct_sink[0].get_param('struct').get_value();
    if bus_struct_src:
        block_n['bus_structure_source'] = bus_struct_src[0].get_param('struct').get_value();
    for output_sig in output_sigs:
        source_n = odict()
        source_n['name'] = output_sig['label']
        source_n['type'] = output_sig['type']
        source_n['vlen'] = output_sig['vlen']
        if output_sig['optional']: source_n['optional'] = '1'
        block_n['source'].append(source_n)
    #source data msg ports
    for output_sig in output_msgp:
        source_n = odict()
        source_n['name'] = output_sig.get_param("label").get_value();
        source_n['type'] = "message"
        source_n['optional'] = output_sig.get_param("optional").get_value();
        block_n['source'].append(source_n)
    #doc data
    block_n['doc'] = "%s\n%s\n%s"%(block_author, block_desc, python_file)
    block_n['grc_source'] = "%s"%(flow_graph.grc_file_path)
    #write the block_n to file
    xml_file = python_file + '.xml'
    ParseXML.to_file({'block': block_n}, xml_file)
    ParseXML.validate_dtd(xml_file, BLOCK_DTD)
