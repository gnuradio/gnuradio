include(GrPython)

gr_python_install(
    FILES __init__.py base.py generator.py
          # types.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/bindtool/core)
