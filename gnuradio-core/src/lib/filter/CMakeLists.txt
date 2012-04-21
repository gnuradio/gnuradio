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
file(GLOB gr_core_filter_asms ${CMAKE_CURRENT_SOURCE_DIR}/*.S)
foreach(gr_core_filter_asm ${gr_core_filter_asms})
    set_property(SOURCE ${gr_core_filter_asm} PROPERTY LANGUAGE C)
endforeach(gr_core_filter_asm)

#detect 32 or 64 bit compiler
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86|x86_64|amd64)$")
    include(CheckTypeSize)
    check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
    if (${SIZEOF_VOID_P} EQUAL 8)
        set(CMAKE_SYSTEM_PROCESSOR_x86 64)
    else()
        set(CMAKE_SYSTEM_PROCESSOR_x86 32)
    endif()
endif()

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
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} -c "
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

macro(FILTER_GEN_EXTRACT outvar ext)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import os; print ';'.join(
        map(lambda x: os.path.join('${CMAKE_CURRENT_BINARY_DIR}', x.replace('\\\\', '').strip()),
        filter(lambda f: '${ext}' in f, open('${CMAKE_CURRENT_BINARY_DIR}/Makefile.gen').readlines()
    )))" OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE ${outvar})
    file(TO_CMAKE_PATH "${${outvar}}" ${outvar})
endmacro(FILTER_GEN_EXTRACT)

FILTER_GEN_EXTRACT(generated_filter_sources ".cc")
FILTER_GEN_EXTRACT(generated_filter_includes ".h")
FILTER_GEN_EXTRACT(generated_filter_swigs ".i")

#TODO simplify this list with a triple-threat for loop
set(generated_filter_deps
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

add_custom_command(
    OUTPUT
        ${generated_filter_sources}
        ${generated_filter_includes}
        ${generated_filter_swigs}
    DEPENDS ${generated_filter_deps}
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} -c
    "import os, sys;sys.path.append('${GR_CORE_PYTHONPATH}');sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}');os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}';from generate_all import generate_all;generate_all()"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "generating filter files"
    VERBATIM
)

add_custom_target(filter_generated DEPENDS
    ${generated_filter_sources}
    ${generated_filter_includes}
    ${generated_filter_swigs}
)

########################################################################
# Add target specific files
# May VOLK put a rest to all the insanity below.
########################################################################
if(MSVC)
    list(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_generic.cc
    )
    list(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_generic.cc
    )
else(MSVC)
if(CMAKE_SYSTEM_PROCESSOR_x86)
    list(APPEND gnuradio_core_sources
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
    list(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_float_dotprod_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_complex_dotprod_x86.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_ccomplex_dotprod_x86.cc
    )
endif()

if(CMAKE_SYSTEM_PROCESSOR_x86 AND "${CMAKE_SYSTEM_PROCESSOR_x86}" STREQUAL "64")
    list(APPEND gnuradio_core_sources
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
elseif(CMAKE_SYSTEM_PROCESSOR_x86 AND "${CMAKE_SYSTEM_PROCESSOR_x86}" STREQUAL "32")
    list(APPEND gnuradio_core_sources
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
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
    if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-maltivec)
    endif()
    list(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_powerpc.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_sysconfig_powerpc.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_cpu_powerpc.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fff_altivec.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_altivec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dotprod_fff_altivec.c
    )
    list(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_powerpc.cc
    )
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    list(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_sysconfig_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_cpu_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_fff_armv7_a.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/dotprod_fff_armv7_a.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dotprod_ccf_armv7_a.c
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_fir_ccf_armv7_a.cc
    )
    list(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_armv7_a.cc
    )
else()
    list(APPEND gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/sysconfig_generic.cc
    )
    list(APPEND test_gnuradio_core_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_dotprod_generic.cc
    )
endif()
endif(MSVC)

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
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
list(APPEND test_gnuradio_core_sources
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
install(FILES
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
if(ENABLE_PYTHON)
install(FILES
    ${generated_filter_swigs}
    ${CMAKE_CURRENT_SOURCE_DIR}/filter.i
    ${CMAKE_CURRENT_BINARY_DIR}/filter_generated.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)
endif(ENABLE_PYTHON)

########################################################################
# Handle triple-threat files that have cc, h, and i
########################################################################
set(gr_core_filter_triple_threats
    gr_adaptive_fir_ccc
    gr_adaptive_fir_ccf
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
    gr_pfb_synthesizer_ccf
    gr_pfb_decimator_ccf
    gr_pfb_interpolator_ccf
    gr_pfb_arb_resampler_ccf
    gr_pfb_arb_resampler_fff
    gr_pfb_clock_sync_ccf
    gr_pfb_clock_sync_fff
)

foreach(file_tt ${gr_core_filter_triple_threats})
    list(APPEND gnuradio_core_sources ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.cc)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio COMPONENT "core_devel")
    if(ENABLE_PYTHON)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.i DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig COMPONENT "core_swig")
    endif(ENABLE_PYTHON)
endforeach(file_tt ${gr_core_filter_triple_threats})
