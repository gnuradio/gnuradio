include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND pmt_python_files pmt_python.cc pmt_pool_python.cc pmt_sugar_python.cc
     python_bindings.cc)

gr_pybind_make(pmt ../../.. pmt "${pmt_python_files}")

install(
    TARGETS pmt_python
    DESTINATION ${GR_PYTHON_DIR}/pmt
    COMPONENT pythonapi)
