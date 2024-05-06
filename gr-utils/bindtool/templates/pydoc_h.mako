##
## Copyright 2020 Free Software Foundation, Inc.
##
## This file is part of GNU Radio
##
## SPDX-License-Identifier: GPL-3.0-or-later
##
##
${license}
<%
    namespace = header_info['namespace']
    modname = header_info['module_name']
%>\
\
#include "pydoc_macros.h"
#define D(...) DOC(gr, ${modname}, __VA_ARGS__)
\
/*
  This file contains placeholders for docstrings for the Python bindings.
  Do not edit! These were automatically extracted during the binding process
  and will be overwritten during the build process
 */
${render_namespace(namespace=namespace, modname=[modname])} \
\
<%def name='render_docstring_const(modname,names,info,docstring="",info_all=None,max_unambiguous=1)'>
<% 
  names = list(filter(None, names))
  suffix = ''
  if info_all:
    matcher = lambda x,name: x['name'] == name
    matched_list = [f for f in info_all if matcher(f,info['name'])]
    overloaded = len(matched_list) > max_unambiguous
    suffix = ''
    if overloaded:
      index_into_list = matched_list.index(info)
      suffix = '_'+str(index_into_list)
%> \
static const char *__doc_${'_'.join(names)}${suffix} = R"doc(${docstring})doc";
</%def> \
<%def name='render_namespace(namespace, modname)'>
<%
    classes=namespace['classes'] if 'classes' in namespace else []
    free_functions=namespace['free_functions'] if 'free_functions' in namespace else []
    free_enums = namespace['enums'] if 'enums' in namespace else []
    subnamespaces = namespace['namespaces'] if 'namespaces' in namespace else []
%>
\
% for cls in classes:
<%
        member_functions = cls['member_functions'] if 'member_functions' in cls else []
        constructors = cls['constructors'] if 'constructors' in cls else []
        class_enums = cls['enums'] if 'enums' in cls else []
        class_variables = cls['variables'] if 'variables' in cls else []
%>\
\
${render_docstring_const(modname,namespace['name'].split('::')+[cls['name']],cls,cls['docstring'] if 'docstring' in cls else "")}
\
% for cotr in constructors:
${render_docstring_const(modname,namespace['name'].split('::')+[cls['name'],cotr['name']],cotr,cotr['docstring'] if 'docstring' in cotr else "",constructors,0)}
% endfor ## constructors
\
% for fcn in member_functions:
${render_docstring_const(modname,namespace['name'].split('::')+[cls['name'],fcn['name']],fcn,fcn['docstring'] if 'docstring' in fcn else "",member_functions)}
% endfor ## member_functions
% endfor ## classes 
\
% if free_functions:
% for fcn in free_functions:
${render_docstring_const(modname,namespace['name'].split('::')+[fcn['name']],fcn,fcn['docstring'] if 'docstring' in fcn else "",free_functions)}
% endfor ## free_functions
% endif ## free_functions
\
% for sns in subnamespaces:
<%  
  submod_name = sns['name'].split('::')[-1]
%>
${render_namespace(namespace=sns,modname=modname+[submod_name])}
% endfor
</%def>
