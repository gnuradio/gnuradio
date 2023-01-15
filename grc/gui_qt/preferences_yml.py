# User-exposed settings

import yaml

PREF_YML = \
"""
categories:
  - key: general
    name: General
    items:
      - key: editor
        name: Editor
        tooltip: Choose the editor
        dtype: str
        default: gedit
      - key: hide_disabled_blocks
        name: Hide disabled blocks
        dtype: bool
        default: False
      - key: show_complexity
        name: Show flowgraph complexity
        dtype: bool
        default: False
      - key: show_port_labels
        name: Show port labels
        dtype: bool
        default: True
      - key: show_grid
        name: Show grid
        dtype: bool
        default: False
      - key: snap_to_grid
        name: Snap to grid
        dtype: bool
        default: False
      - key: generated_code_preview
        name: Generated code preview
        dtype: bool
        default: False
  - key: appearance
    name: Appearance
    items:
      - key: theme
        name: Theme
        tooltip: Choose the theme
        dtype: enum
        default: dark
        options:
          - light
          - dark
        option_labels:
          - Light
          - Dark
"""

pref_dict = yaml.safe_load(PREF_YML)
