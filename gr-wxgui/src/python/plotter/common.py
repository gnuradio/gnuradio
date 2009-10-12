#
# Copyright 2009 Free Software Foundation, Inc.
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

import threading
import time
import math
import wx

##################################################
# Number formatting
##################################################
def get_exp(num):
	"""
	Get the exponent of the number in base 10.
	@param num the floating point number
	@return the exponent as an integer
	"""
	if num == 0: return 0
	return int(math.floor(math.log10(abs(num))))

def get_si_components(num):
	"""
	Get the SI units for the number.
	Extract the coeff and exponent of the number.
	The exponent will be a multiple of 3.
	@param num the floating point number
	@return the tuple coeff, exp, prefix
	"""
	num = float(num)
	exp = get_exp(num)
	exp -= exp%3
	exp = min(max(exp, -24), 24) #bounds on SI table below
	prefix = {
		24: 'Y', 21: 'Z',
		18: 'E', 15: 'P',
		12: 'T', 9: 'G',
		6: 'M', 3: 'k',
		0: '',
		-3: 'm', -6: 'u',
		-9: 'n', -12: 'p',
		-15: 'f', -18: 'a',
		-21: 'z', -24: 'y',
	}[exp]
	coeff = num/10**exp
	return coeff, exp, prefix

def sci_format(num):
	"""
	Format a floating point number into scientific notation.
	@param num the number to format
	@return a label string
	"""
	coeff, exp, prefix = get_si_components(num)
	if -3 <= exp < 3: return '%g'%num
	return '%.3ge%d'%(coeff, exp)

def eng_format(num, units=''):
	"""
	Format a floating point number into engineering notation.
	@param num the number to format
	@param units the units to append
	@return a label string
	"""
	coeff, exp, prefix = get_si_components(num)
	if -3 <= exp < 3: return '%g'%num
	return '%g%s%s%s'%(coeff, units and ' ' or '', prefix, units)

##################################################
# Interface with thread safe lock/unlock
##################################################
class mutex(object):
	_lock = threading.Lock()
	def lock(self): self._lock.acquire()
	def unlock(self): self._lock.release()

##################################################
# Periodic update thread for point label
##################################################
class point_label_thread(threading.Thread, mutex):

	def __init__(self, plotter):
		self._plotter = plotter
		self._coor_queue = list()
		#bind plotter mouse events
		self._plotter.Bind(wx.EVT_MOTION, lambda evt: self.enqueue(evt.GetPosition()))
		self._plotter.Bind(wx.EVT_LEAVE_WINDOW, lambda evt: self.enqueue(None))
		self._plotter.Bind(wx.EVT_RIGHT_DOWN, lambda evt: plotter.enable_point_label(not plotter.enable_point_label()))
		#start the thread
		threading.Thread.__init__(self)
		self.start()

	def enqueue(self, coor):
		self.lock()
		self._coor_queue.append(coor)
		self.unlock()

	def run(self):
		last_ts = time.time()
		last_coor = coor = None
		try: 
			while True:
				time.sleep(1.0/30.0)
				self.lock()
				#get most recent coor change
				if self._coor_queue:
					coor = self._coor_queue[-1]
					self._coor_queue = list()
				self.unlock()
				#update if coor change, or enough time expired
				if last_coor != coor or (time.time() - last_ts) > (1.0/2.0):
					self._plotter.set_point_label_coordinate(coor)
					last_coor = coor
					last_ts = time.time()
		except wx.PyDeadObjectError: pass
