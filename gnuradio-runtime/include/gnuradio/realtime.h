/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_REALTIME_H
#define INCLUDED_GR_REALTIME_H

#include <gnuradio/api.h>
#include <gnuradio/realtime_impl.h>

namespace gr {

/*!
 * \brief If possible, enable high-priority "real time" scheduling.
 * \ingroup misc
 */
GR_RUNTIME_API rt_status_t enable_realtime_scheduling();

} /* namespace gr */

#endif /* INCLUDED_GR_REALTIME_H */
