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
gi.require_version('PangoCairo', '1.0')


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
    """Tests that DrawingArea._draw_grid method exists with correct signature."""

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


class TestGridDrawingBehavior(unittest.TestCase):
    """Behavioral tests for _draw_grid using a mock Cairo context.

    These tests verify what _draw_grid actually *does*, not just that it
    exists.  A mock Cairo context allows us to inspect every call made
    without requiring a live GTK display.
    """

    def setUp(self):
        grc_root = os.path.normpath(
            os.path.join(os.path.dirname(__file__), '..', '..'))
        if grc_root not in sys.path:
            sys.path.insert(0, grc_root)

        from unittest.mock import MagicMock
        from grc.gui.DrawingArea import DrawingArea

        # Build a minimal stand-in for 'self' — only zoom_factor is needed
        # by _draw_grid; everything else can be a mock.
        self.mock_self = MagicMock()
        self.mock_self.zoom_factor = 1.0
        self.mock_cr = MagicMock()
        self.DrawingArea = DrawingArea

    # ------------------------------------------------------------------
    # Cairo state isolation
    # ------------------------------------------------------------------

    def test_draw_grid_calls_save_before_restore(self):
        """_draw_grid must call cr.save() exactly once to isolate Cairo state.

        This ensures that changes to line-width and colour do not bleed
        into subsequent drawing operations (e.g. block/connection rendering).
        """
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        self.mock_cr.save.assert_called_once_with()

    def test_draw_grid_calls_restore(self):
        """_draw_grid must call cr.restore() to undo any Cairo state changes."""
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        self.mock_cr.restore.assert_called_once_with()

    def test_draw_grid_save_restore_balanced(self):
        """cr.save() and cr.restore() must be called the same number of times."""
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        self.assertEqual(
            self.mock_cr.save.call_count,
            self.mock_cr.restore.call_count,
            "Every cr.save() must have a matching cr.restore()"
        )

    # ------------------------------------------------------------------
    # Grid lines are actually drawn
    # ------------------------------------------------------------------

    def test_draw_grid_calls_stroke(self):
        """_draw_grid must call cr.stroke() to commit lines to the surface."""
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        self.mock_cr.stroke.assert_called_once()

    def test_draw_grid_draws_multiple_lines(self):
        """_draw_grid must draw more than one line in each direction."""
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        # Each line requires one move_to + one line_to call.
        # With 800x600 canvas at step=8 we expect ~100+75 = 175 line pairs.
        self.assertGreater(
            self.mock_cr.move_to.call_count, 10,
            "Expected many grid lines; got too few move_to calls"
        )
        self.assertEqual(
            self.mock_cr.move_to.call_count,
            self.mock_cr.line_to.call_count,
            "Every move_to must have exactly one matching line_to"
        )

    def test_draw_grid_line_count_matches_canvas_grid_size(self):
        """Number of grid lines must be consistent with CANVAS_GRID_SIZE.

        For a canvas of width W and height H (at zoom 1.0), the number of
        vertical lines is floor(W/step)+1 and horizontal floor(H/step)+1.
        """
        import math
        from grc.gui import Constants
        width, height = 160, 80          # exact multiples of 8
        step = Constants.CANVAS_GRID_SIZE

        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, width, height)

        expected_vertical = math.floor(width / step) + 1
        expected_horizontal = math.floor(height / step) + 1
        expected_total = expected_vertical + expected_horizontal

        self.assertEqual(
            self.mock_cr.move_to.call_count,
            expected_total,
            "Grid line count does not match CANVAS_GRID_SIZE spacing"
        )

    # ------------------------------------------------------------------
    # Colour
    # ------------------------------------------------------------------

    def test_draw_grid_sets_grid_color(self):
        """_draw_grid must paint with CANVAS_GRID_COLOR, not an arbitrary colour."""
        from grc.gui.canvas.colors import CANVAS_GRID_COLOR
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        self.mock_cr.set_source_rgba.assert_called_once_with(*CANVAS_GRID_COLOR)

    # ------------------------------------------------------------------
    # Zoom factor handling
    # ------------------------------------------------------------------

    def test_draw_grid_works_at_minimum_zoom(self):
        """_draw_grid must not raise at the minimum zoom level (0.1)."""
        self.mock_self.zoom_factor = 0.1
        try:
            self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        except Exception as exc:
            self.fail("_draw_grid raised %s at zoom=0.1" % exc)
        self.mock_cr.stroke.assert_called_once()

    def test_draw_grid_works_at_maximum_zoom(self):
        """_draw_grid must not raise at the maximum zoom level (5.0)."""
        self.mock_self.zoom_factor = 5.0
        try:
            self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        except Exception as exc:
            self.fail("_draw_grid raised %s at zoom=5.0" % exc)
        self.mock_cr.stroke.assert_called_once()

    def test_draw_grid_line_width_is_zoom_corrected(self):
        """Line width passed to Cairo must be 1/zoom so lines stay 1 px on screen."""
        zoom = 2.0
        self.mock_self.zoom_factor = zoom
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800, 600)
        self.mock_cr.set_line_width.assert_called_once_with(1.0 / zoom)

    def test_draw_grid_canvas_size_is_zoom_corrected(self):
        """Grid must cover the full logical canvas, not just widget pixels."""
        zoom = 2.0
        width, height = 400, 300
        self.mock_self.zoom_factor = zoom
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, width, height)

        logical_w = width / zoom   # 200
        logical_h = height / zoom  # 150

        for call in self.mock_cr.move_to.call_args_list:
            x, y = call[0]
            self.assertLessEqual(x, logical_w,
                                 "move_to x exceeds logical canvas width")
            self.assertLessEqual(y, logical_h,
                                 "move_to y exceeds logical canvas height")


class TestGridColorVisibility(unittest.TestCase):
    """Additional visibility checks for CANVAS_GRID_COLOR."""

    def setUp(self):
        grc_root = os.path.normpath(
            os.path.join(os.path.dirname(__file__), '..', '..'))
        if grc_root not in sys.path:
            sys.path.insert(0, grc_root)

    def test_canvas_grid_color_alpha_is_visible(self):
        """Grid color must not be fully transparent (alpha > 0)."""
        from grc.gui.canvas.colors import CANVAS_GRID_COLOR
        alpha = CANVAS_GRID_COLOR[3]
        self.assertGreater(alpha, 0.0,
                           "Grid color alpha must be > 0 so the grid is visible")

    def test_canvas_grid_color_rgb_darker_than_background(self):
        """Grid RGB luminance must be lower than the white background."""
        from grc.gui.canvas.colors import CANVAS_GRID_COLOR, FLOWGRAPH_BACKGROUND_COLOR
        bg_luminance = sum(FLOWGRAPH_BACKGROUND_COLOR[:3]) / 3
        grid_luminance = sum(CANVAS_GRID_COLOR[:3]) / 3
        self.assertLess(
            grid_luminance, bg_luminance,
            "Grid color must be darker than the background to be visible"
        )


if __name__ == '__main__':
    unittest.main()
