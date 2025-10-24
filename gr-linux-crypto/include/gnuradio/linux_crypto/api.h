/* -*- c++ -*- */
/*
 * Copyright 2024 GNU Radio
 *
 * This file is part of gr-linux-crypto.
 *
 * gr-linux-crypto is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * gr-linux-crypto is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gr-linux-crypto; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_LINUX_CRYPTO_API_H
#define INCLUDED_GR_LINUX_CRYPTO_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_linux_crypto_EXPORTS
#  define LINUX_CRYPTO_API __GR_ATTR_IMPORT
#else
#  define LINUX_CRYPTO_API __GR_ATTR_EXPORT
#endif

#endif /* INCLUDED_GR_LINUX_CRYPTO_API_H */
