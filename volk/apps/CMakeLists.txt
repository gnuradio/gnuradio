#
# Copyright 2011 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

########################################################################
# Setup profiler
########################################################################
IF(MSVC)
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/msvc)
ENDIF(MSVC)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

ADD_EXECUTABLE(volk_profile
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_profile.cc
    ${CMAKE_SOURCE_DIR}/lib/qa_utils.cc
    ${CMAKE_SOURCE_DIR}/lib/volk_prefs.cc
)

TARGET_LINK_LIBRARIES(volk_profile volk ${Boost_LIBRARIES})
