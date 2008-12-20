/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_MB_CLASS_REGISTRY_H
#define INCLUDED_MB_CLASS_REGISTRY_H

#include <mblock/common.h>

//! conceptually, pointer to constructor
typedef mb_mblock_sptr (*mb_mblock_maker_t)(mb_runtime *runtime,
					    const std::string &instance_name,
					    pmt_t user_arg);

/*
 * \brief Maintain mapping between mblock class_name and factory (maker)
 */
class mb_class_registry : public boost::noncopyable {
public:
  static bool register_maker(const std::string &name, mb_mblock_maker_t maker);
  static bool lookup_maker(const std::string &name, mb_mblock_maker_t *maker);
};

template<class mblock>
mb_mblock_sptr mb_mblock_maker(mb_runtime *runtime,
			       const std::string &instance_name,
			       pmt_t user_arg)
{
  return mb_mblock_sptr(new mblock(runtime, instance_name, user_arg));
}

#define REGISTER_MBLOCK_CLASS(name) \
  bool __RBC__ ## name = mb_class_registry::register_maker(#name, &mb_mblock_maker<name>)

#endif /* INCLUDED_MB_CLASS_REGISTRY_H */
