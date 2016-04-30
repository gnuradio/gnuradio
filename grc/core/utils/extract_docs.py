"""
Copyright 2008-2015 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import sys
import re
import subprocess
import threading
import json
import Queue
import random
import itertools


###############################################################################
# The docstring extraction
###############################################################################

def docstring_guess_from_key(key):
    """
    Extract the documentation from the python __doc__ strings
    By guessing module and constructor names from key

    Args:
        key: the block key

    Returns:
        a dict (block_name --> doc string)
    """
    doc_strings = dict()

    in_tree = [key.partition('_')[::2] + (
        lambda package: getattr(__import__('gnuradio.' + package), package),
    )]

    key_parts = key.split('_')
    oot = [
        ('_'.join(key_parts[:i]), '_'.join(key_parts[i:]), __import__)
        for i in range(1, len(key_parts))
    ]

    for module_name, init_name, importer in itertools.chain(in_tree, oot):
        if not module_name or not init_name:
            continue
        try:
            module = importer(module_name)
            break
        except ImportError:
            continue
    else:
        return doc_strings

    pattern = re.compile('^' + init_name.replace('_', '_*').replace('x', r'\w') + r'\w*$')
    for match in filter(pattern.match, dir(module)):
        try:
            doc_strings[match] = getattr(module, match).__doc__
        except AttributeError:
            continue

    return doc_strings


def docstring_from_make(key, imports, make):
    """
    Extract the documentation from the python __doc__ strings
    By importing it and checking a truncated make

    Args:
        key: the block key
        imports: a list of import statements (string) to execute
        make: block constructor template

    Returns:
        a list of tuples (block_name, doc string)
    """

    try:
        blk_cls = make.partition('(')[0].strip()
        if '$' in blk_cls:
            raise ValueError('Not an identifier')
        ns = dict()
        for _import in imports:
            exec(_import.strip(), ns)
        blk = eval(blk_cls, ns)
        doc_strings = {key: blk.__doc__}

    except (ImportError, AttributeError, SyntaxError, ValueError):
        doc_strings = docstring_guess_from_key(key)

    return doc_strings


###############################################################################
# Manage docstring extraction in separate process
###############################################################################

class SubprocessLoader(object):
    """
    Start and manage docstring extraction process
    Manages subprocess and handles RPC.
    """

    BOOTSTRAP = "import runpy; runpy.run_path({!r}, run_name='__worker__')"
    AUTH_CODE = random.random()  # sort out unwanted output of worker process
    RESTART = 5  # number of worker restarts before giving up
    DONE = object()  # sentinel value to signal end-of-queue

    def __init__(self, callback_query_result, callback_finished=None):
        self.callback_query_result = callback_query_result
        self.callback_finished = callback_finished or (lambda: None)

        self._queue = Queue.Queue()
        self._thread = None
        self._worker = None
        self._shutdown = threading.Event()
        self._last_cmd = None

    def start(self):
        """ Start the worker process handler thread """
        if self._thread is not None:
            return
        self._shutdown.clear()
        thread = self._thread = threading.Thread(target=self.run_worker)
        thread.daemon = True
        thread.start()

    def run_worker(self):
        """ Read docstring back from worker stdout and execute callback. """
        for _ in range(self.RESTART):
            if self._shutdown.is_set():
                break
            try:
                self._worker = subprocess.Popen(
                    args=(sys.executable, '-uc', self.BOOTSTRAP.format(__file__)),
                    stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE
                )
                self._handle_worker()

            except (OSError, IOError):
                msg = "Warning: restarting the docstring loader"
                cmd, args = self._last_cmd
                if cmd == 'query':
                    msg += " (crashed while loading {0!r})".format(args[0])
                print >> sys.stderr, msg
                continue  # restart
            else:
                break  # normal termination, return
            finally:
                self._worker.terminate()
        else:
            print >> sys.stderr, "Warning: docstring loader crashed too often"
        self._thread = None
        self._worker = None
        self.callback_finished()

    def _handle_worker(self):
        """ Send commands and responses back from worker. """
        assert '1' == self._worker.stdout.read(1)
        for cmd, args in iter(self._queue.get, self.DONE):
            self._last_cmd = cmd, args
            self._send(cmd, args)
            cmd, args = self._receive()
            self._handle_response(cmd, args)

    def _send(self, cmd, args):
        """ Send a command to worker's stdin """
        fd = self._worker.stdin
        json.dump((self.AUTH_CODE, cmd, args), fd)
        fd.write('\n'.encode())

    def _receive(self):
        """ Receive response from worker's stdout """
        for line in iter(self._worker.stdout.readline, ''):
            try:
                key, cmd, args = json.loads(line, encoding='utf-8')
                if key != self.AUTH_CODE:
                    raise ValueError('Got wrong auth code')
                return cmd, args
            except ValueError:
                continue  # ignore invalid output from worker
        else:
            raise IOError("Can't read worker response")

    def _handle_response(self, cmd, args):
        """ Handle response from worker, call the callback """
        if cmd == 'result':
            key, docs = args
            self.callback_query_result(key, docs)
        elif cmd == 'error':
            print args
        else:
            print >> sys.stderr, "Unknown response:", cmd, args

    def query(self, key, imports=None, make=None):
        """ Request docstring extraction for a certain key """
        if self._thread is None:
            self.start()
        if imports and make:
            self._queue.put(('query', (key, imports, make)))
        else:
            self._queue.put(('query_key_only', (key,)))

    def finish(self):
        """ Signal end of requests """
        self._queue.put(self.DONE)

    def wait(self):
        """ Wait for the handler thread to die """
        if self._thread:
            self._thread.join()

    def terminate(self):
        """ Terminate the worker and wait """
        self._shutdown.set()
        try:
            self._worker.terminate()
            self.wait()
        except (OSError, AttributeError):
            pass


###############################################################################
# Main worker entry point
###############################################################################

def worker_main():
    """
    Main entry point for the docstring extraction process.
    Manages RPC with main process through.
    Runs a docstring extraction for each key it read on stdin.
    """
    def send(cmd, args):
        json.dump((code, cmd, args), sys.stdout)
        sys.stdout.write('\n'.encode())

    sys.stdout.write('1')
    for line in iter(sys.stdin.readline, ''):
        code, cmd, args = json.loads(line, encoding='utf-8')
        try:
            if cmd == 'query':
                key, imports, make = args
                send('result', (key, docstring_from_make(key, imports, make)))
            elif cmd == 'query_key_only':
                key, = args
                send('result', (key, docstring_guess_from_key(key)))
            elif cmd == 'exit':
                break
        except Exception as e:
            send('error', repr(e))


if __name__ == '__worker__':
    worker_main()

elif __name__ == '__main__':
    def callback(key, docs):
        print key
        for match, doc in docs.iteritems():
            print '-->', match
            print doc.strip()
            print
        print

    r = SubprocessLoader(callback)

    # r.query('analog_feedforward_agc_cc')
    # r.query('uhd_source')
    r.query('expr_utils_graph')
    r.query('blocks_add_cc')
    r.query('blocks_add_cc', ['import gnuradio.blocks'], 'gnuradio.blocks.add_cc(')
    # r.query('analog_feedforward_agc_cc')
    # r.query('uhd_source')
    # r.query('uhd_source')
    # r.query('analog_feedforward_agc_cc')
    r.finish()
    # r.terminate()
    r.wait()
