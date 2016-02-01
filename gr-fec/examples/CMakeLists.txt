# Copyright 2014 Free Software Foundation, Inc.
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

install(
    FILES
    ber_test.grc
    ber_curve_gen.grc
    ber_curve_gen_ldpc.grc
    fecapi_decoders.grc
    fecapi_encoders.grc
    fecapi_async_decoders.grc
    fecapi_async_encoders.grc
    fecapi_async_to_stream.grc
    fecapi_async_packed_decoders.grc
    fecapi_tagged_decoders.grc
    fecapi_tagged_encoders.grc
    fecapi_cc_decoders.grc
    fecapi_polar_async_packed_decoders.grc
    fecapi_polar_decoders.grc
    fecapi_polar_encoders.grc
    polar_ber_curve_gen.grc
    polar_code_example.grc
    tpc_ber_curve_gen.grc
    DESTINATION ${GR_PKG_FEC_EXAMPLES_DIR}
    COMPONENT "fec_python"
)
