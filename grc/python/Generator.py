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

import os
import sys
import subprocess
import tempfile
from distutils.spawn import find_executable
from Cheetah.Template import Template

from .. gui import Messages
from .. base import ParseXML
from .. base import odict

from . Constants import TOP_BLOCK_FILE_MODE, FLOW_GRAPH_TEMPLATE, \
    XTERM_EXECUTABLE, HIER_BLOCK_FILE_MODE, HIER_BLOCKS_LIB_DIR, BLOCK_DTD
from . import expr_utils


class Generator(object):
    def __init__(self, flow_graph, file_path):
        """
        Initialize the generator object.
        Determine the file to generate.

        Args:
            flow_graph: the flow graph object
            file_path: the path to the grc file
        """
        self._generate_options = flow_graph.get_option('generate_options')
        if self._generate_options == 'hb':
            self._generator = HierBlockGenerator(flow_graph, file_path)
        else:
            self._generator = TopBlockGenerator(flow_graph, file_path)

    def get_generate_options(self):
        return self._generate_options

    def __getattr__(self, item):
        return getattr(self._generator, item)


class TopBlockGenerator(object):

    def __init__(self, flow_graph, file_path):
        """
        Initialize the generator object.

        Args:
            flow_graph: the flow graph object
            file_path: the path to write the file to
        """
        self._flow_graph = flow_graph
        self._generate_options = self._flow_graph.get_option('generate_options')
        self._mode = TOP_BLOCK_FILE_MODE
        dirname = os.path.dirname(file_path)
        #handle the case where the directory is read-only
        #in this case, use the system's temp directory
        if not os.access(dirname, os.W_OK):
            dirname = tempfile.gettempdir()
        filename = self._flow_graph.get_option('id') + '.py'
        self._file_path = os.path.join(dirname, filename)

    def get_file_path(self): return self._file_path

    def write(self):
        #do throttle warning
        throttling_blocks = filter(lambda b: b.throttle(), self._flow_graph.get_enabled_blocks())
        if not throttling_blocks and self._generate_options != 'hb':
            Messages.send_warning("This flow graph may not have flow control: "
                                  "no audio or RF hardware blocks found. "
                                  "Add a Misc->Throttle block to your flow "
                                  "graph to avoid CPU congestion.")
        if len(throttling_blocks) > 1:
            keys = set(map(lambda b: b.get_key(), throttling_blocks))
            if len(keys) > 1 and 'blocks_throttle' in keys:
                Messages.send_warning("This flow graph contains a throttle "
                                      "block and another rate limiting block, "
                                      "e.g. a hardware source or sink. "
                                      "This is usually undesired. Consider "
                                      "removing the throttle block.")
        #generate
        open(self.get_file_path(), 'w').write(
            self._build_python_code_from_template()
        )

    def get_popen(self):
        """
        Execute this python flow graph.

        Returns:
            a popen object
        """
        #extract the path to the python executable
        python_exe = sys.executable

        #when using wx gui on mac os, execute with pythonw
        #using pythonw is not necessary anymore, disabled below
        #if self._generate_options == 'wx_gui' and 'darwin' in sys.platform.lower():
        #   python_exe = 'pythonw'

        #setup the command args to run
        cmds = [python_exe, '-u', self.get_file_path()]  # -u is unbuffered stdio

        # when in no gui mode on linux, use a graphical terminal (looks nice)
        xterm_executable = find_executable(XTERM_EXECUTABLE)
        if self._generate_options == 'no_gui' and xterm_executable:
            cmds = [xterm_executable, '-e'] + cmds

        p = subprocess.Popen(
            args=cmds, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            shell=False, universal_newlines=True)
        return p

    def _build_python_code_from_template(self):
        """
        Convert the flow graph to python code.

        Returns:
            a string of python code
        """
        title = self._flow_graph.get_option('title') or self._flow_graph.get_option('id').replace('_', ' ').title()
        imports = self._flow_graph.get_imports()
        variables = self._flow_graph.get_variables()
        parameters = self._flow_graph.get_parameters()
        monitors = self._flow_graph.get_monitors()
        #list of blocks not including variables and imports and parameters and disabled
        def _get_block_sort_text(block):
            code = block.get_make().replace(block.get_id(), ' ')
            try:
                code += block.get_param('notebook').get_value() #older gui markup w/ wxgui
            except:
                pass
            try:
                code += block.get_param('gui_hint').get_value() #newer gui markup w/ qtgui
            except:
                pass
            return code
        blocks = expr_utils.sort_objects(
            self._flow_graph.get_enabled_blocks(),
            lambda b: b.get_id(), _get_block_sort_text
        )
        #list of regular blocks (all blocks minus the special ones)
        blocks = filter(lambda b: b not in (imports + parameters), blocks)
        #list of connections where each endpoint is enabled
        connections = filter(lambda c: not (c.is_bus() or c.is_msg() or c.is_message()), self._flow_graph.get_enabled_connections())
        messages = filter(lambda c: c.is_msg(), self._flow_graph.get_enabled_connections())
        messages2 = filter(lambda c: c.is_message(), self._flow_graph.get_enabled_connections())
        #list of variable names
        var_ids = [var.get_id() for var in parameters + variables]
        #prepend self.
        replace_dict = dict([(var_id, 'self.%s'%var_id) for var_id in var_ids])
        #list of callbacks
        callbacks = [
            expr_utils.expr_replace(cb, replace_dict)
            for cb in sum([block.get_callbacks() for block in self._flow_graph.get_enabled_blocks()], [])
        ]
        #map var id to callbacks
        var_id2cbs = dict(
            [(var_id, filter(lambda c: expr_utils.get_variable_dependencies(c, [var_id]), callbacks))
            for var_id in var_ids]
        )
        #load the namespace
        namespace = {
            'title': title,
            'imports': imports,
            'flow_graph': self._flow_graph,
            'variables': variables,
            'parameters': parameters,
            'monitors': monitors,
            'blocks': blocks,
            'connections': connections,
            'messages': messages,
            'messages2': messages2,
            'generate_options': self._generate_options,
            'var_id2cbs': var_id2cbs,
        }
        #build the template
        t = Template(open(FLOW_GRAPH_TEMPLATE, 'r').read(), namespace)
        return str(t)


class HierBlockGenerator(TopBlockGenerator):

    def __init__(self, flow_graph, file_path):
        TopBlockGenerator.__init__(self, flow_graph, file_path)

        self._mode = HIER_BLOCK_FILE_MODE

        dirname = HIER_BLOCKS_LIB_DIR
        filename = self._flow_graph.get_option('id')

        self._file_path = os.path.join(dirname, filename + '.py')
        self._file_path_xml = self._file_path + '.xml'

    def get_file_path(self): return self._file_path
    def get_file_path_xml(self): return self._file_path_xml

    def write(self):
        TopBlockGenerator.write(self)
        ParseXML.to_file(self._build_block_n_from_flow_graph_io(), self.get_file_path_xml())
        ParseXML.validate_dtd(self.get_file_path_xml(), BLOCK_DTD)

    def _build_block_n_from_flow_graph_io(self):
        flow_graph = self._flow_graph
        python_file = self.get_file_path()

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
            sink_n['vlen'] = var_or_value(input_sig['vlen'], parameters)
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
            source_n['vlen'] = var_or_value(output_sig['vlen'], parameters)
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
        n = {'block': block_n}
        return n


def var_or_value(name, parameters):
    if name in map(lambda p: p.get_id(), parameters):
        return "$"+name
    return name

