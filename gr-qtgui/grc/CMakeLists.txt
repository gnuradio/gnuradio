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
file(GLOB xml_cmakein_files "*.xml.cmakein")

foreach(xml_cmakein_file IN ITEMS ${xml_cmakein_files})
	get_filename_component(FILE_NAME ${xml_cmakein_file} NAME_WE)
	configure_file(${xml_cmakein_file} "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.xml" @ONLY)
	set(xml_files ${xml_files} "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.xml")
endforeach(xml_cmakein_file)

install(FILES ${xml_files} DESTINATION ${GRC_BLOCKS_DIR})
