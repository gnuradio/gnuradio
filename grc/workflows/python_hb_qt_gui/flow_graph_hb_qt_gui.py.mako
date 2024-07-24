# -*- coding: utf-8 -*-
<%def name="indent(code)">${ '\n        '.join(str(code).splitlines()) }</%def>
#
# SPDX-License-Identifier: GPL-3.0
#
##################################################
# GNU Radio Python Flow Graph
# Title: ${title}
% if flow_graph.get_option('author'):
# Author: ${flow_graph.get_option('author')}
% endif
% if flow_graph.get_option('copyright'):
# Copyright: ${flow_graph.get_option('copyright')}
% endif
% if flow_graph.get_option('description'):
# Description: ${flow_graph.get_option('description')}
% endif
# GNU Radio version: ${version}
##################################################

########################################################
##Create Imports
########################################################
from PyQt5 import Qt
from gnuradio import qtgui
% for imp in imports:
##${imp.replace("  # grc-generated hier_block", "")}
${imp}
% endfor
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal

########################################################
##Prepare snippets
########################################################
% for snip in flow_graph.get_snippets_dict():

${indent(snip['def'])}
% for line in snip['lines']:
    ${indent(line)}
% endfor
% endfor
\
<%
snippet_sections = ['main_after_init', 'main_after_start', 'main_after_stop', 'init_before_blocks']
snippets = {}
for section in snippet_sections:
    snippets[section] = flow_graph.get_snippets_dict(section)
%>
\
%for section in snippet_sections:
%if snippets[section]:

def snippets_${section}(tb):
    % for snip in snippets[section]:
    ${indent(snip['call'])}
    % endfor
%endif
%endfor

########################################################
##Create Class
##  Write the class declaration for a top or hier block.
##  The parameter names are the arguments to __init__.
##  Setup the IO signature (hier block only).
########################################################
<%
    class_name = flow_graph.get_option('id')
    param_str = ', '.join(['self'] + ['%s=%s'%(param.name, param.templates.render('make')) for param in parameters])
%>\
    <% in_sigs = flow_graph.get_hier_block_stream_io('in') %>
    <% out_sigs = flow_graph.get_hier_block_stream_io('out') %>


class ${class_name}(gr.hier_block2, Qt.QWidget):
<%def name="make_io_sig(io_sigs)">\
    <% size_strs = ['%s*%s'%(io_sig['size'], io_sig['vlen']) for io_sig in io_sigs] %>\
    % if len(io_sigs) == 0:
gr.io_signature(0, 0, 0)\
    % elif len(io_sigs) == 1:
gr.io_signature(1, 1, ${size_strs[0]})\
    % else:
gr.io_signature.makev(${len(io_sigs)}, ${len(io_sigs)}, [${', '.join(size_strs)}])\
    % endif
</%def>\
    def __init__(${param_str}):
        gr.hier_block2.__init__(
            self, "${ title }",
            ${make_io_sig(in_sigs)},
            ${make_io_sig(out_sigs)},
        )
    % for pad in flow_graph.get_hier_block_message_io('in'):
        self.message_port_register_hier_in("${ pad['label'] }")
    % endfor
    % for pad in flow_graph.get_hier_block_message_io('out'):
        self.message_port_register_hier_out("${ pad['label'] }")
    % endfor

        Qt.QWidget.__init__(self)
        self.top_layout = Qt.QVBoxLayout()
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)
        self.setLayout(self.top_layout)
% if flow_graph.get_option('thread_safe_setters'):

        self._lock = threading.RLock()
% endif
########################################################
##Create Parameters
##  Set the parameter to a property of self.
########################################################
% if parameters:

        ${'##################################################'}
        # Parameters
        ${'##################################################'}
% endif
% for param in parameters:
        ${indent(param.get_var_make())}
% endfor
########################################################
##Create Variables
########################################################
% if variables:

        ${'##################################################'}
        # Variables
        ${'##################################################'}
% endif
% for var in variables:
        ${indent(var.templates.render('var_make'))}
% endfor
        % if blocks:

        ${'##################################################'}
        # Blocks
        ${'##################################################'}
        % endif
        ${'snippets_init_before_blocks(self)' if snippets['init_before_blocks'] else ''}
        % for blk, blk_make in blocks:
        % if blk_make:
        ${ indent(blk_make.strip('\n')) }
        % endif
        % if 'alias' in blk.params and blk.params['alias'].get_evaluated():
        self.${blk.name}.set_block_alias("${blk.params['alias'].get_evaluated()}")
        % endif
        % if 'affinity' in blk.params and blk.params['affinity'].get_evaluated():
        self.${blk.name}.set_processor_affinity(${blk.params['affinity'].to_code()})
        % endif
        % if len(blk.sources) > 0 and 'minoutbuf' in blk.params and int(blk.params['minoutbuf'].get_evaluated()) > 0:
        self.${blk.name}.set_min_output_buffer(${blk.params['minoutbuf'].to_code()})
        % endif
        % if len(blk.sources) > 0 and 'maxoutbuf' in blk.params and int(blk.params['maxoutbuf'].get_evaluated()) > 0:
        self.${blk.name}.set_max_output_buffer(${blk.params['maxoutbuf'].to_code()})
        % endif
        % endfor

        % if connections:

        ${'##################################################'}
        # Connections
        ${'##################################################'}
        % for connection in connections:
        ${ indent(connection.rstrip()) }
        % endfor
        % endif

########################################################
## Create Callbacks
## Write a set method for this variable that calls the callbacks
########################################################
    % for var in parameters + variables:

    def get_${ var.name }(self):
        return self.${ var.name }

    def set_${ var.name }(self, ${ var.name }):
        % if flow_graph.get_option('thread_safe_setters'):
        with self._lock:
            self.${ var.name } = ${ var.name }
            % for callback in callbacks[var.name]:
            ${ indent(callback) }
            % endfor
        % else:
        self.${ var.name } = ${ var.name }
        % for callback in callbacks[var.name]:
        ${ indent(callback) }
        % endfor
        % endif
    % endfor
\

