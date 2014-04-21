/*---------------------------------------------------------------------------*\

  FILE........: phaseexp.h
  AUTHOR......: David Rowe
  DATE CREATED: June 2012

  Experimental functions for quantising, modelling and synthesising phase.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2012 David Rowe

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

#ifndef __PHASEEXP__
#define __PHASEEXP__

#include "kiss_fft.h"

struct PEXP;

struct PEXP * phase_experiment_create();
void phase_experiment_destroy(struct PEXP *pexp);
void phase_experiment(struct PEXP *pexp, MODEL *model, char *arg);

#endif
