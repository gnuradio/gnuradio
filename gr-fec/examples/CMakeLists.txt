# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

install(
    FILES ber_curve_gen.grc
          ber_curve_gen_ldpc.grc
          ber_test.grc
          fecapi_async_decoders.grc
          fecapi_async_encoders.grc
          fecapi_async_ldpc_decoders.grc
          fecapi_async_ldpc_encoders.grc
          fecapi_async_packed_decoders.grc
          fecapi_async_to_stream.grc
          fecapi_cc_decoders.grc
          fecapi_decoders.grc
          fecapi_encoders.grc
          fecapi_ldpc_decoders.grc
          fecapi_ldpc_encoders.grc
          fecapi_polar_async_packed_decoders.grc
          fecapi_polar_decoders.grc
          fecapi_polar_encoders.grc
          fecapi_tagged_decoders.grc
          fecapi_tagged_encoders.grc
          fecapi_tagged_ldpc_decoders.grc
          fecapi_tagged_ldpc_encoders.grc
          polar_ber_curve_gen.grc
          polar_code_example.grc
          tpc_ber_curve_gen.grc
    DESTINATION ${GR_PKG_FEC_EXAMPLES_DIR})
