"""
Copyright 2016 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later
"""

import subprocess
import threading
import time
from pathlib import Path

from gi.repository import GLib

from ..core import Messages


class ExecFlowGraphThread(threading.Thread):
    """Execute the flow graph as a new process and wait on it to finish."""

    _OUTPUT_BATCH_CHARS = 8192
    _OUTPUT_BATCH_DELAY = 0.05

    def __init__(self, flow_graph_page, xterm_executable, update_gui_callback):
        """Initialize ExecFlowGraphThread.

        Args:
            flow_graph_page: Reference to the flow graph page object.
            xterm_executable: Path to the xterm executable.
            update_gui_callback: Callback function to update the GUI. To be
                                called after the process has started and finished.
        """
        threading.Thread.__init__(self)

        self.page = flow_graph_page  # store page and don't use main window calls in run
        self.flow_graph = self.page.flow_graph
        self.xterm_executable = xterm_executable
        self.update_callback = update_gui_callback
        self.generator = self.page.get_generator()
        
        self._pending_output = []
        self._pending_output_lock = threading.Lock()
        self._output_flush_id = None

        try:
            self.process = self.page.process = self._popen(self.generator.get_exec_args())
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

    def _queue_output(self, text):
        if not text:
            return

        with self._pending_output_lock:
            self._pending_output.append(text)
            if self._output_flush_id is not None:
                return
            self._output_flush_id = GLib.idle_add(self._flush_queued_output)

    def _flush_queued_output(self):
        with self._pending_output_lock:
            output = ''.join(self._pending_output)
            self._pending_output = []

        if output:
            Messages.send_verbose_exec(output)
            
        with self._pending_output_lock:
            self._output_flush_id = None
            
        return GLib.SOURCE_REMOVE

    def _finish(self):
        self._flush_queued_output()
        self.done()
        return False

    def run(self):
        """
        Wait on the executing process by reading from its stdout.
        Use GObject.idle_add when calling functions that modify gtk objects.
        """
        self._queue_output("\n")

        pending = []
        pending_backspace = None
        flush_deadline = time.monotonic() + self._OUTPUT_BATCH_DELAY

        while True:
            try:
                chunk = self.process.stdout.buffer.read1(8192)
            except (ValueError, AttributeError):
                break

            if not chunk:
                break

            text = chunk.decode('utf-8', errors='replace')

            for r in text:
                is_backspace = (r == '\b')

                if pending and is_backspace != pending_backspace:
                    self._queue_output(''.join(pending))
                    pending = []
                    flush_deadline = time.monotonic() + self._OUTPUT_BATCH_DELAY

                pending.append(r)
                pending_backspace = is_backspace

            now = time.monotonic()
            if len(pending) >= self._OUTPUT_BATCH_CHARS or now >= flush_deadline:
                self._queue_output(''.join(pending))
                pending = []
                pending_backspace = None
                flush_deadline = now + self._OUTPUT_BATCH_DELAY

        if pending:
            self._queue_output(''.join(pending))

        # Properly close pipe before thread is terminated
        self.process.stdout.close()
        while self.process.poll() is None:
            # Wait for the process to fully terminate
            time.sleep(0.05)

        GLib.idle_add(self._finish)

    def done(self):
        """Perform end of execution tasks."""
        Messages.send_end_exec(self.process.returncode)
        self.page.process = None
        self.update_callback()