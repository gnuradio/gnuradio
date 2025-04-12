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
#include <gnuradio/top_block.h>
% for inc in includes:
${inc}
% endfor

#include <QApplication>
#include <QCloseEvent>
#include <QGridLayout>
#include <QScrollArea>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>


using namespace gr;

<%
class_name = flow_graph.get_option('id') + ('_' if flow_graph.get_option('id') == 'top_block' else '')
param_str = ", ".join((param.vtype + " " + param.name) for param in parameters)
%>\

class ${class_name} : public QWidget {
    Q_OBJECT

private:
    QVBoxLayout *top_scroll_layout;
    QScrollArea *top_scroll;
    QWidget *top_widget;
    QVBoxLayout *top_layout;
    QGridLayout *top_grid_layout;
    QSettings *settings;
    void closeEvent(QCloseEvent *event);

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
    top_block_sptr tb;
    ${class_name}(${param_str});
    ~${class_name}();

% for var in parameters + variables:
    ${var.vtype} get_${var.name} () const;
    void set_${var.name}(${var.vtype} ${var.name});
% endfor

};

#endif
