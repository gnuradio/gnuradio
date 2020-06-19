include(GrPybind)

${'########################################################################'}
# Python Bindings
${'########################################################################'}
<%
import os
file_list = module_name + '_python_files'
%>
list(APPEND ${file_list}
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

install(TARGETS ${module_name}_python DESTINATION ${'${GR_PYTHON_DIR}'}/gnuradio/${module_name} COMPONENT pythonapi)
