<%def name="indent(code)">${ '    ' + '\n    '.join(str(code).splitlines()) }</%def>\
<%def name="doubleindent(code)">${ '\n        '.join(str(code).splitlines()) }</%def>\
#ifndef ${flow_graph.get_option('id').upper()}_HPP
#define ${flow_graph.get_option('id').upper()}_HPP
/********************
GNU Radio C++ Flow Graph Header File

Title: ${title}
% if flow_graph.get_option('author'):
Author: ${flow_graph.get_option('author')}
% endif
% if flow_graph.get_option('description'):
Description: ${flow_graph.get_option('description')}
% endif
GNU Radio version: ${config.version}
********************/

/********************
** Create includes
********************/
% for inc in includes:
${inc}
% endfor

% if generate_options == 'qt_gui':
#include <QVBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QGridLayout>
#include <QSettings>
#include <QApplication>
% endif

% if parameters:
#include <boost/program_options.hpp>
% endif

using namespace gr;

<%
class_name = flow_graph.get_option('id') + ('_' if flow_graph.get_option('id') == 'top_block' else '')
param_str = ", ".join((param.vtype + " " + param.name) for param in parameters)
%>\

% if generate_options.startswith('hb'):
class ${class_name};
typedef std::shared_ptr<${class_name}> ${class_name}_sptr;
${class_name}_sptr make_${class_name}();
% endif

% if generate_options == 'no_gui':
class ${class_name} {
% elif generate_options.startswith('hb'):
class ${class_name} : public hier_block2 {
% elif generate_options == 'qt_gui':
class ${class_name} : public QWidget {
    Q_OBJECT
% endif

private:
% if generate_options == 'qt_gui':
    QVBoxLayout *top_scroll_layout;
    QScrollArea *top_scroll;
    QWidget *top_widget;
    QVBoxLayout *top_layout;
    QGridLayout *top_grid_layout;
    QSettings *settings;
% endif


% for block, make, declarations in blocks:
% if declarations:
${indent(declarations)}
% endif
% endfor

% if parameters:
// Parameters:
% for param in parameters:
    ${param.vtype} ${param.cpp_templates.render('var_make')}
% endfor
% endif

% if variables:
// Variables:
% for var in variables:
    ${var.vtype} ${var.cpp_templates.render('var_make')}
% endfor
% endif

public:
% if not generate_options.startswith('hb'):
    top_block_sptr tb;
% endif
    ${class_name}(${param_str});
    ~${class_name}();

% for var in parameters + variables:
    ${var.vtype} get_${var.name} () const;
    void set_${var.name}(${var.vtype} ${var.name});
% endfor

};


% if generate_options.startswith('hb'):
<% in_sigs = flow_graph.get_hier_block_stream_io('in') %>
<% out_sigs = flow_graph.get_hier_block_stream_io('out') %>

<%def name="make_io_sig(io_sigs)">\
    <% size_strs = [ '%s*%s'%(io_sig['cpp_size'], io_sig['vlen']) for io_sig in io_sigs] %>\
    % if len(io_sigs) == 0:
    gr::io_signature::make(0, 0, 0)\
    % elif len(io_sigs) == 1:
    gr::io_signature::make(1, 1, ${size_strs[0]})\
    % else:
    gr::io_signaturev(${len(io_sigs)}, ${len(io_sigs)}, [${', '.join(size_strs)}])\
    % endif
</%def>\

${class_name}::${class_name} (${param_str}) : hier_block2("${title}",
        ${make_io_sig(in_sigs)},
        ${make_io_sig(out_sigs)}
        ) {
% for pad in flow_graph.get_hier_block_message_io('in'):
    message_port_register_hier_in("${pad['label']}")
% endfor
% for pad in flow_graph.get_hier_block_message_io('out'):
    message_port_register_hier_out("${pad['label']}")
% endfor

% if flow_graph.get_option('thread_safe_setters'):
## self._lock = threading.RLock()
% endif

% if blocks:
// Blocks:
% for blk, blk_make, declarations in blocks:
    {
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
${class_name}_sptr
make_${class_name}()
{
    return gnuradio::get_initial_sptr(new ${class_name}());
}
% endif
#endif

