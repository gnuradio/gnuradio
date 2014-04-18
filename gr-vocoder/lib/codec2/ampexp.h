/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: ampexp.h                                     
  AUTHOR......: David Rowe                                             
  DATE CREATED: & August 2012
                                                                             
  Functions for experimenting with amplitude quantisation.
                                                                             
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
  along with this program; if not,see <http://www.gnu.org/licenses/>. 
*/

#ifndef __AMPEX__
#define __AMPEXP__

#include "defines.h"

struct AEXP;

struct AEXP *amp_experiment_create();
void amp_experiment_destroy(struct AEXP *aexp);
void amp_experiment(struct AEXP *aexp, MODEL *model, char *arg);

#endif
