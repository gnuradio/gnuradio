# Copyright 2012-2013 Free Software Foundation, Inc.
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
file(GLOB xml_files "*.xml")

# Force out the controlport GRC blocks if we've disabled it.
if(NOT ENABLE_GR_CTRLPORT)
  list(REMOVE_ITEM xml_files
    ${CMAKE_CURRENT_SOURCE_DIR}/ctrlport_probe_c.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/ctrlport_probe2_c.xml
    )
endif(NOT ENABLE_GR_CTRLPORT)

install(FILES ${xml_files} DESTINATION ${GRC_BLOCKS_DIR} COMPONENT "blocks_python")
