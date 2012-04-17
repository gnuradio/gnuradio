#
# Copyright 2011-2012 Free Software Foundation, Inc.
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
find_package(Boost)

if(Boost_FOUND AND UNIX) #uses mkdir and $HOME

if(MSVC)
    include_directories(${CMAKE_SOURCE_DIR}/msvc)
endif(MSVC)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
)

add_executable(volk_profile
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_profile.cc
    ${CMAKE_SOURCE_DIR}/lib/qa_utils.cc
)

target_link_libraries(volk_profile volk ${Boost_LIBRARIES})

install(
    TARGETS volk_profile
    DESTINATION bin
    COMPONENT "volk"
)

endif(Boost_FOUND AND UNIX)
