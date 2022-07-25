% if not generate_options.startswith('hb'):
#!/usr/bin/env python3
% endif
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

% if generate_options == 'qt_gui':
from packaging.version import Version as StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

% endif
########################################################
##Create Imports
########################################################
% for imp in imports:
##${imp.replace("  # grc-generated hier_block", "")}
${imp}
% endfor

% if generate_options == 'pyqtgraph':
import pyqtgraph as pg
from pyqtgraph.Qt import QtGui
from pyqtgraph.dockarea import *
from gnuradio.pyqtgraph.numpy import *
% endif

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
snippet_sections = ['main_after_init', 'main_after_start', 'main_after_stop']
snippets = {}
for section in snippet_sections:
    snippets[section] = flow_graph.get_snippets_dict(section)
%>
\
%for section in snippet_sections:
%if snippets[section]:

def snippets_${section}(fg, rt=None):
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
% if generate_options == 'qt_gui':
from gnuradio import qtgui

class ${class_name}(Qt.QWidget):
    def start(self):
        self.fg.start()

    def stop(self):
        self.fg.stop()
        
    def wait(self):
        self.fg.wait()
    
    def connect(self,*args):
        return self.fg.connect(*args)

    def msg_connect(self,*args):
        return self.fg.connect(*args)

    def __init__(${param_str}):
        self.fg = gr.flowgraph("${title}")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("${title}")
        ## qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
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

        self.settings = Qt.QSettings("GNU Radio", "${class_name}")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass
% elif generate_options == 'bokeh_gui':

class ${class_name}(gr.flowgraph):
    def __init__(self, doc):
        gr.flowgraph.__init__(self, "${title}")
        self.doc = doc
        self.plot_lst = []
        self.widget_lst = []
% elif generate_options == 'pyqtgraph':

class ${class_name}(gr.flowgraph):
    def __init__(self):
        gr.flowgraph.__init__(self, "${title}")
        self.win = QtGui.QMainWindow()
        self.area = DockArea()
        self.win.setCentralWidget(self.area)
        self.win.resize(1000,500)
        self.win.setWindowTitle("${title}")
        self.docks = []

% elif generate_options == 'no_gui':

class ${class_name}(gr.flowgraph):

    def __init__(${param_str}):
        gr.flowgraph.__init__(self, "${title}")
% elif generate_options.startswith('hb'):
    <% in_sigs = flow_graph.get_hier_block_stream_io('in') %>
    <% out_sigs = flow_graph.get_hier_block_stream_io('out') %>


% if generate_options == 'hb_qt_gui':
class ${class_name}(gr.hier_block2, Qt.QWidget):
% else:
class ${class_name}(gr.hier_block2):
% endif
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
    % if generate_options == 'hb_qt_gui':

        Qt.QWidget.__init__(self)
        self.top_layout = Qt.QVBoxLayout()
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)
        self.setLayout(self.top_layout)
    % endif
% endif
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
        % for blk, blk_make in blocks:
        % if blk_make:
        ${ indent(blk_make.strip('\n')) }
        % endif
        % if 'alias' in blk.params and blk.params['alias'].get_evaluated():
        self.${blk.name}.set_block_alias("${blk.params['alias'].get_evaluated()}")
        % endif
        % if 'affinity' in blk.params and blk.params['affinity'].get_evaluated():
        self.${blk.name}.set_processor_affinity(${blk.params['affinity'].get_evaluated()})
        % endif
        % if len(blk.sources) > 0 and 'minoutbuf' in blk.params and int(blk.params['minoutbuf'].get_evaluated()) > 0:
        self.${blk.name}.set_min_output_buffer(${blk.params['minoutbuf'].get_evaluated()})
        % endif
        % if len(blk.sources) > 0 and 'maxoutbuf' in blk.params and int(blk.params['maxoutbuf'].get_evaluated()) > 0:
        self.${blk.name}.set_max_output_buffer(${blk.params['maxoutbuf'].get_evaluated()})
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
% if generate_options == 'qt_gui':

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "${class_name}")
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
            print(e, file=sys.stderr)
    % endif
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
% if not generate_options.startswith('hb'):
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


