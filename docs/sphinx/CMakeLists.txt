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

include(GrPython)

# You can set these variables from the command line.
set(SPHINXOPTS)
set(SPHINXBUILD sphinx-build)
set(PAPER)
set(BUILDDIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(SRCDIR   ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Internal variables.
set(PAPEROPT_a4 -D latex_paper_size=a4)
set(PAPEROPT_letter -D latex_paper_size=letter)
set(ALLSPHINXOPTS -d ${BUILDDIR}/doctrees ${PAPEROPT_${PAPER}} ${SPHINXOPTS} ${SRCDIR})

# Set up environmental variables to pass to sphinx
set(LDPATH LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR})
set(PYPATH PYTHONPATH=${CMAKE_INSTALL_PREFIX}/${GR_PYTHON_DIR})
message(STATUS "ENVIRONS: ${LDPATH} ${PYPATH}")

add_custom_target(sphinx
  COMMAND  ${LDPATH} ${PYPATH} ${SPHINX_EXECUTABLE} -b html ${ALLSPHINXOPTS} ${BUILDDIR}/html
  COMMENT "Building Sphinx Documents from installed system."
 )

