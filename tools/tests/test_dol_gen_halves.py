"""tools/dol_gen.py: `.2byte` pairs render as one word (join_halves)."""
import ast
import unittest
from pathlib import Path

_src = (Path(__file__).resolve().parent.parent / "dol_gen.py").read_text()
_fn = next(n for n in ast.parse(_src).body if isinstance(n, ast.FunctionDef) and n.name == "join_halves")
_ns = {}
exec(compile(ast.Module(body=[_fn], type_ignores=[]), "dol_gen.py", "exec"), _ns)
join_halves = _ns["join_halves"]


class JoinHalves(unittest.TestCase):
    def test_pair(self):
        self.assertEqual(join_halves(["0x000A", "0x0001"]), "0x000A0001")
        self.assertEqual(join_halves(["0xFFFF", "0"]), "0xFFFF0000")

    def test_range(self):
        with self.assertRaises(ValueError):
            join_halves(["0x10000", "0"])


if __name__ == "__main__":
    unittest.main()
