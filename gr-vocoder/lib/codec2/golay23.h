/*---------------------------------------------------------------------------*\

  FILE........: golay23.h
  AUTHOR......: David Rowe
  DATE CREATED: 3 March 2013

  Header file for Golay FEC.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2013 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __GOLAY23__
#define __GOLAY23__

#ifdef __cplusplus
extern "C" {
#endif

void golay23_init(void);
int  golay23_encode(int data);
int  golay23_decode(int received_codeword);
int  golay23_count_errors(int recd_codeword, int corrected_codeword);

#ifdef __cplusplus
}
#endif

#endif
