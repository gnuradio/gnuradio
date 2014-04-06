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

set(D ${CMAKE_CURRENT_SOURCE_DIR}/codebook)

# lsp quantisers

set(CODEBOOKS
    ${D}/lsp1.txt
    ${D}/lsp2.txt
    ${D}/lsp3.txt
    ${D}/lsp4.txt
    ${D}/lsp5.txt
    ${D}/lsp6.txt
    ${D}/lsp7.txt
    ${D}/lsp8.txt
    ${D}/lsp9.txt
    ${D}/lsp10.txt
)

# lspd quantisers

set(CODEBOOKSD
    ${D}/dlsp1.txt
    ${D}/dlsp2.txt
    ${D}/dlsp3.txt
    ${D}/dlsp4.txt
    ${D}/dlsp5.txt
    ${D}/dlsp6.txt
    ${D}/dlsp7.txt
    ${D}/dlsp8.txt
    ${D}/dlsp9.txt
    ${D}/dlsp10.txt
)

# lspd VQ quantisers

set(CODEBOOKSVQ
    ${D}/lsp1.txt
    ${D}/lsp2.txt
    ${D}/lsp3.txt
    ${D}/lsp4.txt
)

set(CODEBOOKSJND
    ${D}/lsp1.txt
    ${D}/lsp2.txt
    ${D}/lsp3.txt
    ${D}/lsp4.txt
)

set(CODEBOOKSDT
    ${D}/lspdt1.txt
    ${D}/lspdt2.txt
    ${D}/lspdt3.txt
    ${D}/lspdt4.txt
    ${D}/lspdt5.txt
    ${D}/lspdt6.txt
    ${D}/lspdt7.txt
    ${D}/lspdt8.txt
    ${D}/lspdt9.txt
    ${D}/lspdt10.txt
)

set(CODEBOOKSJVM
    ${D}/lspjvm1.txt
    ${D}/lspjvm2.txt
    ${D}/lspjvm3.txt
)

set(CODEBOOKSVQANSSI
    ${D}/lspvqanssi1.txt
    ${D}/lspvqanssi2.txt
    ${D}/lspvqanssi3.txt
    ${D}/lspvqanssi4.txt
)

set(CODEBOOKSGE ${D}/gecb.txt)

# when crosscompiling import the executable targets from a file
if(CMAKE_CROSSCOMPILING)
    SET(IMPORT_EXECUTABLES "IMPORTFILE-NOTFOUND" CACHE FILEPATH "Point it to the export file from a native build")
    INCLUDE(${IMPORT_EXECUTABLES})
endif(CMAKE_CROSSCOMPILING)

# Build code generator binaries. These do not get installed.
if(NOT CMAKE_CROSSCOMPILING)
    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
    # generate_codebook
    add_executable(generate_codebook ${CMAKE_CURRENT_SOURCE_DIR}/generate_codebook.c)
    target_link_libraries(generate_codebook -lm)
    # genlspdtcb
    add_executable(genlspdtcb ${CMAKE_CURRENT_SOURCE_DIR}/genlspdtcb.c)
    target_link_libraries(genlspdtcb -lm)
    # Make native builds available for cross-compiling.
    export(TARGETS generate_codebook genlspdtcb
        FILE ${CMAKE_BINARY_DIR}/ImportExecutables.cmake)
endif(NOT CMAKE_CROSSCOMPILING)

# codebook.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebook.c
    COMMAND generate_codebook lsp_cb ${CODEBOOKS} > ${CMAKE_CURRENT_BINARY_DIR}/codebook.c
    DEPENDS generate_codebook ${CODEBOOKS}
)

# codebookd.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookd.c
    COMMAND generate_codebook lsp_cbd ${CODEBOOKSD} > ${CMAKE_CURRENT_BINARY_DIR}/codebookd.c
    DEPENDS generate_codebook ${CODEBOOKSD}
)

# codebookdt.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookdt.c
    COMMAND generate_codebook lsp_cbdt ${CODEBOOKSDT} > ${CMAKE_CURRENT_BINARY_DIR}/codebookdt.c
    DEPENDS generate_codebook ${CODEBOOKSDT}
)

# codebookvq.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookvq.c
    COMMAND generate_codebook lsp_cbvq ${CODEBOOKSVQ} > ${CMAKE_CURRENT_BINARY_DIR}/codebookvq.c
    DEPENDS generate_codebook ${CODEBOOKSVQ}
)

# codebookjnd.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookjnd.c
    COMMAND generate_codebook lsp_cbjnd ${CODEBOOKSJND} > ${CMAKE_CURRENT_BINARY_DIR}/codebookjnd.c
    DEPENDS generate_codebook ${CODEBOOKSJND}
)

# codebookjvm.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookjvm.c
    COMMAND generate_codebook lsp_cbjvm ${CODEBOOKSJVM} > ${CMAKE_CURRENT_BINARY_DIR}/codebookjvm.c
    DEPENDS generate_codebook ${CODEBOOKSJVM}
)

# codebookvqanssi.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookvqanssi.c
    COMMAND generate_codebook lsp_cbvqanssi ${CODEBOOKSVQANSSI} > ${CMAKE_CURRENT_BINARY_DIR}/codebookvqanssi.c
    DEPENDS generate_codebook ${CODEBOOKSVQANSSI}
)

# codebookge.c
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codebookge.c
    COMMAND generate_codebook ge_cb ${CODEBOOKSGE} > ${CMAKE_CURRENT_BINARY_DIR}/codebookge.c
    DEPENDS generate_codebook ${CODEBOOKSGE}
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
    ${CMAKE_CURRENT_SOURCE_DIR}/golay23.c
)
