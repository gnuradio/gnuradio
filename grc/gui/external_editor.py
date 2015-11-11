"""
Copyright 2015 Free Software Foundation, Inc.
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

import os
import sys
import time
import threading
import tempfile
import subprocess

import Constants


class ExternalEditor(threading.Thread):

    def __init__(self, name, value, callback):
        threading.Thread.__init__(self)
        self.daemon = True
        self._stop_event = threading.Event()

        self.callback = callback
        self.tempfile = self._create_tempfile(name, value)

    def _create_tempfile(self, name, value):
        fp = tempfile.NamedTemporaryFile(mode='w', suffix='.py',
                                         prefix=name + '_')
        fp.write(value)
        fp.flush()
        return fp

    @property
    def filename(self):
        return self.tempfile.name

    def open_editor(self):
        proc = subprocess.Popen(
            args=(Constants.EDITOR, self.filename)
        )
        proc.poll()
        return proc

    def stop(self):
        self._stop_event.set()

    def run(self):
        filename = self.filename
        # print "file monitor: started for", filename
        last_change = os.path.getmtime(filename)
        try:
            while not self._stop_event.is_set():
                mtime = os.path.getmtime(filename)
                if mtime > last_change:
                    # print "file monitor: reload trigger for", filename
                    last_change = mtime
                    with open(filename) as fp:
                        data = fp.read()
                    self.callback(data)
                time.sleep(1)

        except Exception as e:
            print >> sys.stderr, "file monitor crashed:", str(e)
        else:
            # print "file monitor: done with", filename
            pass


if __name__ == '__main__':
    def p(data):
        print data

    Constants.EDITOR = '/usr/bin/gedit'
    editor = ExternalEditor("test", "content", p)
    editor.open_editor()
    editor.start()
    time.sleep(15)
    editor.stop()
    editor.join()
