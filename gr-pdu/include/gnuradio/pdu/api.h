/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_API_H
#define INCLUDED_PDU_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_pdu_EXPORTS
#define PDU_API __GR_ATTR_EXPORT
#else
#define PDU_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_PDU_API_H */
