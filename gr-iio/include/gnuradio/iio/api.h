/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_API_H
#define INCLUDED_IIO_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_iio_EXPORTS
#define IIO_API __GR_ATTR_EXPORT
#else
#define IIO_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_IIO_API_H */
