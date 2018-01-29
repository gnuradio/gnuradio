"""Thread module emulating a subset of Java's threading model."""

# This started life as the threading.py module of Python 2.3
# It's been patched to fix a problem with join, where a KeyboardInterrupt
# caused a lock to be left in the acquired state.

import sys as _sys

try:
    import thread
except ImportError:
    del _sys.modules[__name__]
    raise

from StringIO import StringIO as _StringIO
from time import time as _time, sleep as _sleep
from traceback import print_exc as _print_exc

# Rename some stuff so "from threading import *" is safe
__all__ = ['activeCount', 'Condition', 'currentThread', 'enumerate', 'Event',
           'Lock', 'RLock', 'Semaphore', 'BoundedSemaphore', 'Thread',
           'Timer', 'setprofile', 'settrace']

_start_new_thread = thread.start_new_thread
_allocate_lock = thread.allocate_lock
_get_ident = thread.get_ident
ThreadError = thread.error
del thread


# Debug support (adapted from ihooks.py).
# All the major classes here derive from _Verbose.  We force that to
# be a new-style class so that all the major classes here are new-style.
# This helps debugging (type(instance) is more revealing for instances
# of new-style classes).

_VERBOSE = False

if __debug__:

    class _Verbose(object):

        def __init__(self, verbose=None):
            if verbose is None:
                verbose = _VERBOSE
            self.__verbose = verbose

        def _note(self, format, *args):
            if self.__verbose:
                format = format % args
                format = "%s: %s\n" % (
                    currentThread().getName(), format)
                _sys.stderr.write(format)

else:
    # Disable this when using "python -O"
    class _Verbose(object):
        def __init__(self, verbose=None):
            pass
        def _note(self, *args):
            pass

# Support for profile and trace hooks

_profile_hook = None
_trace_hook = None

def setprofile(func):
    global _profile_hook
    _profile_hook = func

def settrace(func):
    global _trace_hook
    _trace_hook = func

# Synchronization classes

Lock = _allocate_lock

def RLock(*args, **kwargs):
    return _RLock(*args, **kwargs)

class _RLock(_Verbose):

    def __init__(self, verbose=None):
        _Verbose.__init__(self, verbose)
        self.__block = _allocate_lock()
        self.__owner = None
        self.__count = 0

    def __repr__(self):
        return "<%s(%s, %d)>" % (
                self.__class__.__name__,
                self.__owner and self.__owner.getName(),
                self.__count)

    def acquire(self, blocking=1):
        me = currentThread()
        if self.__owner is me:
            self.__count = self.__count + 1
            if __debug__:
                self._note("%s.acquire(%s): recursive success", self, blocking)
            return 1
        rc = self.__block.acquire(blocking)
        if rc:
            self.__owner = me
            self.__count = 1
            if __debug__:
                self._note("%s.acquire(%s): initial success", self, blocking)
        else:
            if __debug__:
                self._note("%s.acquire(%s): failure", self, blocking)
        return rc

    def release(self):
        me = currentThread()
        assert self.__owner is me, "release() of un-acquire()d lock"
        self.__count = count = self.__count - 1
        if not count:
            self.__owner = None
            self.__block.release()
            if __debug__:
                self._note("%s.release(): final release", self)
        else:
            if __debug__:
                self._note("%s.release(): non-final release", self)

    # Internal methods used by condition variables

    def _acquire_restore(self, (count, owner)):
        self.__block.acquire()
        self.__count = count
        self.__owner = owner
        if __debug__:
            self._note("%s._acquire_restore()", self)

    def _release_save(self):
        if __debug__:
            self._note("%s._release_save()", self)
        count = self.__count
        self.__count = 0
        owner = self.__owner
        self.__owner = None
        self.__block.release()
        return (count, owner)

    def _is_owned(self):
        return self.__owner is currentThread()


def Condition(*args, **kwargs):
    return _Condition(*args, **kwargs)

