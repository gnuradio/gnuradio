include(GrPybind)

${'########################################################################'}
# Python Bindings
${'########################################################################'}
<%
import os
file_list = module_name + '_python_files'
%>
list(append ${file_list}
## File Includes
% for f in files:  
<%
basename = os.path.splitext(f)[0]
%>\
    ${basename}_python.cc
% endfor
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(${module_name} 
   ../../.. 
   gr::${module_name}
   "${'${'+file_list+'}'}")

install(targets ${module_name}_python destination ${'${GR_PYTHON_DIR}'}/gnuradio/${module_name} component pythonapi)
