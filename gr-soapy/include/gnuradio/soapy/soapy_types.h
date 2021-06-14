/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* GR-style type aliases for Soapy types exposed in gr-soapy blocks */
#ifndef INCLUDED_GR_SOAPY_TYPES_H
#define INCLUDED_GR_SOAPY_TYPES_H

#include <SoapySDR/Types.hpp>

namespace gr {
namespace soapy {

using arginfo_t = SoapySDR::ArgInfo;
using arginfo_list_t = SoapySDR::ArgInfoList;

using argtype_t = SoapySDR::ArgInfo::Type;

using kwargs_t = SoapySDR::Kwargs;
using kwargs_list_t = SoapySDR::KwargsList;

using range_t = SoapySDR::Range;
using range_list_t = SoapySDR::RangeList;

} // namespace soapy
} // namespace gr

#endif
