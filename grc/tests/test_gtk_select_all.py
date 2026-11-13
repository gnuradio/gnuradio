# Copyright 2026 Nikkesh Parekh
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
"""
Ctrl+A in the GTK GRC must select the text of a focused text field, not the
whole flow graph.

These tests send a real key-press event to the main window, which goes through
the application accelerators first and then to the focused widget, the same
way a user's keystroke does. Triggering the select-all action directly would
skip that routing.
"""

import pytest

gi = pytest.importorskip("gi")
gi.require_version('Gtk', '3.0')
gi.require_version('Gdk', '3.0')
gi.require_version('PangoCairo', '1.0')
from gi.repository import Gtk, Gdk, Gio  # noqa: E402

if not Gtk.init_check()[0]:
    pytest.skip("GTK GRC tests need a display (try xvfb-run)",
                allow_module_level=True)

from gnuradio import gr  # noqa: E402


def flush():
    while Gtk.events_pending():
        Gtk.main_iteration_do(False)


def press_ctrl_a(window):
    """Send Ctrl+A to the main window like a keyboard would."""
    display = window.get_display()
    keymap = Gdk.Keymap.get_for_display(display)
    _, keys = keymap.get_entries_for_keyval(Gdk.KEY_a)

    event = Gdk.Event.new(Gdk.EventType.KEY_PRESS)
    event.window = window.get_window()
    event.send_event = True
    event.time = Gdk.CURRENT_TIME
    event.state = Gdk.ModifierType.CONTROL_MASK
    event.keyval = Gdk.KEY_a
    event.hardware_keycode = keys[0].keycode
    event.group = keys[0].group
    event.set_device(display.get_default_seat().get_keyboard())
    window.event(event)
    flush()


def selection_of(editable):
    sel = editable.get_selection_bounds()
    return (sel[-2], sel[-1]) if len(sel) >= 2 else (0, 0)


@pytest.fixture(scope='module')
def app():
    from grc.gui.Platform import Platform
    from grc.gui.Application import Application

    platform = Platform(
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(),
                       gr.minor_version()),
        prefs=gr.prefs(),
        install_prefix=gr.prefix(),
    )
    platform.build_library()

    application = Application([], platform)
    Gio.Application.set_default(application)
    application.register(None)
    application.activate()
    flush()
    application.main_window.show_all()
    flush()

    yield application

    application.main_window.destroy()
    flush()


def start_inline_edit(win, kind):
    """Start editing the value of a variable/import in the Variables panel."""
    fg = win.current_flow_graph
    if kind == 'import':
        fg.add_new_block('import', (100, 100))
        fg.update()
        block = [b for b in fg.blocks if b.is_import][-1]
        block.params['imports'].set_value('import math')
    win.vars.update_gui(fg.blocks)
    flush()

    def is_target(block):
        if kind == 'import':
            return block.is_import
        return block.is_variable and block.name == 'samp_rate'

    found = []

    def visit(model, path, it):
        block = model[it][0]
        if block is not None and is_target(block):
            found.append(path.copy())
            return True
        return False

    win.vars.treestore.foreach(visit)
    assert found, "no {} row in the Variables panel".format(kind)
    win.vars.treeview.set_cursor(found[0], win.vars.value_column, True)
    flush()
    return win.get_focus()


@pytest.mark.parametrize('kind', ['variable', 'import'])
def test_ctrl_a_in_inline_editor_selects_text(app, kind):
    win = app.main_window
    fg = win.current_flow_graph
    fg.unselect()

    entry = start_inline_edit(win, kind)
    assert isinstance(entry, Gtk.Editable), \
        "inline editor did not take focus, got {!r}".format(entry)
    text = entry.get_text()
    assert text
    # GTK selects all text when cell editing starts: clear that, so the
    # test only passes if Ctrl+A does the selecting.
    entry.set_position(len(text))
    assert selection_of(entry) == (0, 0)

    press_ctrl_a(win)

    # Collect everything first so a failure shows all symptoms at once.
    focus_kept = win.get_focus() == entry
    state = {
        'focus_kept': focus_kept,
        'text_selection': selection_of(entry) if focus_kept else None,
        'flowgraph_selected': len(fg.selected_elements),
    }
    assert state == {
        'focus_kept': True,
        'text_selection': (0, len(text)),
        'flowgraph_selected': 0,
    }

    win.set_focus(None)
    flush()


def test_ctrl_a_in_block_search_entry_selects_text(app):
    """Regression guard: the block search entry keeps working.
    Not a red/green test: the old has_focus() check is False without an
    active window (Xvfb has no window manager), so it fails on old code there.
    """
    from grc.gui import Actions
    win = app.main_window
    fg = win.current_flow_graph
    fg.unselect()

    Actions.FIND_BLOCKS()
    flush()
    entry = win.btwin.search_entry
    entry.set_text('sig')
    entry.set_position(-1)
    flush()
    assert win.get_focus() == entry
    assert selection_of(entry) == (0, 0)

    press_ctrl_a(win)

    assert win.get_focus() == entry
    state = {'text_selection': selection_of(entry), 'flowgraph_selected': len(fg.selected_elements)}
    assert state == {'text_selection': (0, len(entry.get_text())), 'flowgraph_selected': 0}

    entry.set_text('')
    entry.hide()
    win.set_focus(None)
    flush()


def test_ctrl_a_with_canvas_focus_selects_all_blocks(app):
    win = app.main_window
    fg = win.current_flow_graph
    win.set_focus(None)
    flush()
    fg.unselect()
    assert fg.blocks

    press_ctrl_a(win)

    assert set(fg.blocks) <= fg.selected_elements


def test_select_all_action_selects_all_blocks(app):
    from grc.gui import Actions
    win = app.main_window
    fg = win.current_flow_graph
    win.set_focus(None)
    flush()
    fg.unselect()

    Actions.SELECT_ALL()
    flush()

    assert set(fg.blocks) <= fg.selected_elements
