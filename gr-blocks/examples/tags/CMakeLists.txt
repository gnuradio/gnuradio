# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

install(FILES tagged_file_sink.grc test_tag_prop.grc
        DESTINATION ${GR_PKG_DATA_DIR}/examples/tags)

gr_python_install(PROGRAMS test_file_tags.py DESTINATION ${GR_PKG_DATA_DIR}/examples/tags)
