"""Tests for tools/pool_plan.py's obj_paths(): the target object and ours for dol units, REL units, and a unit name repeated
across RELs (ours then lives under build/G8MJ01/src/rel/<mod>/).

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import glob
import os
import tempfile
import unittest
from pathlib import Path

_src = (Path(__file__).resolve().parent.parent / "pool_plan.py").read_text()
_a = _src.index("def obj_paths")
_b = _src.index("REL_MODS = ")
_code = _src[_a:_b]


def paths(unit, files, rel_mods=("gor", "yuu")):
    old = os.getcwd()
    with tempfile.TemporaryDirectory() as d:
        os.chdir(d)
        try:
            for f in files:
                p = Path(f)
                p.parent.mkdir(parents=True, exist_ok=True)
                p.write_bytes(b"")
            ns = {"glob": glob, "os": os, "unit": unit, "REL_MODS": set(rel_mods), "sys": __import__("sys")}
            exec(_code, ns)
            return ns["obj_paths"]()
        finally:
            os.chdir(old)


class ObjPathsTest(unittest.TestCase):
    def test_dol_unit(self):
        t, o = paths("event/evt_bero", ["build/G8MJ01/obj/event/evt_bero.o"])
        self.assertEqual(t, "build/G8MJ01/obj/event/evt_bero.o")
        self.assertEqual(o, "build/G8MJ01/src/event/evt_bero.o")

    def test_rel_unit_flat(self):
        t, o = paths("hei/hei_10", ["build/G8MJ01/hei/obj/hei_10.o", "build/G8MJ01/src/hei_10.o"], ("hei",))
        self.assertEqual(o, "build/G8MJ01/src/hei_10.o")

    def test_rel_unit_repeated_name_uses_module_dir(self):
        files = ["build/G8MJ01/gor/obj/event/evt_yuuranking.o", "build/G8MJ01/yuu/obj/event/evt_yuuranking.o",
                 "build/G8MJ01/src/rel/gor/event/evt_yuuranking.o", "build/G8MJ01/src/rel/yuu/event/evt_yuuranking.o"]
        t, o = paths("gor/event/evt_yuuranking", files)
        self.assertEqual(t, "build/G8MJ01/gor/obj/event/evt_yuuranking.o")
        self.assertEqual(o, "build/G8MJ01/src/rel/gor/event/evt_yuuranking.o")


if __name__ == "__main__":
    unittest.main()
