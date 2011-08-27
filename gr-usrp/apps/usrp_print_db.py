#!/usr/bin/env python
#
# Copyright 2006,2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

#!/usr/bin/env python

from gnuradio import gr
from gnuradio import usrp
from optparse import OptionParser
from usrpm import usrp_dbid

u_source = usrp.source_c()
u_sink = usrp.sink_c()

subdev_Ar = usrp.selected_subdev(u_source, (0,0))
subdev_Br = usrp.selected_subdev(u_source, (1,0))
subdev_At = usrp.selected_subdev(u_sink, (0,0))
subdev_Bt = usrp.selected_subdev(u_sink, (1,0))

print "RX d'board %s" % (subdev_Ar.side_and_name(),)
print "RX d'board %s" % (subdev_Br.side_and_name(),)
print "TX d'board %s" % (subdev_At.side_and_name(),)
print "TX d'board %s" % (subdev_Bt.side_and_name(),)

