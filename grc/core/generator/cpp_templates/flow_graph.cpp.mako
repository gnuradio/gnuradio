<%def name="doubleindent(code)">${ '\n        '.join(str(code).splitlines()) }</%def>\
/********************
GNU Radio C++ Flow Graph Source File

Title: ${title}
% if flow_graph.get_option('author'):
Author: ${flow_graph.get_option('author')}
% endif
% if flow_graph.get_option('description'):
Description: ${flow_graph.get_option('description')}
% endif
GNU Radio version: ${config.version}
********************/

#include "${flow_graph.get_option('id')}.hpp"
% if flow_graph.get_option('realtime_scheduling'):
#include <gnuradio/realtime.h>
% endif

% if parameters:

namespace po = boost::program_options;

% endif
using namespace gr;

<%
class_name = flow_graph.get_option('id') + ('_' if flow_graph.get_option('id') == 'top_block' else '')

param_str = ", ".join((param.vtype + " " + param.name) for param in parameters)
param_str_without_types = ", ".join(param.name for param in parameters)
initializer_str = ",\n  ".join((param.name + "(" + param.name + ")") for param in parameters)

if generate_options == 'qt_gui':
  initializer_str = 'QWidget()' + (',\n  ' if len(parameters) > 0 else '') + initializer_str

if len(initializer_str) > 0:
  initializer_str = '\n: ' + initializer_str
%>\

${class_name}::${class_name} (${param_str}) ${initializer_str} {

% if generate_options == 'qt_gui':
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
    this->tb = gr::make_top_block("${title}");

% if blocks:
// Blocks:
% for blk, blk_make, declarations in blocks:
        ${doubleindent(blk_make)}
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
    % for parameter in parameters:
    ${parameter.vtype} ${parameter.name} = ${parameter.cpp_templates.render('make')};
    % endfor

    po::options_description desc("Options");
    desc.add_options()
    ("help", "display help")
    % for parameter in parameters:
    ("${parameter.name}", po::value<${parameter.vtype}>(&${parameter.name}), "${parameter.label}")
    % endfor
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }
    % endif
    % if flow_graph.get_option('realtime_scheduling'):
    if (enable_realtime_scheduling() != RT_OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }
    % endif

    % if generate_options == 'no_gui':
    ${class_name}* top_block = new ${class_name}(${param_str_without_types});
    ## TODO: params
    % if flow_graph.get_option('run_options') == 'prompt':
    top_block->tb->start();
    % for m in monitors:
    (top_block->${m.name}).start();
    % endfor
    std::cout << "Press Enter to quit: ";
    std::cin.ignore();
    top_block->tb->stop();
    % elif flow_graph.get_option('run_options') == 'run':
    top_block->tb->start();
    % endif
    % for m in monitors:
    (top_block->${m.name}).start();
    % endfor
    top_block->tb->wait();
    % elif generate_options == 'qt_gui':
    QApplication app(argc, argv);

    ${class_name}* top_block = new ${class_name}(${param_str_without_types});

    top_block->tb->start();
    top_block->show();
    app.exec();

    % endif

    return 0;
}
% if generate_options == 'qt_gui':
#include "moc_${class_name}.cpp"
% endif
