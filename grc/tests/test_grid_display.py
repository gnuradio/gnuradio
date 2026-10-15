# Copyright 2024 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
"""
Tests for GRC canvas grid display feature (Issue #5672).

These tests verify:
- TOGGLE_GRID_DISPLAY action exists with correct metadata
- Default state is False (grid hidden)
- State can be toggled on and off
- Preference name is correct for persistence
- CANVAS_GRID_COLOR is a valid RGBA tuple
- Grid drawing uses the existing CANVAS_GRID_SIZE constant
- DrawingArea._draw_grid method exists
"""

import sys
import os
import unittest

# Specify GTK3 before any GRC module imports. On Ubuntu 22.04 the default
# GTK version resolves to GTK4 which lacks GTK3-only symbols (e.g.
# Gtk.TargetEntry) that GRC's Constants.py uses at module level.
import gi
gi.require_version('Gtk', '3.0')
gi.require_version('Gdk', '3.0')


class TestGridDisplayAction(unittest.TestCase):
    """Tests for the TOGGLE_GRID_DISPLAY action."""

    def setUp(self):
        # Import the Actions module (does not require a running GRC instance)
        # We need to add the gnuradio source to sys.path
        grc_root = os.path.normpath(
            os.path.join(os.path.dirname(__file__), '..', '..'))
        if grc_root not in sys.path:
            sys.path.insert(0, grc_root)

        from grc.gui import Actions
        self.Actions = Actions

    def test_toggle_grid_display_action_exists(self):
        """TOGGLE_GRID_DISPLAY action must be registered."""
        self.assertTrue(
            hasattr(self.Actions, 'TOGGLE_GRID_DISPLAY'),
            "Actions.TOGGLE_GRID_DISPLAY must exist"
        )

    def test_toggle_grid_display_has_label(self):
        """Action must have a human-readable label."""
        action = self.Actions.TOGGLE_GRID_DISPLAY
        self.assertIsNotNone(action.label)
        self.assertIn('Grid', action.label,
                      "Label should mention 'Grid'")

    def test_toggle_grid_display_has_tooltip(self):
        """Action must have a tooltip."""
        action = self.Actions.TOGGLE_GRID_DISPLAY
        self.assertIsNotNone(action.tooltip)
        self.assertTrue(len(action.tooltip) > 0)

    def test_toggle_grid_display_preference_name(self):
        """Action must have the correct preference_name for persistence."""
        action = self.Actions.TOGGLE_GRID_DISPLAY
        self.assertEqual(action.preference_name, 'grid_display')

    def test_toggle_grid_display_default_is_false(self):
        """Grid must be hidden by default (default=False)."""
        action = self.Actions.TOGGLE_GRID_DISPLAY
        self.assertFalse(action.default,
                         "Grid should be hidden by default")

    def test_toggle_grid_display_set_active(self):
        """State can be toggled on and off."""
        action = self.Actions.TOGGLE_GRID_DISPLAY
        # Toggle on
        action.set_active(True)
        self.assertTrue(action.get_active())
        # Toggle off
        action.set_active(False)
        self.assertFalse(action.get_active())

    def test_toggle_grid_display_separate_from_snap_to_grid(self):
        """Grid display and snap-to-grid are independent actions."""
        grid_action = self.Actions.TOGGLE_GRID_DISPLAY
        snap_action = self.Actions.TOGGLE_SNAP_TO_GRID
        self.assertIsNot(grid_action, snap_action,
                         "Grid display and snap-to-grid must be separate actions")
        self.assertNotEqual(grid_action.preference_name,
                            snap_action.preference_name)


class TestGridColor(unittest.TestCase):
    """Tests for CANVAS_GRID_COLOR."""

    def setUp(self):
        grc_root = os.path.normpath(
            os.path.join(os.path.dirname(__file__), '..', '..'))
        if grc_root not in sys.path:
            sys.path.insert(0, grc_root)

    def test_canvas_grid_color_exists(self):
        """CANVAS_GRID_COLOR must be defined in colors module."""
        from grc.gui.canvas.colors import CANVAS_GRID_COLOR
        self.assertIsNotNone(CANVAS_GRID_COLOR)

    def test_canvas_grid_color_is_rgba_tuple(self):
        """CANVAS_GRID_COLOR must be a 4-element tuple (R, G, B, A)."""
        from grc.gui.canvas.colors import CANVAS_GRID_COLOR
        self.assertEqual(len(CANVAS_GRID_COLOR), 4,
                         "Color must be a 4-element (R, G, B, A) tuple")

    def test_canvas_grid_color_values_in_range(self):
        """All RGBA components must be in [0.0, 1.0]."""
        from grc.gui.canvas.colors import CANVAS_GRID_COLOR
        for i, component in enumerate(CANVAS_GRID_COLOR):
            self.assertGreaterEqual(component, 0.0,
                                    "Component %d must be >= 0.0" % i)
            self.assertLessEqual(component, 1.0,
                                 "Component %d must be <= 1.0" % i)

    def test_canvas_grid_color_distinct_from_background(self):
        """Grid color must differ from background color so the grid is visible."""
        from grc.gui.canvas.colors import CANVAS_GRID_COLOR, FLOWGRAPH_BACKGROUND_COLOR
        self.assertNotEqual(CANVAS_GRID_COLOR, FLOWGRAPH_BACKGROUND_COLOR,
                            "Grid color must differ from background")


class TestGridConstant(unittest.TestCase):
    """Tests that grid display uses the existing snap-to-grid constant."""

    def setUp(self):
        grc_root = os.path.normpath(
            os.path.join(os.path.dirname(__file__), '..', '..'))
        if grc_root not in sys.path:
            sys.path.insert(0, grc_root)

    def test_canvas_grid_size_exists(self):
        """CANVAS_GRID_SIZE must be defined in Constants."""
        from grc.gui import Constants
        self.assertTrue(hasattr(Constants, 'CANVAS_GRID_SIZE'))

    def test_canvas_grid_size_is_positive_integer(self):
        """CANVAS_GRID_SIZE must be a positive integer."""
        from grc.gui import Constants
        self.assertIsInstance(Constants.CANVAS_GRID_SIZE, int)
        self.assertGreater(Constants.CANVAS_GRID_SIZE, 0)


class TestDrawingAreaGridMethod(unittest.TestCase):
    """Tests that DrawingArea._draw_grid method exists."""

    def setUp(self):
        grc_root = os.path.normpath(
            os.path.join(os.path.dirname(__file__), '..', '..'))
        if grc_root not in sys.path:
            sys.path.insert(0, grc_root)

    def test_draw_grid_method_exists(self):
        """DrawingArea must have a _draw_grid method."""
        from grc.gui.DrawingArea import DrawingArea
        self.assertTrue(
            hasattr(DrawingArea, '_draw_grid'),
            "DrawingArea must have a _draw_grid method"
        )
        self.assertTrue(
            callable(getattr(DrawingArea, '_draw_grid')),
            "_draw_grid must be callable"
        )

    def test_draw_grid_method_signature(self):
        """_draw_grid must accept (self, cr, width, height) parameters."""
        import inspect
        from grc.gui.DrawingArea import DrawingArea
        sig = inspect.signature(DrawingArea._draw_grid)
        params = list(sig.parameters.keys())
        self.assertEqual(params, ['self', 'cr', 'width', 'height'],
                         "_draw_grid must accept (self, cr, width, height)")


if __name__ == '__main__':
    unittest.main()
