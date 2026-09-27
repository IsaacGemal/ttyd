"""Tests for the merge checks, starting with the bugs already found in them.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import re
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import label_partials  # noqa: E402
import lint_patch  # noqa: E402


class LintDefinitions(unittest.TestCase):
    def test_finds_plain_and_event_functions(self):
        src = "s32 foo(s32 a) {\n    return a;\n}\nUSER_FUNC(bar) {\n    return 2;\n}\n"
        self.assertEqual(set(lint_patch.definitions(src)), {"foo", "bar"})

    def test_ignores_control_statements(self):
        src = "void f(void) {\n    if (x) {\n        y();\n    }\n}\n"
        self.assertEqual(set(lint_patch.definitions(src)), {"f"})


class LintStubs(unittest.TestCase):
    def test_empty_and_trivial_bodies_are_stubs(self):
        for body in ("", "\n", "return;", " return 0; ", "return NULL;", "return FALSE;"):
            self.assertTrue(lint_patch.is_stub(body), body)

    def test_a_comment_does_not_hide_the_body(self):
        # Bug found in wave 3: `//.*` with DOTALL ate everything after the first line comment.
        self.assertFalse(lint_patch.is_stub(" // TODO: fix mtctr\n    int i;\n    return i;\n"))

    def test_comment_only_body_is_a_stub(self):
        self.assertTrue(lint_patch.is_stub(" // stubbed in retail\n"))


class LintBase(unittest.TestCase):
    def test_a_ref_with_a_slash_is_not_a_file(self):
        # wave 8: `lint_patch.py UNIT origin/main` opened "origin/main" as a file and crashed on every unit
        self.assertIsNone(lint_patch.base_file("origin/main"))
        self.assertIsNone(lint_patch.base_file("main"))

    def test_an_existing_path_is_a_file(self):
        self.assertEqual(lint_patch.base_file("/dev/null"), "/dev/null")


class LabelArguments(unittest.TestCase):
    def test_note_keeps_function_and_text_in_order(self):
        # Bug found in wave 5: notes[pop()] = pop() evaluates the right-hand side first.
        unit, reset, notes = label_partials.parse_args(["a/b", "--note", "f", "the text", "--reset", "g", "h"])
        self.assertEqual(unit, "a/b")
        self.assertEqual(notes, {"f": "the text"})
        self.assertEqual(reset, {"g", "h"})


class LabelDefinitions(unittest.TestCase):
    def find(self, name, src):
        return re.search(label_partials.DEF.format(name=re.escape(name)), src, re.M)

    def test_matches_event_functions(self):
        # Bug found in wave 5: USER_FUNC(name) never matched, so no event function was labelled.
        self.assertIsNotNone(self.find("evt_x", "USER_FUNC(evt_x) {\n}\n"))

    def test_matches_plain_functions_and_not_calls(self):
        self.assertIsNotNone(self.find("foo", "void foo(s32 a) {\n}\n"))
        self.assertIsNone(self.find("foo", "    foo(1);\n"))


class LabelBlock(unittest.TestCase):
    def test_label_separated_by_a_blank_line_is_found(self):
        # Bug found in wave 7: a label one blank line above its function was missed, and a second label added.
        lines = ["// NONMATCHING (94%): digits differ.", "", "s32 f(void) {", "}"]
        top, label, at = label_partials.label_block(lines, 2)
        self.assertEqual((top, label, at), (0, 0, 1))
        self.assertEqual(lines, ["// NONMATCHING (94%): digits differ.", "s32 f(void) {", "}"])

    def test_unrelated_comment_after_a_blank_line_is_left_alone(self):
        lines = ["// helpers", "", "s32 f(void) {", "}"]
        self.assertEqual(label_partials.label_block(lines, 2), (2, None, 2))
        self.assertEqual(len(lines), 4)


class LabelText(unittest.TestCase):
    def test_a_near_match_is_not_labelled_100(self):
        # Bug found in wave 7: 99.996% was written as "NONMATCHING (100.00%)".
        self.assertEqual(label_partials.pct_text(99.996086), "99.99")
        self.assertEqual(label_partials.pct_text(63.224), "63.22")

    def test_definitions_inside_block_comments_are_skipped(self):
        # Bug found in wave 7: a commented-out partyEntry2 got a 0.00% label.
        text = "/*\ns32 f(void) {\n}\n*/\ns32 g(void) {\n}\n"
        self.assertTrue(label_partials.in_block_comment(text, text.index("s32 f")))
        self.assertFalse(label_partials.in_block_comment(text, text.index("s32 g")))


if __name__ == "__main__":
    unittest.main()