class _Condition(_Verbose):

    def __init__(self, lock=None, verbose=None):
        _Verbose.__init__(self, verbose)
        if lock is None:
            lock = RLock()
        self.__lock = lock
        # Export the lock's acquire() and release() methods
        self.acquire = lock.acquire
        self.release = lock.release
        # If the lock defines _release_save() and/or _acquire_restore(),
        # these override the default implementations (which just call
        # release() and acquire() on the lock).  Ditto for _is_owned().
        try:
            self._release_save = lock._release_save
        except AttributeError:
            pass
        try:
            self._acquire_restore = lock._acquire_restore
        except AttributeError:
            pass
        try:
            self._is_owned = lock._is_owned
        except AttributeError:
            pass
        self.__waiters = []

    def __repr__(self):
        return "<Condition(%s, %d)>" % (self.__lock, len(self.__waiters))

    def _release_save(self):
        self.__lock.release()           # No state to save

    def _acquire_restore(self, x):
        self.__lock.acquire()           # Ignore saved state

    def _is_owned(self):
        # Return True if lock is owned by currentThread.
        # This method is called only if __lock doesn't have _is_owned().
        if self.__lock.acquire(0):
            self.__lock.release()
            return False
        else:
            return True

    def wait(self, timeout=None):
        currentThread() # for side-effect
        assert self._is_owned(), "wait() of un-acquire()d lock"
        waiter = _allocate_lock()
        waiter.acquire()
        self.__waiters.append(waiter)
        saved_state = self._release_save()
        try:    # restore state no matter what (e.g., KeyboardInterrupt)
            if timeout is None:
                waiter.acquire()
                if __debug__:
                    self._note("%s.wait(): got it", self)
            else:
                # Balancing act:  We can't afford a pure busy loop, so we
                # have to sleep; but if we sleep the whole timeout time,
                # we'll be unresponsive.  The scheme here sleeps very
                # little at first, longer as time goes on, but never longer
                # than 20 times per second (or the timeout time remaining).
                endtime = _time() + timeout
                delay = 0.0005 # 500 us -> initial delay of 1 ms
                while True:
                    gotit = waiter.acquire(0)
                    if gotit:
                        break
                    remaining = endtime - _time()
                    if remaining <= 0:
                        break
                    delay = min(delay * 2, remaining, .05)
                    _sleep(delay)
                if not gotit:
                    if __debug__:
                        self._note("%s.wait(%s): timed out", self, timeout)
                    try:
                        self.__waiters.remove(waiter)
                    except ValueError:
                        pass
                else:
                    if __debug__:
                        self._note("%s.wait(%s): got it", self, timeout)
        finally:
            self._acquire_restore(saved_state)

    def notify(self, n=1):
        currentThread() # for side-effect
        assert self._is_owned(), "notify() of un-acquire()d lock"
        __waiters = self.__waiters
        waiters = __waiters[:n]
        if not waiters:
            if __debug__:
                self._note("%s.notify(): no waiters", self)
            return
        self._note("%s.notify(): notifying %d waiter%s", self, n,
                   n!=1 and "s" or "")
        for waiter in waiters:
            waiter.release()
            try:
                __waiters.remove(waiter)
            except ValueError:
                pass

    def notifyAll(self):
        self.notify(len(self.__waiters))


def Semaphore(*args, **kwargs):
    return _Semaphore(*args, **kwargs)

class _Semaphore(_Verbose):

    # After Tim Peters' semaphore class, but not quite the same (no maximum)

    def __init__(self, value=1, verbose=None):
        assert value >= 0, "Semaphore initial value must be >= 0"
        _Verbose.__init__(self, verbose)
        self.__cond = Condition(Lock())
        self.__value = value

    def acquire(self, blocking=1):
        rc = False
        self.__cond.acquire()
        while self.__value == 0:
            if not blocking:
                break
            if __debug__:
                self._note("%s.acquire(%s): blocked waiting, value=%s",
                           self, blocking, self.__value)
            self.__cond.wait()
        else:
            self.__value = self.__value - 1
            if __debug__:
                self._note("%s.acquire: success, value=%s",
                           self, self.__value)
            rc = True
        self.__cond.release()
        return rc

    def release(self):
        self.__cond.acquire()
        self.__value = self.__value + 1
        if __debug__:
            self._note("%s.release: success, value=%s",
                       self, self.__value)
        self.__cond.notify()
        self.__cond.release()


def BoundedSemaphore(*args, **kwargs):
    return _BoundedSemaphore(*args, **kwargs)

class _BoundedSemaphore(_Semaphore):
    """Semaphore that checks that # releases is <= # acquires"""
    def __init__(self, value=1, verbose=None):
        _Semaphore.__init__(self, value, verbose)
        self._initial_value = value

    def release(self):
        if self._Semaphore__value >= self._initial_value:
            raise ValueError, "Semaphore released too many times"
        return _Semaphore.release(self)


def Event(*args, **kwargs):
    return _Event(*args, **kwargs)

class _Event(_Verbose):

    # After Tim Peters' event class (without is_posted())

    def __init__(self, verbose=None):
        _Verbose.__init__(self, verbose)
        self.__cond = Condition(Lock())
        self.__flag = False

    def isSet(self):
        return self.__flag

    def set(self):
        self.__cond.acquire()
        try:
            self.__flag = True
            self.__cond.notifyAll()
        finally:
            self.__cond.release()

    def clear(self):
        self.__cond.acquire()
        try:
            self.__flag = False
        finally:
            self.__cond.release()

    def wait(self, timeout=None):
        self.__cond.acquire()
        try:
            if not self.__flag:
                self.__cond.wait(timeout)
        finally:
            self.__cond.release()

