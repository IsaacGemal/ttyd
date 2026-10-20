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


def changes(unit, lost, gained):
    """objdiff `report changes` shape: (name, address, score) of a function only in the old / the new report."""
    fns = [{"name": n, "from": {"fuzzy_match_percent": sc}, "metadata": {"virtual_address": a}} for n, a, sc in lost]
    fns += [{"name": n, "to": {"fuzzy_match_percent": sc}, "metadata": {"virtual_address": a}} for n, a, sc in gained]
    return {"units": [{"name": unit, "functions": fns}]}


class Renamed(unittest.TestCase):
    def test_renamed_in_place_is_not_a_regression(self):
        worse = [("A:unk_80001000", "fuzzy_match_percent", 100.0, 0.0)]
        got = ci.renamed_functions(worse, changes("A", [("unk_80001000", "1", 100.0)], [("goodName", "1", 100.0)]))
        self.assertEqual(got, [(worse[0], "goodName", 100.0)])

    def test_a_new_function_at_another_address_is_not_a_rename(self):
        worse = [("A:unk_80001000", "fuzzy_match_percent", 100.0, 0.0)]
        self.assertEqual(ci.renamed_functions(worse, changes("A", [("unk_80001000", "1", 100.0)],
                                                             [("goodName", "2", 100.0)])), [])

    def test_renamed_at_a_lower_score_still_fails(self):
        worse = [("A:unk_80001000", "fuzzy_match_percent", 100.0, 0.0)]
        self.assertEqual(ci.renamed_functions(worse, changes("A", [("unk_80001000", "1", 100.0)],
                                                             [("goodName", "1", 90.0)])), [])

    def test_vanished_without_a_successor_still_fails(self):
        worse = [("A:unk_80001000", "fuzzy_match_percent", 100.0, 0.0)]
        self.assertEqual(ci.renamed_functions(worse, changes("A", [("unk_80001000", "1", 100.0)], [])), [])

    def test_same_address_in_another_unit_is_not_a_rename(self):
        worse = [("A:unk_80001000", "fuzzy_match_percent", 100.0, 0.0)]
        data = changes("A", [("unk_80001000", "1", 100.0)], [])
        data["units"] += changes("B", [], [("goodName", "1", 100.0)])["units"]
        self.assertEqual(ci.renamed_functions(worse, data), [])

    def test_a_score_drop_that_keeps_the_name_still_fails(self):
        worse = [("A:fn", "fuzzy_match_percent", 100.0, 90.0)]
        data = {"units": [{"name": "A", "functions": [{"name": "fn", "from": {"fuzzy_match_percent": 100.0},
                                                       "to": {"fuzzy_match_percent": 90.0},
                                                       "metadata": {"virtual_address": "1"}}]}]}
        self.assertEqual(ci.renamed_functions(worse, data), [])

    def test_no_address_is_not_a_rename(self):
        worse = [("A:unk_80001000", "fuzzy_match_percent", 100.0, 0.0)]
        fns = [{"name": "unk_80001000", "from": {"fuzzy_match_percent": 100.0}},
               {"name": "goodName", "to": {"fuzzy_match_percent": 100.0}}]
        self.assertEqual(ci.renamed_functions(worse, {"units": [{"name": "A", "functions": fns}]}), [])

    def test_one_successor_excuses_one_lost_function(self):
        worse = [("A:x", "fuzzy_match_percent", 100.0, 0.0), ("A:y", "fuzzy_match_percent", 100.0, 0.0)]
        got = ci.renamed_functions(worse, changes("A", [("x", "1", 100.0), ("y", "1", 100.0)],
                                                  [("goodName", "1", 100.0)]))
        self.assertEqual(len(got), 1)

    def test_unit_and_section_measures_are_never_excused(self):
        worse = [("A", "fuzzy_match_percent", 100.0, 0.0), ("A[.data]", "fuzzy_match_percent", 100.0, 0.0)]
        self.assertEqual(ci.renamed_functions(worse, changes("A", [("unk_80001000", "1", 100.0)],
                                                             [("goodName", "1", 100.0)])), [])


if __name__ == "__main__":
    unittest.main()
