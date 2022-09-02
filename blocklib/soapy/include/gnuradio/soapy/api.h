/*
 * gr-soapy: Soapy SDR Radio Module
 *
 *  Copyright (C) 2018
 *  Libre Space Foundation <http://librespacefoundation.org/>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/attributes.h>

#ifdef gnuradio_soapy_EXPORTS
#define SOAPY_API __GR_ATTR_EXPORT
#else
#define SOAPY_API __GR_ATTR_IMPORT
#endif
