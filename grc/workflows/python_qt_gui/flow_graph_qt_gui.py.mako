#!/usr/bin/env python3
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
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation


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
class ${class_name}(gr.top_block, Qt.QWidget):

    def __init__(${param_str}):
        gr.top_block.__init__(self, "${title}", catch_exceptions=${catch_exceptions})
        Qt.QWidget.__init__(self)
        self.setWindowTitle("${title}")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except BaseException as exc:
            print(f"Qt GUI: Could not set Icon: {str(exc)}", file=sys.stderr)
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("gnuradio/flowgraphs", "${class_name}")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)
% if flow_graph.get_option('thread_safe_setters'):

        self._lock = threading.RLock()
% endif
        self.flowgraph_started = threading.Event()
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
## QT sink close method reimplementation
########################################################

    def closeEvent(self, event):
        self.settings = Qt.QSettings("gnuradio/flowgraphs", "${class_name}")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()
        ${'snippets_main_after_stop(self)' if snippets['main_after_stop'] else ''}
        event.accept()
    % if flow_graph.get_option('qt_qss_theme'):

    def setStyleSheetFromFile(self, filename):
        try:
            if not os.path.exists(filename):
                filename = os.path.join(
                    gr.prefix(), "share", "gnuradio", "themes", filename)
            with open(filename) as ss:
                self.setStyleSheet(ss.read())
        except Exception as e:
            self.logger.error(f"setting stylesheet: {str(e)}")
    % endif
##
##
##
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

########################################################
##Create Main
##  For top block code, generate a main routine.
##  Instantiate the top block and run as gui or cli.
########################################################
<% params_eq_list = list() %>
% if parameters:

<% arg_parser_args = '' %>\
def argument_parser():
    % if flow_graph.get_option('description'):
    <%
        arg_parser_args = 'description=description'
    %>description = ${repr(flow_graph.get_option('description'))}
    % endif
    parser = ArgumentParser(${arg_parser_args})
    % for param in parameters:
<%
        switches = ['"--{}"'.format(param.name.replace('_', '-'))]
        short_id = param.params['short_id'].get_value()
        if short_id:
            switches.insert(0, '"-{}"'.format(short_id))

        type_ = param.params['type'].get_value()
        if type_:
            params_eq_list.append('%s=options.%s' % (param.name, param.name))

        default = param.templates.render('make')
        if type_ == 'eng_float':
            default = "eng_notation.num_to_str(float(" + default + "))"
        # FIXME:
        if type_ == 'string':
            type_ = 'str'
    %>\
    % if type_:
    parser.add_argument(
        ${ ', '.join(switches) }, dest="${param.name}", type=${type_}, default=${ default },
        help="Set ${param.params['label'].get_evaluated() or param.name} [default=%(default)r]")
    % endif
    % endfor
    return parser
% endif


def main(top_block_cls=${class_name}, options=None):
    % if parameters:
    if options is None:
        options = argument_parser().parse_args()
    % endif
    % if flow_graph.get_option('realtime_scheduling'):
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        gr.logger("realtime").warn("Error: failed to enable real-time scheduling.")
    % endif

    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(${ ', '.join(params_eq_list) })
    ${'snippets_main_after_init(tb)' if snippets['main_after_init'] else ''}
    % if flow_graph.get_option('run'):
    tb.start(${flow_graph.get_option('max_nouts') or ''})
    tb.flowgraph_started.set()
    % endif
    ${'snippets_main_after_start(tb)' if snippets['main_after_start'] else ''}
    % if flow_graph.get_option('qt_qss_theme'):
    tb.setStyleSheetFromFile("${ flow_graph.get_option('qt_qss_theme') }")
    % endif
    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()
        ${'snippets_main_after_stop(tb)' if snippets['main_after_stop'] else ''}
        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    % for m in monitors:
    % if m.params['en'].get_value() == 'True':
    tb.${m.name}.start()
    % endif
    % endfor
    qapp.exec_()

if __name__ == '__main__':
    main()
