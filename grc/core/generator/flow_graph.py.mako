% if not generate_options.startswith('hb'):
<%
from sys import version_info
from gnuradio import eng_notation
python_version = version_info.major
%>\
% if python_version == 2:
#!/usr/bin/env python2
% elif python_version == 3:
#!/usr/bin/env python3
% endif
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
from distutils.version import StrictVersion

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

class ${class_name}(gr.top_block, Qt.QWidget):

    def __init__(${param_str}):
        gr.top_block.__init__(self, "${title}")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("${title}")
        qtgui.util.check_set_qss()
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

class ${class_name}(gr.top_block):
    def __init__(self, doc):
        gr.top_block.__init__(self, "${title}")
        self.doc = doc
        self.plot_lst = []
        self.widget_lst = []
% elif generate_options == 'no_gui':

class ${class_name}(gr.top_block):

    def __init__(${param_str}):
        gr.top_block.__init__(self, "${title}")
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
gr.io_signaturev(${len(io_sigs)}, ${len(io_sigs)}, [${', '.join(size_strs)}])\
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
##         % if 'alias' in blk.params and blk.params['alias'].get_evaluated():
##         (self.${blk.name}).set_block_alias("${blk.params['alias'].get_evaluated()}")
##         % endif
##         % if 'affinity' in blk.params and blk.params['affinity'].get_evaluated():
##         (self.${blk.name}).set_processor_affinity(${blk.params['affinity'].get_evaluated()})
##         % endif
##         % if len(blk.sources) > 0 and 'minoutbuf' in blk.params and int(blk.params['minoutbuf'].get_evaluated()) > 0:
##         (self.${blk.name}).set_min_output_buffer(${blk.params['minoutbuf'].get_evaluated()})
##         % endif
##         % if len(blk.sources) > 0 and 'maxoutbuf' in blk.params and int(blk.params['maxoutbuf'].get_evaluated()) > 0:
##         (self.${blk.name}).set_max_output_buffer(${blk.params['maxoutbuf'].get_evaluated()})
##         % endif
        % endfor

##########################################################
## Create a layout entry if not manually done for BokehGUI
##########################################################
% if generate_options == 'bokeh_gui':
        if self.widget_lst:
            input_t = bokehgui.BokehLayout.widgetbox(self.widget_lst)
            widgetbox = bokehgui.BokehLayout.WidgetLayout(input_t)
            widgetbox.set_layout(*(${flow_graph.get_option('placement')}))
            list_obj = [widgetbox] + self.plot_lst
        else:
            list_obj = self.plot_lst
        layout_t = bokehgui.BokehLayout.create_layout(list_obj, "${flow_graph.get_option('sizing_mode')}")
        self.doc.add_root(layout_t)
% endif

        % if connections:

        ${'##################################################'}
        # Connections
        ${'##################################################'}
        % for connection in connections:
        ${ connection.rstrip() }
        % endfor
        % endif
########################################################
## QT sink close method reimplementation
########################################################
% if generate_options == 'qt_gui':

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "${class_name}")
        self.settings.setValue("geometry", self.saveGeometry())
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
########################################################
##Create Main
##  For top block code, generate a main routine.
##  Instantiate the top block and run as gui or cli.
########################################################
<%def name="make_default(type_, param)">
    % if type_ == 'eng_float':
eng_notation.num_to_str(float(${param.templates.render('make')}))
    % else:
${param.templates.render('make')}
    % endif
</%def>\
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
            default = '"' + eng_notation.num_to_str(float(default)) + '"'
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
        print("Error: failed to enable real-time scheduling.")
    % endif
    % if generate_options == 'qt_gui':

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(${ ', '.join(params_eq_list) })
    % if flow_graph.get_option('run'):
    tb.start(${flow_graph.get_option('max_nouts') or ''})
    % endif
    % if flow_graph.get_option('qt_qss_theme'):
    tb.setStyleSheetFromFile(${ flow_graph.get_option('qt_qss_theme') })
    % endif
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.aboutToQuit.connect(quitting)
    % for m in monitors:
    if 'en' in m.params:
        if m.params['en'].get_value():
            (tb.${m.name}).start()
    else:
        sys.stderr.write("Monitor '{0}' does not have an enable ('en') parameter.".format("tb.${m.name}"))
    % endfor
    qapp.exec_()
    % elif generate_options == 'bokeh_gui':
    serverProc, port = bokehgui.utils.create_server()
    def killProc(signum, frame, tb):
        tb.stop()
        tb.wait()
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
        tb = top_block_cls(doc)
        try:
            tb.start()
            signal.signal(signal.SIGTERM, functools.partial(killProc, tb=tb))
            session.loop_until_closed()
        finally:
            print("Exiting the simulation. Stopping Bokeh Server")
            tb.stop()
            tb.wait()
    finally:
        serverProc.terminate()
        serverProc.kill()
    % elif generate_options == 'no_gui':
    tb = top_block_cls(${ ', '.join(params_eq_list) })
    % if flow_graph.get_option('run_options') == 'prompt':
    tb.start(${ flow_graph.get_option('max_nouts') or '' })
    % for m in monitors:
    (tb.${m.name}).start()
    % endfor
    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    % elif flow_graph.get_option('run_options') == 'run':
    tb.start(${flow_graph.get_option('max_nouts') or ''})
    % endif
    % for m in monitors:
    (tb.${m.name}).start()
    % endfor
    tb.wait()
    % endif


if __name__ == '__main__':
    main()
% endif
