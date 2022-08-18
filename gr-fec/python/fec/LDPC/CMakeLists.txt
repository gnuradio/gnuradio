# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

#####################################################################
include(GrPython)

gr_python_install(
    FILES __init__.py Generate_LDPC_matrix_functions.py Generate_LDPC_matrix.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec/LDPC)
