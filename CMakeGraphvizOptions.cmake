# Include custom targets in dep graph (docstrings)
set(GRAPHVIZ_CUSTOM_TARGETS TRUE)
# Don't include executables in dep graph
# We're not interested in GRC deps
set(GRAPHVIZ_EXECUTABLES FALSE)
# We don't need to see TPLS
# also if using Conan, this is incredibly polluted
set(GRAPHVIZ_EXTERNAL_LIBS FALSE)
# Ignore Pygen targets
set(GRAPHVIZ_IGNORE_TARGETS "^pygen.*")
