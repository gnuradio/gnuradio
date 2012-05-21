Sphinx is a popular tool for documenting Python APIs which uses reStructuredText
as a its lightweight markup language. Sphinx extends restructured text with
semantic markup elements for documenting Python APIs but once these are used the
ratio of markup to content becomes too high and readability is compromised
enough that the docstring becomes unsuitable for use with standard Python
introspection mechanisms like help() or IDEs.

Hieroglyph is an a Sphinx extension which automatically converts a highly
readable docstring format suitable for use with help() and IDEs to the
reStructuredText hieroglyphics required by Sphinx.