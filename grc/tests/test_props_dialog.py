from types import SimpleNamespace
from unittest.mock import Mock, patch

import gi
import pytest


@pytest.mark.parametrize("state", ["disabled", "enabled"])
def test_docs_page_renders_make_only_when_enabled(state):
    gi.require_version("Gtk", "3.0")
    gi.require_version("Gdk", "3.0")
    gi.require_version("PangoCairo", "1.0")
    from gi.repository import Gtk

    # ParamWidgets installs screen-dependent styling during import.
    with patch.object(Gtk.StyleContext, "add_provider_for_screen"):
        from grc.gui.PropsDialog import PropsDialog

    templates = Mock()
    templates.render.return_value = "example.target()"
    block = SimpleNamespace(
        state=state,
        category=[],
        is_connection=False,
        doc_url="",
        documentation={"target": "Target docs"},
        templates=templates,
    )
    dialog = SimpleNamespace(
        _block=block,
        _docs_text_display=Mock(),
        _docs_link=Mock(),
    )

    PropsDialog._update_docs_page(dialog)

    if state == "disabled":
        templates.render.assert_not_called()
    else:
        templates.render.assert_called_once_with("make")
