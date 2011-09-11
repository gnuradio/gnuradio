# Copyright 2010-2011 Free Software Foundation, Inc.
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
# This file included, use CMake directory variables
########################################################################

#set the C language property on the assembly files so the compiler will pick them up
FILE(GLOB gr_core_filter_asms ${CMAKE_CURRENT_SOURCE_DIR}/*.S)
FOREACH(gr_core_filter_asm ${gr_core_filter_asms})
    SET_PROPERTY(SOURCE ${gr_core_filter_asm} PROPERTY LANGUAGE C)
ENDFOREACH(gr_core_filter_asm)

#detect 32 or 64 bit compiler
IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86|x86_64)$")
    include(CheckTypeSize)
    check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
    if (${SIZEOF_VOID_P} EQUAL 8)
        set(CMAKE_SYSTEM_PROCESSOR_x86 64)
    else()
        set(CMAKE_SYSTEM_PROCESSOR_x86 32)
    endif()
ENDIF()

########################################################################
# Generate the makefile.gen, then extract its sources:
# This is a round-about way to extract the sources,
# but it requires minimum changed to the python utils.
#
# The recommended way to do this:
#   - Make a generation macro that registers the sources command.
#   - List the generation macro with each templated source file.
#   - Create a python script (very generic) to perform generation.
#   - This way the targets would depend only on their sources.
########################################################################
EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c "
import os, sys
sys.path.append('${GR_CORE_PYTHONPATH}')
sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.environ['gendir'] = '${CMAKE_CURRENT_BINARY_DIR}'
os.environ['do_makefile'] = '1'
os.environ['do_sources'] = '0'
from generate_all import generate_all
generate_all()
    " WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

MACRO(FILTER_GEN_EXTRACT outvar ext)
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import os; print ';'.join(
        map(lambda x: os.path.join('${CMAKE_CURRENT_BINARY_DIR}', x.replace('\\\\', '').strip()),
        filter(lambda f: '${ext}' in f, open('${CMAKE_CURRENT_BINARY_DIR}/Makefile.gen').readlines()
    )))" OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE ${outvar})
    FILE(TO_CMAKE_PATH "${${outvar}}" ${outvar})
ENDMACRO(FILTER_GEN_EXTRACT)

FILTER_GEN_EXTRACT(generated_filter_sources ".cc")
FILTER_GEN_EXTRACT(generated_filter_includes ".h")
FILTER_GEN_EXTRACT(generated_filter_swigs ".i")

#TODO simplify this list with a triple-threat for loop
SET(generated_filter_deps
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_all.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_fir_XXX.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_fir_filter_XXX.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_interp_fir_filter_XXX.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_rational_resampler_base_XXX.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_fir_sysconfig.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_fir_sysconfig_generic.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_fir_util.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gr_freq_xlating_fir_filter_XXX.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_gri_fir_filter_with_buffer_XXX.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_utils.py
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_XXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_XXX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_XXX_generic.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_XXX_generic.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_filter_XXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_filter_XXX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_filter_XXX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_interp_fir_filter_XXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_interp_fir_filter_XXX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_interp_fir_filter_XXX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_rational_resampler_base_XXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_rational_resampler_base_XXX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_rational_resampler_base_XXX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_freq_xlating_fir_filter_XXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_freq_xlating_fir_filter_XXX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_freq_xlating_fir_filter_XXX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fir_filter_with_buffer_XXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fir_filter_with_buffer_XXX.h.t
)

ADD_CUSTOM_COMMAND(
    OUTPUT
        ${generated_filter_sources}
        ${generated_filter_includes}
        ${generated_filter_swigs}
    DEPENDS ${generated_filter_deps}
    COMMAND ${PYTHON_EXECUTABLE} -B -c
    \"import os, sys\;sys.path.append('${GR_CORE_PYTHONPATH}')\;sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}')\;os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'\;from generate_all import generate_all\;generate_all()\"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "generating filter files"
)

ADD_CUSTOM_TARGET(filter_generated DEPENDS
    ${generated_filter_sources}
    ${generated_filter_includes}
    ${generated_filter_swigs}
)

########################################################################
# Add target specific files
# May VOLK put a rest to all the insanity below.
########################################################################
IF(MSVC)
    LIST(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_generic.cc
    )
    LIST(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_generic.cc
    )
ELSE(MSVC)
IF(CMAKE_SYSTEM_PROCESSOR_x86)
    LIST(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_sysconfig_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_cpu_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_ccc_simd.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_ccc_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fff_simd.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fff_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fsf_simd.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fsf_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_scc_simd.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_scc_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fcc_simd.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fcc_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_ccf_simd.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_ccf_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sse_debug.c
    )
    LIST(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_float_dotprod_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_complex_dotprod_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_ccomplex_dotprod_x86.cc
    )
ENDIF()

IF(CMAKE_SYSTEM_PROCESSOR_x86 AND ${CMAKE_SYSTEM_PROCESSOR_x86} EQUAL 64)
    LIST(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/float_dotprod_sse64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/float_dotprod_3dnow64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_3dnowext64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_3dnow64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_sse64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_3dnowext64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_3dnow64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_sse64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/fcomplex_dotprod_3dnow64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/fcomplex_dotprod_sse64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/short_dotprod_mmx64.S
    )
ELSEIF(CMAKE_SYSTEM_PROCESSOR_x86 AND ${CMAKE_SYSTEM_PROCESSOR_x86} EQUAL 32)
    LIST(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/float_dotprod_sse.S
        ${CMAKE_CURRENT_SOURCE_DIR}/float_dotprod_3dnow.S
        ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_3dnowext.S
        ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_3dnow.S
        ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_sse.S
        ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_3dnowext.S
        ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_3dnow.S
        ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_sse.S
        ${CMAKE_CURRENT_SOURCE_DIR}/fcomplex_dotprod_3dnow.S
        ${CMAKE_CURRENT_SOURCE_DIR}/fcomplex_dotprod_sse.S
        ${CMAKE_CURRENT_SOURCE_DIR}/short_dotprod_mmx.S
    )
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
    LIST(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_powerpc.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_sysconfig_powerpc.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_cpu_powerpc.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fff_altivec.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_altivec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dotprod_fff_altivec.c
    )
    LIST(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_powerpc.cc
    )
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    LIST(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_sysconfig_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_cpu_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fff_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/dotprod_fff_armv7_a.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dotprod_ccf_armv7_a.c
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_ccf_armv7_a.cc
    )
    LIST(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_armv7_a.cc
    )
ELSE()
    LIST(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_generic.cc
    )
    LIST(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_generic.cc
    )
ENDIF()
ENDIF(MSVC)

########################################################################
# Append gnuradio-core library sources
########################################################################
LIST(APPEND gnuradio_core_sources
    ${generated_filter_sources}
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fft_filter_fff_generic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fft_filter_ccc_generic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sincos.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_goertzel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_mmse_fir_interpolator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_mmse_fir_interpolator_cc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_generic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_generic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/float_dotprod_generic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/short_dotprod_generic.c
)

########################################################################
# Append gnuradio-core test sources
########################################################################
LIST(APPEND test_gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fir_ccf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fir_fcc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fir_fff.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fir_ccc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fir_scc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_rotator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_mmse_fir_interpolator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_mmse_fir_interpolator_cc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_fir_filter_with_buffer_ccf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_fir_filter_with_buffer_ccc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_fir_filter_with_buffer_fcc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_fir_filter_with_buffer_fff.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_fir_filter_with_buffer_fsf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_fir_filter_with_buffer_scc.cc
)

########################################################################
# Install runtime headers
########################################################################
INSTALL(FILES
    ${generated_filter_includes}
    ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_generic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/complex_dotprod_x86.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fcomplex_dotprod_x86.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_generic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ccomplex_dotprod_x86.h
    ${CMAKE_CURRENT_SOURCE_DIR}/float_dotprod_generic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/float_dotprod_x86.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_altivec.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_cpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fft_filter_fff_generic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fft_filter_ccc_generic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_sysconfig_x86.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_sysconfig_powerpc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_rotator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sincos.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_single_pole_iir.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vec_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_goertzel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_iir.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_mmse_fir_interpolator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_mmse_fir_interpolator_cc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_filter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/short_dotprod_generic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/short_dotprod_x86.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sse_debug.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Install swig headers
########################################################################
IF(ENABLE_PYTHON)
INSTALL(FILES
    ${generated_filter_swigs}
    ${CMAKE_CURRENT_SOURCE_DIR}/filter.i
    ${CMAKE_CURRENT_BINARY_DIR}/filter_generated.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)
ENDIF(ENABLE_PYTHON)

########################################################################
# Handle triple-threat files that have cc, h, and i
########################################################################
SET(gr_core_filter_triple_threats
    gr_adaptive_fir_ccc
    gr_adaptive_fir_ccf
    gr_cma_equalizer_cc
    gr_dc_blocker_cc
    gr_dc_blocker_ff
    gr_fft_filter_ccc
    gr_fft_filter_fff
    gr_filter_delay_fc
    gr_fractional_interpolator_ff
    gr_fractional_interpolator_cc
    gr_goertzel_fc
    gr_hilbert_fc
    gr_iir_filter_ffd
    gr_single_pole_iir_filter_ff
    gr_single_pole_iir_filter_cc
    gr_pfb_channelizer_ccf
    gr_pfb_synthesis_filterbank_ccf
    gr_pfb_decimator_ccf
    gr_pfb_interpolator_ccf
    gr_pfb_arb_resampler_ccf
    gr_pfb_clock_sync_ccf
    gr_pfb_clock_sync_fff
)

FOREACH(file_tt ${gr_core_filter_triple_threats})
    LIST(APPEND gnuradio_core_sources ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.cc)
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio COMPONENT "core_devel")
    IF(ENABLE_PYTHON)
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.i DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig COMPONENT "core_swig")
    ENDIF(ENABLE_PYTHON)
ENDFOREACH(file_tt ${gr_core_filter_triple_threats})
