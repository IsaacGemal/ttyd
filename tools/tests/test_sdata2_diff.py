"""Tests for tools/sdata2_diff.py's classify(): the verdicts and the EXTRA/MISSING/DUP/neg/prod classes.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import struct
import types
import unittest
from pathlib import Path

_src = (Path(__file__).resolve().parent.parent / "sdata2_diff.py").read_text()
_a = _src.index("def key_of")
_b = _src.index("def entries_of")
_c = _src.index("def classify")
_d = _src.index("def report")
_pp = types.SimpleNamespace(show=lambda kind, blob: repr(struct.unpack(">f", blob)[0]) if len(blob) == 4 else repr(blob),
                            kind_of=lambda sec, name, blob: "f32")
_ns = {"struct": struct, "pp": _pp}
exec(_src[_a:_b] + _src[_c:_d], _ns)
classify = _ns["classify"]


def f(v, name="x", users=()):
    blob = struct.pack(">f", v)
    return dict(sec=".sdata2", name=name, blob=blob, key=blob, users=list(users))


class ClassifyTest(unittest.TestCase):
    def test_identical(self):
        self.assertEqual(classify([f(1), f(2)], [f(1), f(2)])["verdict"], "IDENTICAL")

    def test_pad_tail(self):
        t = [f(1), dict(f(0), blob=b"\0\0\0\0", key=b"\0\0\0\0")]
        self.assertEqual(classify(t, [f(1)])["verdict"], "PAD")

    def test_order_only(self):
        r = classify([f(1, users=["a"]), f(2, users=["b"])], [f(2, users=["b"]), f(1, users=["a"])])
        self.assertEqual(r["verdict"], "ORDER")
        self.assertEqual(r["first"], 0)
        self.assertEqual(r["displaced"], 2)

    def test_extra_value_and_users(self):
        r = classify([f(1)], [f(1), f(7, users=["fn"])])
        self.assertEqual(r["verdict"], "CONTENT")
        self.assertEqual(r["classes"]["EXTRA"][0]["users"], ["fn"])

    def test_duplicate_ours_merges_target(self):
        r = classify([f(1)], [f(1), f(1)])
        self.assertIn("DUP_OURS", r["classes"])
        self.assertNotIn("EXTRA", r["classes"])

    def test_missing_and_duplicate_target(self):
        r = classify([f(1), f(1), f(5)], [f(1)])
        self.assertIn("DUP_TARGET", r["classes"])
        self.assertIn("MISSING", r["classes"])

    def test_negation_fold_tag(self):
        r = classify([f(16), f(2)], [f(16), f(-16), f(2)])
        self.assertEqual(r["classes"]["EXTRA"][0]["fold"], "neg")

    def test_product_fold_tag(self):
        r = classify([f(2), f(16), f(32)], [f(2), f(16)])
        self.assertEqual(r["classes"]["MISSING"][0]["fold"], "prod")


if __name__ == "__main__":
    unittest.main()
