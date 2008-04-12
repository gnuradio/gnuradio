#
# Copyright 2008 Free Software Foundation, Inc.
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
import os, sys, signal

# Turn application into a background daemon process.
#
# When this function returns:
#
# 1) The calling process is disconnected from its controlling terminal
#    and will not exit when the controlling session exits
# 2) If a pidfile name is provided, it is created and the new pid is
#    written into it.
# 3) If a logfile name is provided, it is opened and stdout/stderr are
#    redirected to it.
# 4) The process current working directory is changed to '/' to avoid
#    pinning any filesystem mounts.
# 5) The process umask is set to 0111.
#
# The return value is the new pid.
#
# To create GNU Radio applications that operate as daemons, add a call to this
# function after all initialization but just before calling gr.top_block.run()
# or .start().
#
# Daemonized GNU Radio applications may be stopped by sending them a
# SIGINT, SIGKILL, or SIGTERM, e.g., using 'kill pid' from the command line.
#
# If your application uses gr.top_block.run(), the flowgraph will be stopped
# and the function will return.  You should allow your daemon program to exit
# at this point.
#
# If your application uses gr.top_block.start(), you are responsible for hooking
# the Python signal handler (see 'signal' module) and calling gr.top_block.stop()
# on your top block, and otherwise causing your daemon process to exit.
#

def daemonize(pidfile=None, logfile=None):
    # fork() into background
    try:
	pid = os.fork()
    except OSError, e:
	raise Exception, "%s [%d]" % (e.strerror, e.errno)

    if pid == 0:	# First child of first fork()
	# Become session leader of new session
	os.setsid()
	
	# fork() into background again
	try:
	    pid = os.fork()
	except OSError, e:
	    raise Exception, "%s [%d]" % (e.strerror, e.errno)

	if pid != 0:
	    os._exit(0) # Second child of second fork()

    else:		# Second child of first fork()
	os._exit(0)
	
    os.umask(0111)

    # Write pid
    pid = os.getpid()
    if pidfile is not None:
	open(pidfile, 'w').write('%d\n'%pid)
	
    # Redirect streams
    if logfile is not None:
	lf = open(logfile, 'a+')
	sys.stdout = lf
	sys.stderr = lf

    # Prevent pinning any filesystem mounts
    os.chdir('/')

    # Tell caller what pid to send future signals to
    return pid

if __name__ == "__main__":
    import time
    daemonize()
    print "Hello, world, from daemon process."
    time.sleep(20)
    print "Goodbye, world, from daemon process."
