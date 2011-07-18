/*---------------------------------------------------------------------------*\

  FILE........: lsp.c
  AUTHOR......: David Rowe
  DATE CREATED: 24/2/93


  This file contains functions for LPC to LSP conversion and LSP to
  LPC conversion. Note that the LSP coefficients are not in radians
  format but in the x domain of the unit circle.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2009 David Rowe

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

#ifndef __LSP__
#define __LSP__

int lpc_to_lsp (float *a, int lpcrdr, float *freq, int nb, float delta);
void lsp_to_lpc(float *freq, float *ak, int lpcrdr);

#endif
