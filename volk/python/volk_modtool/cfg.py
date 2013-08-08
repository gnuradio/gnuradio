#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

import ConfigParser
import sys
import os
import exceptions
import re


class volk_modtool_config:
    def key_val_sub(self, num, stuff, section):
        return re.sub('\$' + 'k' + str(num), stuff[num][0], (re.sub('\$' + str(num), stuff[num][1], section[1][num])));

    def verify(self):
        for i in self.verification:
            self.verify_section(i)
    def remap(self):
        for i in self.remapification:
            self.verify_section(i)
                    
    def verify_section(self, section):
        stuff = self.cfg.items(section[0])
        for i in range(len(section[1])):
            eval(self.key_val_sub(i, stuff, section))
            try:
               val = eval(self.key_val_sub(i, stuff, section))
               if val == False:
                   raise exceptions.ValueError
            except ValueError:
                raise exceptions.ValueError('Verification function returns False... key:%s, val:%s'%(stuff[i][0], stuff[i][1]))
            except:
                raise exceptions.IOError('bad configuration... key:%s, val:%s'%(stuff[i][0], stuff[i][1]))
           

    def __init__(self, cfg=None):
        self.config_name = 'config'
        self.config_defaults = ['name', 'destination', 'base']
        self.config_defaults_remap = ['1',
                                      'self.cfg.set(self.config_name, \'$k1\', os.path.realpath(os.path.expanduser(\'$1\')))', 
                                      'self.cfg.set(self.config_name, \'$k2\', os.path.realpath(os.path.expanduser(\'$2\')))'] 
                                      
        self.config_defaults_verify = ['re.match(\'[a-zA-Z0-9]+$\', \'$0\')',
                                       'os.path.exists(\'$1\')', 
                                       'os.path.exists(\'$2\')']
        self.remapification = [(self.config_name, self.config_defaults_remap)]
        self.verification = [(self.config_name, self.config_defaults_verify)]
        default = os.path.join(os.getcwd(), 'volk_modtool.cfg')
        icfg = ConfigParser.RawConfigParser()
        if cfg:
            icfg.read(cfg)
        elif os.path.exists(default):
            icfg.read(default)
        else:
            print "Initializing config file..."
            icfg.add_section(self.config_name)
            for kn in self.config_defaults:
                rv = raw_input("%s: "%(kn))
                icfg.set(self.config_name, kn, rv)
        self.cfg = icfg
        self.remap()
        self.verify()
        
            

    def read_map(self, name, inp):
        if self.cfg.has_section(name):
            self.cfg.remove_section(name)
        self.cfg.add_section(name)
        for i in inp:
            self.cfg.set(name, i, inp[i])

    def get_map(self, name):
        retval = {}
        stuff = self.cfg.items(name)
        for i in stuff:
            retval[i[0]] = i[1]
        return retval

    


    


