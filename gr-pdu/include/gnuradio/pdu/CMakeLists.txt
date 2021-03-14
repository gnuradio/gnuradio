# Copyright 2021 Jacob Gilbert
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES
    api.h
    pdu_filter.h
    pdu_remove.h
    pdu_set.h
    pdu_split.h
    pdu_to_tagged_stream.h
    random_pdu.h
    tagged_stream_to_pdu.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/pdu
)

