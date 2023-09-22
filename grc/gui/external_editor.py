"""
Copyright 2015 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

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
            print("file monitor crashed:", str(e), file=sys.stderr)
        finally:
            try:
                os.remove(self.filename)
            except OSError:
                pass


if __name__ == '__main__':
    e = ExternalEditor('/usr/bin/gedit', "test", "content", print)
    e.open_editor()
    e.start()
    time.sleep(15)
    e.stop()
    e.join()
