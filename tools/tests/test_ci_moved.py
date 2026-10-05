import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import ci  # noqa: E402


def report(*units):
    return {"units": [{"name": u, "functions": [{"name": f, "fuzzy_match_percent": sc} for f, sc in fns]}
                      for u, fns in units]}


class Moved(unittest.TestCase):
    def test_moved_at_an_equal_score_is_not_a_regression(self):
        worse = [("A:fn", "fuzzy_match_percent", 100.0, 0.0)]
        moved = ci.moved_functions(worse, report(("A", []), ("B", [("fn", 100.0)])))
        self.assertEqual(moved, [(worse[0], "B", 100.0)])

    def test_moved_at_a_lower_score_still_fails(self):
        worse = [("A:fn", "fuzzy_match_percent", 100.0, 0.0)]
        self.assertEqual(ci.moved_functions(worse, report(("A", []), ("B", [("fn", 99.5)]))), [])

    def test_a_function_that_only_dropped_is_not_moved(self):
        # still in its unit at a lower score: a same-named function elsewhere doesn't excuse it
        worse = [("A:fn", "fuzzy_match_percent", 100.0, 90.0)]
        self.assertEqual(ci.moved_functions(worse, report(("A", [("fn", 90.0)]), ("B", [("fn", 100.0)]))), [])


if __name__ == "__main__":
    unittest.main()
