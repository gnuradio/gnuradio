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
# Create executable to generate other sources
# http://www.vtk.org/Wiki/CMake_Cross_Compiling#Using_executables_in_the_build_created_during_the_build
########################################################################
if(NOT CMAKE_CROSSCOMPILING)
    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
    add_executable(generate_codebook ${CMAKE_CURRENT_SOURCE_DIR}/generate_codebook.c)
    target_link_libraries(generate_codebook -lm)
    export(TARGETS generate_codebook APPEND FILE ${EXPORT_FILE})
endif()

########################################################################
# Create codebook
########################################################################
set(CODEBOOKS
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp1.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp2.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp3.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp4.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp5.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp6.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp7.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp8.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp9.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp10.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebook.c
    DEPENDS generate_codebook ${CODEBOOKS}
    COMMAND generate_codebook lsp_cb ${CODEBOOKS} > ${CMAKE_CURRENT_BINARY_DIR}/codebook.c
)

########################################################################
# Create codebookd
########################################################################
set(CODEBOOKSD
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp1.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp2.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp3.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp4.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp5.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp6.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp7.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp8.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp9.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/dlsp10.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookd.c
    DEPENDS generate_codebook ${CODEBOOKSD}
    COMMAND generate_codebook lsp_cbd ${CODEBOOKSD} > ${CMAKE_CURRENT_BINARY_DIR}/codebookd.c
)

########################################################################
# Create codebookvq
########################################################################
set(CODEBOOKSVQ
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp1.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp2.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp3.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp4.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp45678910.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookvq.c
    DEPENDS generate_codebook ${CODEBOOKSVQ}
    COMMAND generate_codebook lsp_cbvq ${CODEBOOKSVQ} > ${CMAKE_CURRENT_BINARY_DIR}/codebookvq.c
)

########################################################################
# Create codebookjnd
########################################################################
set(CODEBOOKSJND
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp1.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp2.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp3.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lsp4.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspjnd5-10.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookjnd.c
    DEPENDS generate_codebook ${CODEBOOKSJND}
    COMMAND generate_codebook lsp_cbjnd ${CODEBOOKSJND} > ${CMAKE_CURRENT_BINARY_DIR}/codebookjnd.c
)

########################################################################
# Create codebookjvm
########################################################################
set(CODEBOOKSJVM
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspjvm1.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspjvm2.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspjvm3.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookjvm.c
    DEPENDS generate_codebook ${CODEBOOKSJVM}
    COMMAND generate_codebook lsp_cbjvm ${CODEBOOKSJVM} > ${CMAKE_CURRENT_BINARY_DIR}/codebookjvm.c
)

########################################################################
# Create codebookvqanssi
########################################################################
set(CODEBOOKSVQANSSI
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspvqanssi1.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspvqanssi2.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspvqanssi3.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspvqanssi4.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookvqanssi.c
    DEPENDS generate_codebook ${CODEBOOKSVQANSSI}
    COMMAND generate_codebook lsp_cbvqanssi ${CODEBOOKSVQANSSI} > ${CMAKE_CURRENT_BINARY_DIR}/codebookvqanssi.c
)

########################################################################
# Create codebookdt
########################################################################
set(CODEBOOKSDT
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt1.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt2.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt3.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt4.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt5.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt6.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt7.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt8.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt9.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/codebook/lspdt10.txt
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookdt.c
    DEPENDS generate_codebook ${CODEBOOKSDT}
    COMMAND generate_codebook lsp_cbdt ${CODEBOOKSDT} > ${CMAKE_CURRENT_BINARY_DIR}/codebookdt.c
)

########################################################################
# Create codebookge
########################################################################
set(CODEBOOKSGE ${CMAKE_CURRENT_SOURCE_DIR}/codebook/gecb.txt)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookge.c
    DEPENDS generate_codebook ${CODEBOOKSGE}
    COMMAND generate_codebook ge_cb ${CODEBOOKSGE} > ${CMAKE_CURRENT_BINARY_DIR}/codebookge.c
)

########################################################################
# Append all sources in this dir
########################################################################
list(APPEND gr_vocoder_sources
    ${CMAKE_CURRENT_BINARY_DIR}/codebook.c
    ${CMAKE_CURRENT_BINARY_DIR}/codebookd.c
    ${CMAKE_CURRENT_BINARY_DIR}/codebookvq.c
    ${CMAKE_CURRENT_BINARY_DIR}/codebookjnd.c
    ${CMAKE_CURRENT_BINARY_DIR}/codebookjvm.c
    ${CMAKE_CURRENT_BINARY_DIR}/codebookvqanssi.c
    ${CMAKE_CURRENT_BINARY_DIR}/codebookdt.c
    ${CMAKE_CURRENT_BINARY_DIR}/codebookge.c


    ${CMAKE_CURRENT_SOURCE_DIR}/dump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nlp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/postfilter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/codec2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fifo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fdmdv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lsp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/phase.c
    ${CMAKE_CURRENT_SOURCE_DIR}/quantise.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pack.c
)
