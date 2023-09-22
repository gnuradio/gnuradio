# Templates need to be placed in build directory for qa tests to work
file(
    COPY generic_python_cc.mako CMakeLists.txt.mako license.mako python_bindings_cc.mako
         pydoc_h.mako
         # types.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(
    FILES generic_python_cc.mako CMakeLists.txt.mako license.mako python_bindings_cc.mako
          pydoc_h.mako
          # types.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/bindtool/templates)
