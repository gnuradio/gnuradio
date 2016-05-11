/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef GR_SYS_PATHS_H
#define GR_SYS_PATHS_H

#include <gnuradio/api.h>

namespace gr {

  //! directory to create temporary files
  GR_RUNTIME_API const char *tmp_path();

  //! directory to store application data
  GR_RUNTIME_API const char *appdata_path();

  //! directory to store user configuration
  GR_RUNTIME_API const char *userconf_path();

} /* namespace gr */

#endif /* GR_SYS_PATHS_H */
