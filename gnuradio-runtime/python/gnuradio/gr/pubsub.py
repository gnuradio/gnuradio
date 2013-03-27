#!/usr/bin/env python
#
# Copyright 2008,2009 Free Software Foundation, Inc.
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

"""
Abstract GNU Radio publisher/subscriber interface

This is a proof of concept implementation, will likely change significantly.
"""

class pubsub(dict):
    def __init__(self):
	self._publishers = { }
	self._subscribers = { }
	self._proxies = { }

    def __missing__(self, key, value=None):
	dict.__setitem__(self, key, value)
	self._publishers[key] = None
	self._subscribers[key] = []
	self._proxies[key] = None

    def __setitem__(self, key, val):
	if not self.has_key(key):
	    self.__missing__(key, val)
	elif self._proxies[key] is not None:
	    (p, newkey) = self._proxies[key]
	    p[newkey] = val
	else:
	    dict.__setitem__(self, key, val)
	for sub in self._subscribers[key]:
	    # Note this means subscribers will get called in the thread
	    # context of the 'set' caller.
	    sub(val)

    def __getitem__(self, key):
	if not self.has_key(key): self.__missing__(key)
	if self._proxies[key] is not None:
	    (p, newkey) = self._proxies[key]
	    return p[newkey]
	elif self._publishers[key] is not None:
	    return self._publishers[key]()
	else:
	    return dict.__getitem__(self, key)

    def publish(self, key, publisher):
	if not self.has_key(key): self.__missing__(key)
        if self._proxies[key] is not None:
            (p, newkey) = self._proxies[key]
            p.publish(newkey, publisher)
        else:
            self._publishers[key] = publisher

    def subscribe(self, key, subscriber):
	if not self.has_key(key): self.__missing__(key)
        if self._proxies[key] is not None:
            (p, newkey) = self._proxies[key]
            p.subscribe(newkey, subscriber)
        else:
            self._subscribers[key].append(subscriber)

    def unpublish(self, key):
        if self._proxies[key] is not None:
            (p, newkey) = self._proxies[key]
            p.unpublish(newkey)
        else:
            self._publishers[key] = None

    def unsubscribe(self, key, subscriber):
        if self._proxies[key] is not None:
            (p, newkey) = self._proxies[key]
            p.unsubscribe(newkey, subscriber)
        else:
            self._subscribers[key].remove(subscriber)

    def proxy(self, key, p, newkey=None):
	if not self.has_key(key): self.__missing__(key)
	if newkey is None: newkey = key
	self._proxies[key] = (p, newkey)

    def unproxy(self, key):
        self._proxies[key] = None

# Test code
if __name__ == "__main__":
    import sys
    o = pubsub()

    # Non-existent key gets auto-created with None value
    print "Auto-created key 'foo' value:", o['foo']

    # Add some subscribers
    # First is a bare function
    def print_len(x):
	print "len=%i" % (len(x), )
    o.subscribe('foo', print_len)

    # The second is a class member function
    class subber(object):
	def __init__(self, param):
	    self._param = param
	def printer(self, x):
	    print self._param, `x`
    s = subber('param')
    o.subscribe('foo', s.printer)

    # The third is a lambda function
    o.subscribe('foo', lambda x: sys.stdout.write('val='+`x`+'\n'))

    # Update key 'foo', will notify subscribers
    print "Updating 'foo' with three subscribers:"
    o['foo'] = 'bar';

    # Remove first subscriber
    o.unsubscribe('foo', print_len)

    # Update now will only trigger second and third subscriber
    print "Updating 'foo' after removing a subscriber:"
    o['foo'] = 'bar2';

    # Publish a key as a function, in this case, a lambda function
    o.publish('baz', lambda : 42)
    print "Published value of 'baz':", o['baz']

    # Unpublish the key
    o.unpublish('baz')

    # This will return None, as there is no publisher
    print "Value of 'baz' with no publisher:", o['baz']

    # Set 'baz' key, it gets cached
    o['baz'] = 'bazzz'

    # Now will return cached value, since no provider
    print "Cached value of 'baz' after being set:", o['baz']
