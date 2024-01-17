import time


class StopWatch(object):
    '''
    Tool for tracking/profiling application execution. Once initialized, this tracks elapsed
    time between "laps" which can be given a name and accessed at a later point.
    '''

    def __init__(self):
        self._laps = {}
        self._previous = time.time()

    def lap(self, name):
        # Do as little as possible since this is timing things.
        # Save the previous and current time, then overwrite the previous
        lap = (self._previous, time.time())
        self._previous = lap[1]
        self._laps[name] = lap

    def elapsed(self, name):
        # If the lap isn't defined, this should throw a key error
        # Don't worry about catching it here
        start, stop = self._laps[name]
        return stop - start

