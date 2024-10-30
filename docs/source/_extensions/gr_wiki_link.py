from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from collections.abc import Sequence
    from typing import Any

    from docutils.nodes import Node, system_message
    from docutils.parsers.rst.states import Inliner

from docutils import nodes


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

    :param rawtext: The entire markup snippet, with role.
    :param text: The text marked with the role.
    """
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
    return {
        "version": "0.1",
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
