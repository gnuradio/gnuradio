/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_STRTOD_SI_H
#define INCLUDED_STRTOD_SI_H

#include "usrp2_cdefs.h"
__U2_BEGIN_DECLS


/*!
 * \brief convert string at s to double honoring any trailing SI suffixes
 *
 * \param[in]  s is the string to convert
 * \param[out] result is the converted value
 * \returns non-zero iff conversion was successful.
 */
int strtod_si(const char *s, double *result);

__U2_END_DECLS


#endif /* INCLUDED_STRTOD_SI_H */

