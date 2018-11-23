<%def name="indent(code)">${ '    ' + '\n    '.join(str(code).splitlines()) }</%def>\
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
Generated: ${generated_time}
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
% endif

% if parameters:
#include <iostream>
#include <boost/program_options.hpp>
% endif

using namespace gr;

<%
class_name = flow_graph.get_option('id') + ('_' if flow_graph.get_option('id') == 'top_block' else '')
param_str = ", ".join((param.vtype + " " + param.name) for param in parameters)
%>\


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
    ${param.get_cpp_var_make()}
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
    gr::top_block_sptr tb;
% endif
	${class_name}(${param_str});
	~${class_name}();

% for var in parameters + variables:
    ${var.vtype} get_${var.name} () const;
    void set_${var.name}(${var.vtype} ${var.name});
% endfor

};
#endif
