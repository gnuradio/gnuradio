import unittest
from hieroglyph.hieroglyph import first_paragraph_indent, gather_lines, unindent

__author__ = 'Robert Smallshire'

class UnindentTests(unittest.TestCase):

    def test_zero_lines(self):
        source   = []
        expected = []
        actual = unindent(source)
        self.assertEqual(actual, expected)

    def test_one_zero_indent_line(self):
        source   = ["First line"]
        expected = [(0, "First line")]
        actual = unindent(source)
        self.assertEqual(actual, expected)

    def test_two_zero_indent_lines(self):
        source   = ["First line",
                    "Second line"]
        expected = [(0, "First line"),
                    (0, "Second line")]
        actual = unindent(source)
        self.assertEqual(actual, expected)

    def test_two_indented_lines(self):
        source   = ["    First line",
                    "      Second line"]
        expected = [(4, "First line"),
                    (6, "Second line")]
        actual = unindent(source)
        self.assertEqual(actual, expected)

    def test_whitespace_line(self):
        source   = ["    "]
        expected = [(4, "")]
        actual = unindent(source)
        self.assertEqual(actual, expected)

    def test_tab_line(self):
        source   = ["\tHello"]
        expected = [(1, "Hello")]
        actual = unindent(source)
        self.assertEqual(actual, expected)


class FirstParagraphIndentTests(unittest.TestCase):

    def test_zero_lines(self):
        source   = []
        expected = []
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_single_line_non_indented_comment(self):
        source   = [(0, "A single line comment")]
        expected = [(0, "A single line comment")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_single_line_indented_comment(self):
        source   = [(4, "A single line comment")]
        expected = [(4, "A single line comment")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_double_line_non_indented_comment(self):
        source   = [(0, "The first line"),
                    (0, "The second line")]
        expected = [(0, "The first line"),
                    (0, "The second line")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_double_line_indented_comment(self):
        source   = [(4, "The first line"),
                    (4, "The second line")]
        expected = [(4, "The first line"),
                    (4, "The second line")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_first_line_indent(self):
        source   = [(4, "The first line"),
                    (0, "The second line")]
        expected = [(4, "The first line"),
                    (0, "The second line")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_first_line_non_indent(self):
        source   = [(0, "The first line"),
                    (4, "The second line")]
        expected = [(4, "The first line"),
                    (4, "The second line")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_increasing_indent(self):
        source   = [(0, "The first line"),
                    (4, "The second line"),
                    (8, "The third line")]
        expected = [(4, "The first line"),
                    (4, "The second line"),
                    (8, "The third line")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_separate_paragraphs(self):
        source   = [(0, "This is the first paragraph"),
                    (0, ""),
                    (4, "This is the second paragraph")]
        expected = [(0, "This is the first paragraph"),
                    (0, ""),
                    (4, "This is the second paragraph")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_separate_paragraphs_indented(self):
        source   = [(4, "This is the first paragraph"),
                    (4, ""),
                    (8, "This is the second paragraph")]
        expected = [(4, "This is the first paragraph"),
                    (4, ""),
                    (8, "This is the second paragraph")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_separated_lines_first_line_non_indented(self):
        source   = [(0, "The first line"),
                    (0, ""),
                    (4, "The third line")]
        expected = [(0, "The first line"),
                    (0, ""),
                    (4, "The third line")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

    def test_separated_lines_first_line_indented(self):
        source   = [(4, "The first line"),
                    (4, ""),
                    (4, "The third line")]
        expected = [(4, "The first line"),
                    (4, ""),
                    (4, "The third line")]
        actual = first_paragraph_indent(source)
        self.assertEqual(actual, expected)

class GatherLinesTests(unittest.TestCase):

    def test_empty(self):
        source   = []
        expected = []
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_one_liner(self):
        source   = [(0, 'One liner')]
        expected = [(0, ['One liner'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_two_liner(self):
        source   = [(0, 'First line'),
                    (0, 'Second line')]
        expected = [(0, ['First line',
                           'Second line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_separated_lines(self):
        source   = [(0, 'First line'),
                    (0, ''),
                    (0, 'Third line')]
        expected = [(0, ['First line',
                         '']),
                    (0, ['Third line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_separated_multi_lines(self):
        source   = [(0, 'First line'),
                    (0, 'Second line'),
                    (0, ''),
                    (0, 'Fourth line'),
                    (0, 'Fifth line')]
        expected = [(0, ['First line',
                         'Second line',
                         '']),
                    (0, ['Fourth line',
                         'Fifth line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)


    def test_indented_lines(self):
        source   = [(0, 'First line'),
                    (4, 'Second line')]
        expected = [(0, ['First line']),
                    (4, ['Second line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_dedented_lines(self):
        source   = [(4, 'First line'),
                    (0, 'Second line')]
        expected = [(4, ['First line']),
                    (0, ['Second line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_indented_multi_lines(self):
        source   = [(0, 'First line'),
                    (0, 'Second line'),
                    (4, 'Third line'),
                    (4, 'Fourth line')]
        expected = [(0, ['First line',
                         'Second line']),
                    (4, ['Third line',
                         'Fourth line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_dedented_multi_lines(self):
        source   = [(4, 'First line'),
                    (4, 'Second line'),
                    (0, 'Third line'),
                    (0, 'Fourth line')]
        expected = [(4, ['First line',
                         'Second line']),
                    (0, ['Third line',
                         'Fourth line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_indented_separated_multi_lines(self):
        source   = [(0, 'First line'),
                    (0, 'Second line'),
                    (0, ''),
                    (4, 'Fourth line'),
                    (4, 'Fifth line')]
        expected = [(0, ['First line',
                         'Second line',
                         '']),
                    (4, ['Fourth line',
                         'Fifth line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)

    def test_dedented_separated_multi_lines(self):
        source   = [(4, 'First line'),
                    (4, 'Second line'),
                    (4, ''),
                    (0, 'Fourth line'),
                    (0, 'Fifth line')]
        expected = [(4, ['First line',
                         'Second line',
                         '']),
                    (0, ['Fourth line',
                         'Fifth line'])]
        actual = gather_lines(source)
        self.assertEqual(actual, expected)
