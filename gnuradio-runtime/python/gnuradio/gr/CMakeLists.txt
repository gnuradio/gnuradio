# Copyright 2012 Free Software Foundation, Inc.
# Copyright 2021 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

gr_python_install(
    FILES __init__.py
          exceptions.py
          tag_utils.py
          packet_utils.py
          gateway.py
          hier_block2.py
          top_block.py
          pubsub.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/gr)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)
    set(GR_TEST_TARGET_DEPS "")
    set(GR_TEST_LIBRARY_DIRS "")
    set(GR_TEST_PYTHON_DIRS ${PROJECT_BINARY_DIR}/gruel/src/python
                            ${PROJECT_BINARY_DIR}/gnuradio-runtime/python)
    include(GrTest)
    file(GLOB py_qa_test_files "qa_*.py")
    set(py_qa_test_files    qa_flowgraph.py
                            qa_prefs.py
                            qa_python_logging.py
                            qa_random.py
                            qa_sys_paths.py
                            qa_tag_utils.py
                            # Sadly, this still results in a deadlock due to
                            # GIL / Buffer locking; see
                            # https://github.com/gnuradio/gnuradio/issues/7711
                            # qa_get_first_tag_in_range.py
    )
    # This is a check for whether gr-blocks is enabled
    if(ENABLE_DEFAULT OR ENABLE_GR_BLOCKS)
        list(APPEND py_qa_test_files qa_hier_block2.py qa_uncaught_exception.py)
    else()
        message(
            STATUS
                "gr-blocks not enabled: Disabling hier block and uncaught exception test")
    endif()

    foreach(py_qa_test_file ${py_qa_test_files})
        get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
        if(CMAKE_CROSSCOMPILING)
            get_filename_component(py_qa_test_file ${py_qa_test_file} NAME)
            gr_add_test(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
        else()
            gr_add_test(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B
                        ${CMAKE_CURRENT_SOURCE_DIR}/${py_qa_test_file})
        endif(CMAKE_CROSSCOMPILING)
    endforeach(py_qa_test_file)
endif(ENABLE_TESTING)

add_subdirectory(bindings)
