# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

install(FILES file_metadata_sink.grc file_metadata_source.grc
              file_metadata_vector_sink.grc file_metadata_vector_source.grc
        DESTINATION ${GR_PKG_DATA_DIR}/examples/metadata)
