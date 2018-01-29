# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

import gobject
import os
import threading
import shlex
import subprocess
import sys
import re
from distutils.spawn import find_executable

from ..core import Messages


class ExecFlowGraphThread(threading.Thread):
    """Execute the flow graph as a new process and wait on it to finish."""

    def __init__(self, flow_graph_page, xterm_executable, callback):
        """
        ExecFlowGraphThread constructor.

        Args:
            action_handler: an instance of an ActionHandler
        """
        threading.Thread.__init__(self)

        self.page = flow_graph_page  # store page and don't use main window calls in run
        self.xterm_executable = xterm_executable
        self.update_callback = callback

        try:
            self.process = self._popen()
            self.page.set_proc(self.process)
            self.update_callback()
            self.start()
        except Exception as e:
            Messages.send_verbose_exec(str(e))
            Messages.send_end_exec()

    def _popen(self):
        """
        Execute this python flow graph.
        """
        run_command = self.page.get_flow_graph().get_option('run_command')
        generator = self.page.get_generator()

        try:
            run_command = run_command.format(
                python=shlex_quote(sys.executable),
                filename=shlex_quote(generator.file_path))
            run_command_args = shlex.split(run_command)
        except Exception as e:
            raise ValueError("Can't parse run command {!r}: {0}".format(run_command, e))

        # When in no gui mode on linux, use a graphical terminal (looks nice)
        xterm_executable = find_executable(self.xterm_executable)
        if generator.generate_options == 'no_gui' and xterm_executable:
            run_command_args = [xterm_executable, '-e', run_command]

        # this does not reproduce a shell executable command string, if a graphical
        # terminal is used. Passing run_command though shlex_quote would do it but
        # it looks really ugly and confusing in the console panel.
        Messages.send_start_exec(' '.join(run_command_args))

        return subprocess.Popen(
            args=run_command_args,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            shell=False, universal_newlines=True
        )

    def run(self):
        """
        Wait on the executing process by reading from its stdout.
        Use gobject.idle_add when calling functions that modify gtk objects.
        """
        # handle completion
        r = "\n"
        while r:
            gobject.idle_add(Messages.send_verbose_exec, r)
            r = os.read(self.process.stdout.fileno(), 1024)
        self.process.poll()
        gobject.idle_add(self.done)

    def done(self):
        """Perform end of execution tasks."""
        Messages.send_end_exec(self.process.returncode)
        self.page.set_proc(None)
        self.update_callback()


###########################################################
# back-port from python3
###########################################################
_find_unsafe = re.compile(r'[^\w@%+=:,./-]').search


def shlex_quote(s):
    """Return a shell-escaped version of the string *s*."""
    if not s:
        return "''"
    if _find_unsafe(s) is None:
        return s

    # use single quotes, and put single quotes into double quotes
    # the string $'b is then quoted as '$'"'"'b'
    return "'" + s.replace("'", "'\"'\"'") + "'"
