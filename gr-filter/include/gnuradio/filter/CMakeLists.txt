# Copyright 2012 Free Software Foundation, Inc.
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
# generate helper scripts to expand templated files
########################################################################
include(GrPython)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_RUNTIME_PYTHONPATH}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_dict2(name, sig, 'filter')
        build_utils.expand_template(d, inp)

")

macro(expand_h root)
  #make a list of all the generated files
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_h ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
  endforeach(sig)

  #create a command to generate the files
  add_custom_command(
    OUTPUT ${expanded_files_h}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.h.t ${ARGN}
    )

  #install rules for the generated h files
  list(APPEND generated_includes ${expanded_files_h})
endmacro(expand_h)

########################################################################
# Invoke macro to generate various sources
#######################################################################
expand_h(fir_filter_XXX              ccc ccf fcc fff fsf scc)
expand_h(freq_xlating_fir_filter_XXX ccc ccf fcc fcf scf scc)
expand_h(interp_fir_filter_XXX       ccc ccf fcc fff fsf scc)
expand_h(rational_resampler_base_XXX ccc ccf fcc fff fsf scc)

add_custom_target(filter_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    api.h
    firdes.h
    fir_filter.h
    fir_filter_with_buffer.h
    fft_filter.h
    iir_filter.h
    interpolator_taps.h
    mmse_fir_interpolator_cc.h
    mmse_fir_interpolator_ff.h
    pm_remez.h
    polyphase_filterbank.h
    single_pole_iir.h
    dc_blocker_cc.h
    dc_blocker_ff.h
    filter_delay_fc.h
    fft_filter_ccc.h
    fft_filter_fff.h
    fractional_interpolator_cc.h
    fractional_interpolator_ff.h
    fractional_resampler_cc.h
    fractional_resampler_ff.h
    hilbert_fc.h
    iir_filter_ffd.h
    pfb_arb_resampler.h
    pfb_arb_resampler_ccf.h
    pfb_arb_resampler_ccc.h
    pfb_arb_resampler_fff.h
    pfb_channelizer_ccf.h
    pfb_decimator_ccf.h
    pfb_interpolator_ccf.h
    pfb_synthesizer_ccf.h
    single_pole_iir_filter_cc.h
    single_pole_iir_filter_ff.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/filter
    COMPONENT "filter_devel"
)

