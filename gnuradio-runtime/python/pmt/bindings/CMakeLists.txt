include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND pmt_python_files
    pmt_python.cc
    pmt_pool_python.cc
    pmt_sugar_python.cc
    python_bindings.cc
)

GR_PYBIND_MAKE(pmt 
   ../../.. 
   pmt
   "${pmt_python_files}")

install(TARGETS pmt_python DESTINATION ${GR_PYTHON_DIR}/pmt COMPONENT pythonapi)
