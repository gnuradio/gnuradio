"""
Copyright 2016 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-3.0-or-later
"""

import subprocess
import threading
import time
from pathlib import Path

from gnuradio import gr

from ...core import Messages


class ExecFlowGraphThread(threading.Thread):
    """Execute the flow graph as a new process and wait on it to finish."""

    def __init__(self, view, flowgraph, xterm_executable, update_gui_callback):
        """
        ExecFlowGraphThread constructor.
        """
        threading.Thread.__init__(self)

        self.view = view
        self.flow_graph = flowgraph
        self.xterm_executable = xterm_executable or gr.prefs().get_string('grc', 'xterm_executable', 'xterm')
        self.update_callback = update_gui_callback
        self.generator = self.view.get_generator()

        try:
            self.process = self._popen(self.generator.get_exec_args())
            if not self.process:
                return

            self.update_callback()
            self.start()
        except Exception as e:
            Messages.send_verbose_exec(str(e))
            Messages.send_end_exec()

    def _popen(self, exec_args):
        """Execute this flow graph."""
        if not exec_args:
            return None

        def sanitize_popen_args(args):
            """Sanitize the arguments for subprocess.Popen."""
            forbidden_args = ['stdout', 'stderr', ]
            for a in forbidden_args:
                args.pop(a, None)
            if 'cwd' not in args:
                args['cwd'] = Path(self.generator.file_path).parent
            return args
        cmd_args = exec_args.pop("args")
        exec_args = sanitize_popen_args(exec_args)

        # this does not reproduce a shell executable command string, if
        # a graphical terminal is used. Passing run_command though shlex_quote
        # would do it but it looks really ugly and confusing in the console.
        if isinstance(cmd_args, list):
            Messages.send_start_exec(' '.join(cmd_args))
        else:
            Messages.send_start_exec(cmd_args)

        return subprocess.Popen(
            args=cmd_args,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
            **exec_args
        )

    def run(self):
        """
        Wait on the executing process by reading from its stdout.
        """
        # handle completion
        r = "\n"
        while r:
            Messages.send_verbose_exec(r)
            r = self.process.stdout.read(1)

        # Properly close pipe before thread is terminated
        self.process.stdout.close()
        while self.process.poll() is None:
            # Wait for the process to fully terminate
            time.sleep(0.05)

        self.done

    def done(self):
        """Perform end of execution tasks."""
        Messages.send_end_exec(self.process.returncode)
        self.view.process = None
        self.update_callback()
