"""Tests for tools/perm_unit.py (dol/REL unit resolution, cflags lookup) and perm_loop.py's finished_from() retry rule.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(TOOLS))
import perm_unit  # noqa: E402

_src = (TOOLS / "perm_loop.py").read_text()
_ns = {"os": __import__("os")}
exec(_src[_src.index("RETRY = "):_src.index("def finished():")], _ns)
finished_from = _ns["finished_from"]

NINJA = """# a
build build/G8MJ01/src/event/evt_yuucounter.o: mwcc_sjis $
    rels/yuu/src/event/evt_yuucounter.c | build/compilers build/tools/wibo $
    build/tools/sjiswrap.exe tools/transform_dep.py || pre-compile
  mw_version = GC/2.6
  cflags = -nodefaults -O4,p -i include $
      -i rels/yuu/include -lang=c
  basedir = build/G8MJ01/src/event
build build/G8MJ01/src/event/other.o: mwcc rels/zzz/src/event/other.c
  mw_version = GC/2.6
  cflags = -O0
  basedir = build/G8MJ01/src/event
"""


def tree(files):
    d = tempfile.TemporaryDirectory()
    for f in files:
        p = Path(d.name) / f
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text("")
    return d


class ResolveTest(unittest.TestCase):
    def test_dol_unit(self):
        with tree(["src/event/evt_bero.c"]) as d:
            r = perm_unit.resolve("event/evt_bero", d)
        self.assertEqual((r["src"], r["name"], r["target"], r["mod"]),
                         ("src/event/evt_bero.c", "event/evt_bero", "build/G8MJ01/obj/event/evt_bero.o", None))

    def test_rel_unit_with_module(self):
        with tree(["rels/yuu/src/event/evt_yuucounter.c"]) as d:
            r = perm_unit.resolve("yuu/event/evt_yuucounter", d)
        self.assertEqual((r["src"], r["name"], r["target"], r["mod"]),
                         ("rels/yuu/src/event/evt_yuucounter.c", "event/evt_yuucounter",
                          "build/G8MJ01/yuu/obj/event/evt_yuucounter.o", "yuu"))

    def test_rel_unit_bare_when_unique(self):
        with tree(["rels/yuu/src/yuu_02.c", "rels/tou/src/tou_03.c"]) as d:
            self.assertEqual(perm_unit.resolve("yuu_02", d)["mod"], "yuu")

    def test_bare_name_in_two_rels_is_an_error(self):
        with tree(["rels/a/src/x.c", "rels/b/src/x.c"]) as d:
            with self.assertRaises(LookupError):
                perm_unit.resolve("x", d)
            self.assertEqual(perm_unit.resolve("b/x", d)["mod"], "b")

    def test_dol_wins_over_a_rel_of_the_same_path(self):
        with tree(["src/x.c", "rels/a/src/x.c"]) as d:
            self.assertIsNone(perm_unit.resolve("x", d)["mod"])

    def test_missing_unit(self):
        with tree(["src/x.c"]) as d:
            with self.assertRaises(LookupError):
                perm_unit.resolve("nope", d)


class SourceHashTest(unittest.TestCase):
    def test_hash_changes_with_the_source(self):
        with tree(["src/x.c"]) as d:
            a = perm_unit.source_hash("x", d)
            Path(d, "src/x.c").write_text("int y;")
            b = perm_unit.source_hash("x", d)
        self.assertNotEqual(a, b)
        self.assertEqual(len(a), 12)

    def test_unknown_unit_is_a_dash(self):
        with tree(["src/x.c"]) as d:
            self.assertEqual(perm_unit.source_hash("nope", d), "-")


class CflagsTest(unittest.TestCase):
    def test_rel_rule_with_continuations(self):
        self.assertEqual(perm_unit.cflags(NINJA, "event/evt_yuucounter"), "-nodefaults -O4,p -i include -i rels/yuu/include -lang=c")

    def test_picks_the_right_rule(self):
        self.assertEqual(perm_unit.cflags(NINJA, "event/other"), "-O0")

    def test_missing_rule(self):
        with self.assertRaises(LookupError):
            perm_unit.cflags(NINJA, "event/none")


def line(unit, func, reason):
    return "\t".join(["t", unit, func, "-", "-", "0", reason]) + "\n"


class LoopTest(unittest.TestCase):
    def test_finished_reasons(self):
        self.assertEqual(finished_from([line("u", "f", "zero")]), {("u", "f")})

    def test_retryable_reasons(self):
        for reason in ("load", "interrupted", "disk"):
            self.assertEqual(finished_from([line("u", "f", reason)]), set())

    def test_prepare_failed_is_retried_then_given_up(self):
        two = [line("u", "f", "prepare-failed")] * 2
        self.assertEqual(finished_from(two), set())
        self.assertEqual(finished_from(two + [line("u", "f", "prepare-failed")]), {("u", "f")})

    def test_changed_source_requeues(self):
        old = line("u", "f", "zero") .rstrip("\n") + "\tAAAA\n"
        self.assertEqual(finished_from([old], lambda u: "AAAA"), {("u", "f")})
        self.assertEqual(finished_from([old], lambda u: "BBBB"), set())

    def test_line_without_a_hash_does_not_count_when_a_hash_is_required(self):
        self.assertEqual(finished_from([line("u", "f", "zero")], lambda u: "AAAA"), set())

    def test_success_after_failures(self):
        self.assertEqual(finished_from([line("u", "f", "prepare-failed"), line("u", "f", "timeout")]), {("u", "f")})


if __name__ == "__main__":
    unittest.main()
