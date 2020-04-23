<%
import os
## file_list = files.sort()
%>
${license}

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

## File Includes
% for f in files:  
<%
basename = os.path.splitext(f)[0]
%>\
## #include "generated/${basename}_python.hpp"
void bind_${basename}(py::module&);
% endfor

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(${module_name}_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

% for f in files:
<%
basename = os.path.splitext(f)[0]
%>\
    bind_${basename}(m);
% endfor    
}

