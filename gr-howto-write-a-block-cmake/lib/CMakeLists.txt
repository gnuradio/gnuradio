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
# Setup library
########################################################################
INCLUDE(GrPlatform) #define LIB_SUFFIX

ADD_LIBRARY(gnuradio-howto SHARED howto_square_ff.cc howto_square2_ff.cc)
TARGET_LINK_LIBRARIES(gnuradio-howto ${Boost_LIBRARIES} ${GRUEL_LIBRARIES} ${GNURADIO_CORE_LIBRARIES})
SET_TARGET_PROPERTIES(gnuradio-howto PROPERTIES DEFINE_SYMBOL "gnuradio_howto_EXPORTS")

########################################################################
# Install built library files
########################################################################
INSTALL(TARGETS gnuradio-howto
    LIBRARY DESTINATION ${LIB_SUFFIX}/lib # .so/.dylib file
    ARCHIVE DESTINATION ${LIB_SUFFIX}/lib # .lib file
    RUNTIME DESTINATION bin               # .dll file
)

########################################################################
# Install public header files
########################################################################
INSTALL(FILES
    howto_api.h
    howto_square_ff.h
    howto_square2_ff.h
    DESTINATION include/howto
)

########################################################################
# Build and register unit test
########################################################################
FIND_PACKAGE(Boost COMPONENTS unit_test_framework)

INCLUDE(GrTest)
SET(GR_TEST_TARGET_DEPS gnuradio-howto)
#turn each test cpp file into an executable with an int main() function
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

ADD_EXECUTABLE(qa_howto_square_ff qa_howto_square_ff.cc)
TARGET_LINK_LIBRARIES(qa_howto_square_ff gnuradio-howto ${Boost_LIBRARIES})
GR_ADD_TEST(qa_howto_square_ff qa_howto_square_ff)

ADD_EXECUTABLE(qa_howto_square2_ff qa_howto_square2_ff.cc)
TARGET_LINK_LIBRARIES(qa_howto_square2_ff gnuradio-howto ${Boost_LIBRARIES})
GR_ADD_TEST(qa_howto_square2_ff qa_howto_square2_ff)
