/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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

#set $this_machine = $machine_dict[$args[0]]
#set $arch_names = $this_machine.arch_names

#for $arch in $this_machine.archs
#define LV_HAVE_$(arch.name.upper()) 1
#end for

#include <volk/volk_common.h>
#include "volk_machines.h"
#include <volk/volk_config_fixed.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#for $kern in $kernels
#include <volk/$(kern.name).h>
#end for

########################################################################
#def make_arch_have_list($archs)
$(' | '.join(['(1 << LV_%s)'%a.name.upper() for a in $archs]))#slurp
#end def

########################################################################
#def make_tag_str_list($tags)
{$(', '.join(['"%s"'%a for a in $tags]))}#slurp
#end def

########################################################################
#def make_tag_have_list($deps)
{$(', '.join([' | '.join(['(1 << LV_%s)'%a.upper() for a in d]) for d in $deps]))}#slurp
#end def

########################################################################
#def make_tag_kern_list($name, $tags)
{$(', '.join(['%s_%s'%($name, a) for a in $tags]))}#slurp
#end def

struct volk_machine volk_machine_$(this_machine.name) = {
    $make_arch_have_list($this_machine.archs),
    "$this_machine.name",
    $this_machine.alignment,
    #for $kern in $kernels
        #set $taglist, $tagdeps = $kern.get_tags($arch_names)
    "$kern.name",
    $make_tag_str_list($taglist),
    $make_tag_have_list($tagdeps),
    $make_tag_kern_list($kern.name, $taglist),
    $(len($taglist)),
    #end for
};
