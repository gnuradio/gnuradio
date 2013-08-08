# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Append all sources in this dir
########################################################################
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

set(gsm_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/add.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code.c
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_decode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_destroy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_encode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_explode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_implode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_option.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gsm_print.c
    ${CMAKE_CURRENT_SOURCE_DIR}/long_term.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/preprocess.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rpe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/short_term.c
    ${CMAKE_CURRENT_SOURCE_DIR}/table.c
)

set_source_files_properties(${gsm_sources}
    PROPERTIES COMPILE_DEFINITION "NeedFunctionPrototypes=1"
)
list(APPEND gr_vocoder_sources ${gsm_sources})
