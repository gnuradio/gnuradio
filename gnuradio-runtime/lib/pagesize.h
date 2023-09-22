/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_PAGESIZE_H_
#define GR_PAGESIZE_H_

#include <gnuradio/api.h>

namespace gr {

/*!
 * \brief return the page size in bytes
 */
GR_RUNTIME_API int pagesize();

} /* namespace gr */

#endif /* GR_PAGESIZE_H_ */