def main(flowgraph_cls=${class_name}, options=None):
    % if parameters:
    if options is None:
        options = argument_parser().parse_args()
    % endif
    % if flow_graph.get_option('realtime_scheduling'):
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print("Error: failed to enable real-time scheduling.")
    % endif
    % if generate_options == 'pyqtgraph':
    app = pg.mkQApp('DockArea Example')
    % if flow_graph.get_option('pyqtgraph_antialiasing'):
    pg.setConfigOptions(antialias=True)
    % endif
    fg = flowgraph_cls(${ ', '.join(params_eq_list) })
    fg.start()
    fg.win.show()
    app.exec()
    % endif
    % if generate_options == 'qt_gui':

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    fg = flowgraph_cls(${ ', '.join(params_eq_list) })
    rt = gr.runtime()
    ${'snippets_main_after_init(fg, rt)' if snippets['main_after_init'] else ''}
    % if flow_graph.get_option('run'):
    rt.initialize(fg.fg)
    rt.start(${flow_graph.get_option('max_nouts') or ''})
    % endif
    ${'snippets_main_after_start(fg, rt)' if snippets['main_after_start'] else ''}
    % if flow_graph.get_option('qt_qss_theme'):
    fg.setStyleSheetFromFile("${ flow_graph.get_option('qt_qss_theme') }")
    % endif
    fg.show()

    def sig_handler(sig=None, frame=None):
        rt.stop()
        rt.wait()
        ${'snippets_main_after_stop(fg, rt)' if snippets['main_after_stop'] else ''}
        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    % for m in monitors:
    % if m.params['en'].get_value() == 'True':
    fg.${m.name}.start()
    % endif
    % endfor
    qapp.exec_()
    % elif generate_options == 'bokeh_gui':
    serverProc, port = bokehgui.utils.create_server()
    def killProc(signum, frame, fg):
        fg.stop()
        fg.wait()
        ${'snippets_main_after_stop(fg)' if snippets['main_after_stop'] else ''}
        serverProc.terminate()
        serverProc.kill()
    time.sleep(1)
    try:
        # Define the document instance
        doc = curdoc()
        % if flow_graph.get_option('author'):
        doc.title = "${title} - ${flow_graph.get_option('author')}"
        % else:
        doc.title = "${title}"
        % endif
        session = push_session(doc, session_id="${flow_graph.get_option('id')}",
                               url = "http://localhost:" + port + "/bokehgui")
        # Create Top Block instance
        fg = flowgraph_cls(doc)
        ${'snippets_main_after_init(fg)' if snippets['main_after_init'] else ''}
        try:
            fg.start()
            ${'snippets_main_after_start(fg)' if snippets['main_after_start'] else ''}
            signal.signal(signal.SIGTERM, functools.partial(killProc, fg=fg))
            session.loop_until_closed()
        finally:
            print("Exiting the simulation. Stopping Bokeh Server")
            fg.stop()
            fg.wait()
            ${'snippets_main_after_stop(fg, rt)' if snippets['main_after_stop'] else ''}
    finally:
        serverProc.terminate()
        serverProc.kill()
    % elif generate_options == 'no_gui':
    fg = flowgraph_cls(${ ', '.join(params_eq_list) })
    rt = gr.runtime()
    ${'snippets_main_after_init(fg, rt)' if snippets['main_after_init'] else ''}
    
    rt.initialize(fg)

    % if flow_graph.get_option('run_options') == 'prompt':
    rt.start(${ flow_graph.get_option('max_nouts') or '' })
    ${'snippets_main_after_start(fg, rt)' if snippets['main_after_start'] else ''}
    % for m in monitors:
    % if m.params['en'].get_value() == 'True':
    rt.${m.name}.start()
    % endif
    % endfor
    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    rt.stop()
    ## ${'snippets_main_after_stop(fg)' if snippets['main_after_stop'] else ''}
    % elif flow_graph.get_option('run_options') == 'run':
    rt.start(${flow_graph.get_option('max_nouts') or ''})
    ${'snippets_main_after_start(fg, rt)' if snippets['main_after_start'] else ''}
    % for m in monitors:
    % if m.params['en'].get_value() == 'True':
    fg.${m.name}.start()
    % endif
    % endfor
    % endif
    try:
        rt.wait()
    except KeyboardInterrupt:
        rt.stop()
        rt.wait()
    ${'snippets_main_after_stop(fg, rt)' if snippets['main_after_stop'] else ''}
    % for m in monitors:
    % if m.params['en'].get_value() == 'True':
    rt.${m.name}.stop()
    % endif
    % endfor
    % endif

if __name__ == '__main__':
    main()
% endif
