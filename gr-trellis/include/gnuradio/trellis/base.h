/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_BASE_H
#define INCLUDED_TRELLIS_BASE_H

#include <vector>

namespace gr {
namespace trellis {

/*!
 * \brief  change base
 */

bool dec2base(unsigned int num, int base, std::vector<int>& s);
bool dec2bases(unsigned int num, const std::vector<int>& bases, std::vector<int>& s);
unsigned int base2dec(const std::vector<int>& s, int base);
unsigned int bases2dec(const std::vector<int>& s, const std::vector<int>& bases);

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_BASE_H */
