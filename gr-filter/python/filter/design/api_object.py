# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

class ApiObject(object):
    '''
        Filter count variable if the filter design tool has to
        return multiple filter parameters in future
        e.g Cascaded Filters
    '''

    def __init__(self, filtcount=1):
        self.filtercount = filtcount
        self.restype = [''] * self.filtercount
        self.params = [''] * self.filtercount
        self.taps = [''] * self.filtercount

    '''
        Updates params dictionary for the given filter number
    '''

    def update_params(self, params, filtno):
        if (filtno <= self.filtercount):
            self.params[filtno - 1] = params

    '''
        Updates filter type  for the given filter number
    '''

    def update_filttype(self, filttype, filtno):
        if (filtno <= self.filtercount):
            self.filttype[filtno - 1] = filttype

    '''
        updates taps for the given filter number. taps will
        contain a list of coefficients in the case of fir design
        and (b,a) tuple in the case of iir design
    '''

    def update_taps(self, taps, filtno):
        if (filtno <= self.filtercount):
            self.taps[filtno - 1] = taps

    '''
        updates  all of them in a single call
    '''

    def update_all(self, filttype, params, taps, filtno):
        if (filtno <= self.filtercount):
            self.taps[filtno - 1] = taps
            self.params[filtno - 1] = params
            self.restype[filtno - 1] = filttype

    def get_filtercount(self):
        return self.filtercount

    def get_restype(self, filtno=1):
        if (filtno <= self.filtercount):
            return self.restype[filtno - 1]

    def get_params(self, filtno=1):
        if (filtno <= self.filtercount):
            return self.params[filtno - 1]

    def get_taps(self, filtno=1):
        if (filtno <= self.filtercount):
            return self.taps[filtno - 1]
