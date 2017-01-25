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


class ExternalEditor(threading.Thread):

    def __init__(self, editor, name, value, callback):
        threading.Thread.__init__(self)
        self.daemon = True
        self._stop_event = threading.Event()

        self.editor = editor
        self.callback = callback
        self.filename = self._create_tempfile(name, value)

    def _create_tempfile(self, name, value):
        with tempfile.NamedTemporaryFile(
            mode='wb', prefix=name + '_', suffix='.py', delete=False,
        ) as fp:
            fp.write(value.encode('utf-8'))
            return fp.name

    def open_editor(self):
        proc = subprocess.Popen(args=(self.editor, self.filename))
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
                    with open(filename, 'rb') as fp:
                        data = fp.read().decode('utf-8')
                    self.callback(data)
                time.sleep(1)

        except Exception as e:
            print >> sys.stderr, "file monitor crashed:", str(e)
        finally:
            try:
                os.remove(self.filename)
            except OSError:
                pass


if __name__ == '__main__':
    def p(data):
        print data

    e = ExternalEditor('/usr/bin/gedit', "test", "content", p)
    e.open_editor()
    e.start()
    time.sleep(15)
    e.stop()
    e.join()