# Helper to generate new thread names
_counter = 0
def _newname(template="Thread-%d"):
    global _counter
    _counter = _counter + 1
    return template % _counter

# Active thread administration
_active_limbo_lock = _allocate_lock()
_active = {}
_limbo = {}


# Main class for threads

class Thread(_Verbose):

    __initialized = False

    def __init__(self, group=None, target=None, name=None,
                 args=(), kwargs={}, verbose=None):
        assert group is None, "group argument must be None for now"
        _Verbose.__init__(self, verbose)
        self.__target = target
        self.__name = str(name or _newname())
        self.__args = args
        self.__kwargs = kwargs
        self.__daemonic = self._set_daemon()
        self.__started = False
        self.__stopped = False
        self.__block = Condition(Lock())
        self.__initialized = True

    def _set_daemon(self):
        # Overridden in _MainThread and _DummyThread
        return currentThread().isDaemon()

    def __repr__(self):
        assert self.__initialized, "Thread.__init__() was not called"
        status = "initial"
        if self.__started:
            status = "started"
        if self.__stopped:
            status = "stopped"
        if self.__daemonic:
            status = status + " daemon"
        return "<%s(%s, %s)>" % (self.__class__.__name__, self.__name, status)

    def start(self):
        assert self.__initialized, "Thread.__init__() not called"
        assert not self.__started, "thread already started"
        if __debug__:
            self._note("%s.start(): starting thread", self)
        _active_limbo_lock.acquire()
        _limbo[self] = self
        _active_limbo_lock.release()
        _start_new_thread(self.__bootstrap, ())
        self.__started = True
        _sleep(0.000001)    # 1 usec, to let the thread run (Solaris hack)

    def run(self):
        if self.__target:
            self.__target(*self.__args, **self.__kwargs)

    def __bootstrap(self):
        try:
            self.__started = True
            _active_limbo_lock.acquire()
            _active[_get_ident()] = self
            del _limbo[self]
            _active_limbo_lock.release()
            if __debug__:
                self._note("%s.__bootstrap(): thread started", self)

            if _trace_hook:
                self._note("%s.__bootstrap(): registering trace hook", self)
                _sys.settrace(_trace_hook)
            if _profile_hook:
                self._note("%s.__bootstrap(): registering profile hook", self)
                _sys.setprofile(_profile_hook)

            try:
                self.run()
            except SystemExit:
                if __debug__:
                    self._note("%s.__bootstrap(): raised SystemExit", self)
            except:
                if __debug__:
                    self._note("%s.__bootstrap(): unhandled exception", self)
                s = _StringIO()
                _print_exc(file=s)
                _sys.stderr.write("Exception in thread %s:\n%s\n" %
                                 (self.getName(), s.getvalue()))
            else:
                if __debug__:
                    self._note("%s.__bootstrap(): normal return", self)
        finally:
            self.__stop()
            try:
                self.__delete()
            except:
                pass

    def __stop(self):
        self.__block.acquire()
        self.__stopped = True
        self.__block.notifyAll()
        self.__block.release()

    def __delete(self):
        _active_limbo_lock.acquire()
        del _active[_get_ident()]
        _active_limbo_lock.release()

    def join(self, timeout=None):
        assert self.__initialized, "Thread.__init__() not called"
        assert self.__started, "cannot join thread before it is started"
        assert self is not currentThread(), "cannot join current thread"
        if __debug__:
            if not self.__stopped:
                self._note("%s.join(): waiting until thread stops", self)
        self.__block.acquire()
        try:
            if timeout is None:
                while not self.__stopped:
                    self.__block.wait()
                if __debug__:
                    self._note("%s.join(): thread stopped", self)
            else:
                deadline = _time() + timeout
                while not self.__stopped:
                    delay = deadline - _time()
                    if delay <= 0:
                        if __debug__:
                            self._note("%s.join(): timed out", self)
                        break
                    self.__block.wait(delay)
                else:
                    if __debug__:
                        self._note("%s.join(): thread stopped", self)
        finally:
            self.__block.release()

    def getName(self):
        assert self.__initialized, "Thread.__init__() not called"
        return self.__name

    def setName(self, name):
        assert self.__initialized, "Thread.__init__() not called"
        self.__name = str(name)

    def isAlive(self):
        assert self.__initialized, "Thread.__init__() not called"
        return self.__started and not self.__stopped

    def isDaemon(self):
        assert self.__initialized, "Thread.__init__() not called"
        return self.__daemonic

    def setDaemon(self, daemonic):
        assert self.__initialized, "Thread.__init__() not called"
        assert not self.__started, "cannot set daemon status of active thread"
        self.__daemonic = daemonic

# The timer class was contributed by Itamar Shtull-Trauring

