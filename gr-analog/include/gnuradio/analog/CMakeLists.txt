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
        d = build_utils.standard_dict2(name, sig, 'analog')
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
expand_h(noise_source_X     s i f c)
expand_h(fastnoise_source_X s i f c)
expand_h(sig_source_X       s i f c)

add_custom_target(analog_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    api.h
    cpm.h
    noise_type.h
    agc.h
    agc2.h
    noise_type.h
    squelch_base_ff.h
    agc_cc.h
    agc_ff.h
    agc2_cc.h
    agc2_ff.h
    agc3_cc.h
    cpfsk_bc.h
    ctcss_squelch_ff.h
    dpll_bb.h
    feedforward_agc_cc.h
    fmdet_cf.h
    frequency_modulator_fc.h
    phase_modulator_fc.h
    pll_carriertracking_cc.h
    pll_freqdet_cf.h
    pll_refout_cc.h
    probe_avg_mag_sqrd_c.h
    probe_avg_mag_sqrd_cf.h
    probe_avg_mag_sqrd_f.h
    pwr_squelch_cc.h
    pwr_squelch_ff.h
    quadrature_demod_cf.h
    rail_ff.h
    sig_source_waveform.h
    simple_squelch_cc.h 
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/analog
    COMPONENT "analog_devel"
)

