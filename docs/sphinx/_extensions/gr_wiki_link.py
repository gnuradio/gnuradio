from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Any
    from collections.abc import Sequence

    from docutils.nodes import Node, system_message
    from docutils.parsers.rst.states import Inliner

from docutils import nodes

from sphinx.locale import _


def gr_wiki_link_role(
    name: str,
    rawtext: str,
    text: str,
    lineno: int,
    inliner: Inliner,
    options: dict[str, Any] | None = None,
    content: Sequence[str] = (),
) -> tuple[list[Node], list[system_message]]:
    """Link to a GR wiki page.

    Returns 2 part tuple containing list of nodes to insert into the
    document and a list of system messages.  Both are allowed to be
    empty.

    :param name: The role name used in the document.
    :param rawtext: The entire markup snippet, with role.
    :param text: The text marked with the role.
    :param lineno: The line number where rawtext appears in the input.
    :param inliner: The inliner instance that called us.
    :param options: Directive options for customization.
    :param content: The directive content for customization.
    """
    app = inliner.document.settings.env.app
    pnode = nodes.reference(
        rawtext,
        "See Wiki",
        internal=False,
        refuri=f"https://wiki.gnuradio.org/index.php?title={text}",
    )
    pnode["classes"].append("gr-wiki-link")
    return [pnode], []


def setup(app):
    app.add_role("gr-wiki-link", gr_wiki_link_role)
    return
