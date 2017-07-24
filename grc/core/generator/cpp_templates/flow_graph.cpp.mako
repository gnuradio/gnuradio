/********************
GNU Radio C++ Flow Graph Source File

Title: ${title}
% if flow_graph.get_option('author'):
Author: ${flow_graph.get_option('author')}
% endif
% if flow_graph.get_option('description'):
Description: ${flow_graph.get_option('description')}
% endif
Generated: ${generated_time}
********************/

#include "${flow_graph.get_option('id')}.hpp"

using namespace gr;

<%
	class_name = flow_graph.get_option('id')
## TODO: param_str
%>\
<%def name="indent(code)">${ '\n        '.join(str(code).splitlines()) }</%def>

## TODO: param_str
% if generate_options == 'no_gui':
${class_name}::${class_name} () : top_block("${title}") {
% elif generate_options.startswith('hb'):
## TODO: make_io_sig
${class_name}::${class_name} () : hier_block2("${title}") {
% for pad in flow_graph.get_hier_block_message_io('in'):
    message_port_register_hier_in("${pad['label']}")
% endfor
% for pad in flow_graph.get_hier_block_message_io('out'):
    message_port_register_hier_out("${pad['label']}")
% endfor
% elif generate_options == 'qt_gui':
${class_name}::${class_name} () : QWidget(), top_block("display_qt") {
    this->setWindowTitle("${title}");
    // check_set_qss
    // set icon
    this->top_scroll_layout = new QVBoxLayout();
    this->setLayout(this->top_scroll_layout);
    this->top_scroll = new QScrollArea();
    this->top_scroll->setFrameStyle(QFrame::NoFrame);
    this->top_scroll_layout->addWidget(this->top_scroll);
    this->top_scroll->setWidgetResizable(true);
    this->top_widget = new QWidget();
    this->top_scroll->setWidget(this->top_widget);
    this->top_layout = new QVBoxLayout(this->top_widget);
    this->top_grid_layout = new QGridLayout();
    this->top_layout->addLayout(this->top_grid_layout);

    this->settings = new QSettings("GNU Radio", "${class_name}");

% endif

% if flow_graph.get_option('thread_safe_setters'):
## self._lock = threading.RLock()
% endif

% if blocks:
// Blocks:
% for blk, blk_make, declarations in blocks:
    {
        ${indent(blk_make)}
##      % if 'alias' in blk.params and blk.params['alias'].get_evaluated():
##      ${blk.name}.set_block_alias("${blk.params['alias'].get_evaluated()}")
##      % endif
##      % if 'affinity' in blk.params and blk.params['affinity'].get_evaluated():
##      ${blk.name}.set_processor_affinity("${blk.params['affinity'].get_evaluated()}")
##      % endif
##      % if len(blk.sources) > 0 and 'minoutbuf' in blk.params and int(blk.params['minoutbuf'].get_evaluated()) > 0:
##      ${blk.name}.set_min_output_buffer(${blk.params['minoutbuf'].get_evaluated()})
##      % endif
##      % if len(blk.sources) > 0 and 'maxoutbuf' in blk.params and int(blk.params['maxoutbuf'].get_evaluated()) > 0:
##      ${blk.name}.set_max_output_buffer(${blk.params['maxoutbuf'].get_evaluated()})
##      % endif
    }
% endfor
% endif

% if connections:
// Connections:
% for connection in connections:
    ${connection.rstrip()};
% endfor
% endif
}

${class_name}::~${class_name} () {
}

// Callbacks:
% for var in parameters + variables:
${var.vtype} ${class_name}::get_${var.name} () const {
    return this->${var.name};
}

void ${class_name}::set_${var.name} (${var.vtype} ${var.name}) {
% if flow_graph.get_option('thread_safe_setters'):
    ## with self._lock:
    return;
% else:
    this->${var.name} = ${var.name};
    % for callback in callbacks[var.name]:
    ${callback};
    % endfor
% endif
}
% endfor

int main (int argc, char **argv) {
    % if parameters:
    ## parse args
    % endif
    % if flow_graph.get_option('realtime_scheduling'):
    if (enable_realtime_scheduling() != RT_OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }
    % endif

    % if generate_options == 'no_gui':
    ${class_name}* top_block = new ${class_name}();
    ## TODO: params
    % if flow_graph.get_option('run_options') == 'prompt':
    top_block->start();
    % for m in monitors:
    (top_block->${m.name}).start();
    % endfor
    std::cout << "Press Enter to quit: ";
    std::cin.ignore();
    top_block->stop();
    % elif flow_graph.get_option('run_options') == 'run':
    top_block->start();
    % endif
    % for m in monitors:
    (top_block->${m.name}).start();
    % endfor
    top_block->wait();
    % elif generate_options == 'qt_gui':
    QApplication app(argc, argv);

    ${class_name} *top_block = new ${class_name}();

    top_block->start();
    top_block->show();
    app.exec();

    % endif

    return 0;
}
% if generate_options == 'qt_gui':
#include "moc_${class_name}.cpp"
% endif
