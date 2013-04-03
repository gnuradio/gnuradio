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

include(GrPython)

GR_PYTHON_INSTALL(PROGRAMS
  affinity_set.py
  plot_flops.py
  run_synthetic.py
  synthetic.py
  wfm_rcv_pll_to_wav.py
  DESTINATION ${GR_PKG_DATA_DIR}/examples/mp-sched
  COMPONENT "runtime_python"
)

install(
    FILES README
    DESTINATION ${GR_PKG_DATA_DIR}/examples/mp-sched
    COMPONENT "runtime_python"
)
