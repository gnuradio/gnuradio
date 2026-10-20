# Copyright 2026 Nikkesh Parekh
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
"""
Behavioral unit tests for the GRC canvas grid display feature (Issue #5672).

Tests use a mock Cairo context to verify what _draw_grid() actually *does*:
- Cairo state is properly isolated (save/restore balanced)
- Grid lines are committed to the surface (stroke called)
- Line count matches CANVAS_GRID_SIZE spacing
- Grid color is CANVAS_GRID_COLOR
- Line width and canvas dimensions are zoom-corrected
- No exceptions at zoom extremes
- Grid color is visible against the white background
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

# Add the gnuradio source root so GRC modules are importable.
_grc_root = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
if _grc_root not in sys.path:
    sys.path.insert(0, _grc_root)

# Pre-load canvas.colors to resolve a circular import chain:
#   DrawingArea -> canvas -> flowgraph -> MainWindow -> Notebook -> DrawingArea
# Importing colors first lets the canvas package (and DrawingArea as a
# side-effect) finish loading before any test tries to import DrawingArea.
import grc.gui.canvas.colors  # noqa: F401


class TestGridDrawingBehavior(unittest.TestCase):
    """Behavioral tests for _draw_grid using a mock Cairo context.

    These tests verify what _draw_grid actually *does*, not just that it
    exists.  A mock Cairo context allows us to inspect every call made
    without requiring a live GTK display.

    Note: _draw_grid receives *logical* canvas dimensions (already divided
    by zoom_factor); the caller (DrawingArea.draw) is responsible for the
    division.  Tests at zoom != 1.0 must pass the pre-divided values.
    """

    def setUp(self):
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
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800.0, 600.0)
        self.mock_cr.save.assert_called_once_with()

    def test_draw_grid_calls_restore(self):
        """_draw_grid must call cr.restore() to undo any Cairo state changes."""
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800.0, 600.0)
        self.mock_cr.restore.assert_called_once_with()

    def test_draw_grid_save_restore_balanced(self):
        """cr.save() and cr.restore() must be called the same number of times."""
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800.0, 600.0)
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
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800.0, 600.0)
        self.mock_cr.stroke.assert_called_once()

    def test_draw_grid_draws_multiple_lines(self):
        """_draw_grid must draw more than one line in each direction."""
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800.0, 600.0)
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

        _draw_grid receives logical dimensions directly; the test passes
        pre-divided values matching what DrawingArea.draw() would supply.
        """
        import math
        from grc.gui import Constants
        canvas_w, canvas_h = 160.0, 80.0    # exact multiples of 8, logical units
        step = Constants.CANVAS_GRID_SIZE

        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, canvas_w, canvas_h)

        expected_vertical = math.floor(canvas_w / step) + 1
        expected_horizontal = math.floor(canvas_h / step) + 1
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
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 800.0, 600.0)
        self.mock_cr.set_source_rgba.assert_called_once_with(*CANVAS_GRID_COLOR)

    # ------------------------------------------------------------------
    # Zoom factor handling
    # ------------------------------------------------------------------

    def test_draw_grid_works_at_minimum_zoom(self):
        """_draw_grid must not raise at the minimum zoom level (0.1)."""
        self.mock_self.zoom_factor = 0.1
        # Caller pre-divides: logical = widget_px / zoom. Pass representative values.
        try:
            self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 8000.0, 6000.0)
        except Exception as exc:
            self.fail("_draw_grid raised %s at zoom=0.1" % exc)
        self.mock_cr.stroke.assert_called_once()

    def test_draw_grid_works_at_maximum_zoom(self):
        """_draw_grid must not raise at the maximum zoom level (5.0)."""
        self.mock_self.zoom_factor = 5.0
        try:
            self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 160.0, 120.0)
        except Exception as exc:
            self.fail("_draw_grid raised %s at zoom=5.0" % exc)
        self.mock_cr.stroke.assert_called_once()

    def test_draw_grid_line_width_is_zoom_corrected(self):
        """Line width passed to Cairo must be 1/zoom so lines stay 1 px on screen."""
        zoom = 2.0
        self.mock_self.zoom_factor = zoom
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, 400.0, 300.0)
        self.mock_cr.set_line_width.assert_called_once_with(1.0 / zoom)

    def test_draw_grid_canvas_size_is_zoom_corrected(self):
        """Grid lines must stay within the logical canvas dimensions."""
        zoom = 2.0
        canvas_w, canvas_h = 200.0, 150.0   # already logical (400px / 2, 300px / 2)
        self.mock_self.zoom_factor = zoom
        self.DrawingArea._draw_grid(self.mock_self, self.mock_cr, canvas_w, canvas_h)

        for call in self.mock_cr.move_to.call_args_list:
            x, y = call[0]
            self.assertLessEqual(x, canvas_w,
                                 "move_to x exceeds logical canvas width")
            self.assertLessEqual(y, canvas_h,
                                 "move_to y exceeds logical canvas height")


class TestGridColorVisibility(unittest.TestCase):
    """Visibility checks for CANVAS_GRID_COLOR."""

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
