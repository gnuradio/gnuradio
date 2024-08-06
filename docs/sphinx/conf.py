# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = "GNU Radio"
copyright = "2024, GNU Radio Project"
author = "GNU Radio Project"
from gnuradio import gr

version = gr.version()[1:]

# -- General configuration ---------------------------------------------------
import os
import sys

sys.path.append(os.path.abspath("./_extensions"))

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
    # "exhale",
    "gr_wiki_link",
    "sphinx_tabs.tabs",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "gr_sphinx_theme"

base_url = "https://gnuradio.org/docs"

html_theme_options = {
    "versions": {
        "stable": f"{base_url}/stable",
        "3.10.10.0": f"{base_url}/3.10.10.0",
        "3.10.10.0-rc1": f"{base_url}/3.10.10.0-rc1",
        "3.10.9.2": f"{base_url}/3.10.9.2",
        "3.9.7.0": f"{base_url}/3.9.7.0",
    }
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ["_static"]

# -- Breathe (C++) configuration
breathe_projects = {"gnuradio": "../doxygen/xml"}

breathe_default_project = "gnuradio"

# exhale_args = {
#     # These arguments are required
#     "containmentFolder": "./api",
#     "rootFileName": "library_root.rst",
#     "doxygenStripFromPath": "..",
#     # Heavily encouraged optional argument (see docs)
#     "rootFileTitle": "Library API",
#     # Suggested optional arguments
#     "createTreeView": True,
#     # TIP: if using the sphinx-bootstrap-theme, you need
#     "treeViewIsBootstrap": True,
#     "exhaleExecutesDoxygen": False,
# }

# Tell sphinx what the primary language being documented is.
primary_domain = "cpp"

# Tell sphinx what the pygments highlight language should be.
highlight_language = "cpp"


def setup(app):
    from breathe.renderer.sphinxrenderer import (CMacroObject,
                                                 DomainDirectiveFactory)

    DomainDirectiveFactory.cpp_classes["property"] = (CMacroObject, "macro")