def Timer(*args, **kwargs):
    return _Timer(*args, **kwargs)

class _Timer(Thread):
    """Call a function after a specified number of seconds:

    t = Timer(30.0, f, args=[], kwargs={})
    t.start()
    t.cancel() # stop the timer's action if it's still waiting
    """

    def __init__(self, interval, function, args=[], kwargs={}):
        Thread.__init__(self)
        self.interval = interval
        self.function = function
        self.args = args
        self.kwargs = kwargs
        self.finished = Event()

    def cancel(self):
        """Stop the timer if it hasn't finished yet"""
        self.finished.set()

    def run(self):
        self.finished.wait(self.interval)
        if not self.finished.isSet():
            self.function(*self.args, **self.kwargs)
        self.finished.set()

# Special thread class to represent the main thread
# This is garbage collected through an exit handler

class _MainThread(Thread):

    def __init__(self):
        Thread.__init__(self, name="MainThread")
        self._Thread__started = True
        _active_limbo_lock.acquire()
        _active[_get_ident()] = self
        _active_limbo_lock.release()
        import atexit
        atexit.register(self.__exitfunc)

    def _set_daemon(self):
        return False

    def __exitfunc(self):
        self._Thread__stop()
        t = _pickSomeNonDaemonThread()
        if t:
            if __debug__:
                self._note("%s: waiting for other threads", self)
        while t:
            t.join()
            t = _pickSomeNonDaemonThread()
        if __debug__:
            self._note("%s: exiting", self)
        self._Thread__delete()

def _pickSomeNonDaemonThread():
    for t in enumerate():
        if not t.isDaemon() and t.isAlive():
            return t
    return None


# Dummy thread class to represent threads not started here.
# These aren't garbage collected when they die,
# nor can they be waited for.
# Their purpose is to return *something* from currentThread().
# They are marked as daemon threads so we won't wait for them
# when we exit (conform previous semantics).

class _DummyThread(Thread):

    def __init__(self):
        Thread.__init__(self, name=_newname("Dummy-%d"))
        self._Thread__started = True
        _active_limbo_lock.acquire()
        _active[_get_ident()] = self
        _active_limbo_lock.release()

    def _set_daemon(self):
        return True

    def join(self, timeout=None):
        assert False, "cannot join a dummy thread"


# Global API functions

def currentThread():
    try:
        return _active[_get_ident()]
    except KeyError:
        ##print "currentThread(): no current thread for", _get_ident()
        return _DummyThread()

def activeCount():
    _active_limbo_lock.acquire()
    count = len(_active) + len(_limbo)
    _active_limbo_lock.release()
    return count

def enumerate():
    _active_limbo_lock.acquire()
    active = _active.values() + _limbo.values()
    _active_limbo_lock.release()
    return active

# Create the main thread object

_MainThread()


# Self-test code

def _test():

    class BoundedQueue(_Verbose):

        def __init__(self, limit):
            _Verbose.__init__(self)
            self.mon = RLock()
            self.rc = Condition(self.mon)
            self.wc = Condition(self.mon)
            self.limit = limit
            self.queue = []

        def put(self, item):
            self.mon.acquire()
            while len(self.queue) >= self.limit:
                self._note("put(%s): queue full", item)
                self.wc.wait()
            self.queue.append(item)
            self._note("put(%s): appended, length now %d",
                       item, len(self.queue))
            self.rc.notify()
            self.mon.release()

        def get(self):
            self.mon.acquire()
            while not self.queue:
                self._note("get(): queue empty")
                self.rc.wait()
            item = self.queue.pop(0)
            self._note("get(): got %s, %d left", item, len(self.queue))
            self.wc.notify()
            self.mon.release()
            return item

    class ProducerThread(Thread):

        def __init__(self, queue, quota):
            Thread.__init__(self, name="Producer")
            self.queue = queue
            self.quota = quota

        def run(self):
            from random import random
            counter = 0
            while counter < self.quota:
                counter = counter + 1
                self.queue.put("%s.%d" % (self.getName(), counter))
                _sleep(random() * 0.00001)


    class ConsumerThread(Thread):

        def __init__(self, queue, count):
            Thread.__init__(self, name="Consumer")
            self.queue = queue
            self.count = count

        def run(self):
            while self.count > 0:
                item = self.queue.get()
                print item
                self.count = self.count - 1

    NP = 3
    QL = 4
    NI = 5

    Q = BoundedQueue(QL)
    P = []
    for i in range(NP):
        t = ProducerThread(Q, NI)
        t.setName("Producer-%d" % (i+1))
        P.append(t)
    C = ConsumerThread(Q, NI*NP)
    for t in P:
        t.start()
        _sleep(0.000001)
    C.start()
    for t in P:
        t.join()
    C.join()

if __name__ == '__main__':
    _test()
