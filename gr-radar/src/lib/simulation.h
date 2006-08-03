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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef INCLUDED_SIMULATION_H
#define INCLUDED_SIMULATION_H

#include <string>
#include <cmath>
#include <vector>
#include <iostream>


class point {
  double	d_x, d_y;
public:
  point(double x = 0, double y = 0) : d_x(x), d_y(y) {}

  double x() const { return d_x; }
  double y() const { return d_y; }

  void set_x(double x) { d_x = x; }
  void set_y(double y) { d_y = y; }

  static point
  add(const point &p1, const point &p2)
  {
    return point(p1.x() + p2.x(),
		 p1.y() + p2.y());
  }

  static point
  sub(const point &p1, const point &p2)
  {
    return point(p1.x() - p2.x(), p1.y() - p2.y());
  }

  static double
  distance(const point &p1, const point &p2)
  {
    point d = point::sub(p1, p2);
    return std::sqrt(d.x()*d.x() + d.y()*d.y());
  }

};


class dyn_object {
  point		d_pos;
  point		d_vel;
  std::string	d_name;
public:
  dyn_object(point pos=point(0,0), point vel=point(0,0), const std::string name="")
    : d_pos(pos), d_vel(vel), d_name(name) {}

  virtual ~dyn_object() {}

  point pos() const { return d_pos; }
  point vel() const { return d_vel; }
  std::string name() const { return d_name; }

  void set_pos(point pos) { d_pos = pos; }
  void set_vel(point vel) { d_vel = vel; }

  virtual bool update(double delta_t);

  static double
  distance(const dyn_object &o1, const dyn_object &o2)
  {
    return point::distance(o1.pos(), o2.pos());
  }

};


class simulation {
  double 			d_timestep;
  double 			d_now;
protected:
  std::vector<dyn_object *>	d_obj;

public:
  simulation(double timestep = 1.0, double now = 0.0)
    : d_timestep(timestep), d_now(now) {}
  virtual ~simulation();
  virtual bool update();
  virtual bool run(long long nsteps);

  void add_object(dyn_object *obj);
  double now() const { return d_now; }
  double timestep() const { return d_timestep; }
};

std::ostream& operator<<(std::ostream& out, const dyn_object& o);
std::ostream& operator<<(std::ostream& out, const point& p);

#endif /* INCLUDED_SIMULATION_H */

