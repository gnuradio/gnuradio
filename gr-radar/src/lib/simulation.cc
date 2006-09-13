/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "simulation.h"

bool
dyn_object::update(double delta_t)
{
  double new_x = pos().x() + delta_t * vel().x();
  double new_y = pos().y() + delta_t * vel().y();
  set_pos(point(new_x, new_y));
  return true;
}

simulation::~simulation()
{
  for (unsigned i = 0; i < d_obj.size(); i++){
    delete d_obj[i];
    d_obj[i] = 0;
  }
}

bool
simulation::update()
{
  bool ok = true;
  for (unsigned i = 0; i < d_obj.size(); i++){
    ok &= d_obj[i]->update(d_timestep);
  }
  d_now += d_timestep;
  return ok;
}

bool
simulation::run(long long nsteps)
{
  for (long long i = 0; i < nsteps; i++)
    if (!update())
      return false;

  return true;
}

void
simulation::add_object(dyn_object *obj)
{
  d_obj.push_back(obj);
}

// ----------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const dyn_object& o)
{
  out << "<" << o.name()
      << " pos: " << o.pos()
      << " vel: " << o.vel()
      << ">";
  return out;
}

std::ostream& operator<<(std::ostream& out, const point& p)
{
  out << "(" << p.x() << ", " << p.y() << ")";
  return out;
}

