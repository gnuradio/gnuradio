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
from Cheetah.Template import Template
import expr_utils
from Constants import \
    TOP_BLOCK_FILE_MODE, HIER_BLOCK_FILE_MODE, HIER_BLOCKS_LIB_DIR, FLOW_GRAPH_TEMPLATE, XTERM_EXECUTABLE
import convert_hier
from .. gui import Messages


class Generator(object):

    def __init__(self, flow_graph, file_path):
        """
        Initialize the generator object.
        Determine the file to generate.
        
        Args:
            flow_graph: the flow graph object
            file_path: the path to write the file to
        """
        self._flow_graph = flow_graph
        self._generate_options = self._flow_graph.get_option('generate_options')
        if self._generate_options == 'hb':
            self._mode = HIER_BLOCK_FILE_MODE
            dirname = HIER_BLOCKS_LIB_DIR
        else:
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
        throttled = any(map(lambda b: b.throttle(), self._flow_graph.get_enabled_blocks()))
        if not throttled and self._generate_options != 'hb':
            Messages.send_warning('''\
This flow graph may not have flow control: no audio or usrp blocks found. \
Add a Misc->Throttle block to your flow graph to avoid CPU congestion.''')
        #generate
        open(self.get_file_path(), 'w').write(str(self))
        if self._generate_options == 'hb':
            #convert hier block to xml wrapper
            convert_hier.convert_hier(self._flow_graph, self.get_file_path())
        os.chmod(self.get_file_path(), self._mode)

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
        cmds = [python_exe, '-u', self.get_file_path()] #-u is unbuffered stdio

        #when in no gui mode on linux, use an xterm (looks nice)
        if self._generate_options == 'no_gui' and 'linux' in sys.platform.lower():
            cmds = [XTERM_EXECUTABLE, '-e'] + cmds

        p = subprocess.Popen(args=cmds, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=False, universal_newlines=True)
        return p

    def __str__(self):
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
            try: code += block.get_param('notebook').get_value() #older gui markup w/ wxgui
            except: pass
            try: code += block.get_param('gui_hint').get_value() #newer gui markup w/ qtgui
            except: pass
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
