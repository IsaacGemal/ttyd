import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import reloc_value_check as rvc  # noqa: E402


class Normalise(unittest.TestCase):
    def test_relocated_fields_are_zeroed_and_the_rest_kept(self):
        lfs = (48 << 26 | 1 << 21 | 2 << 16 | 0x1234).to_bytes(4, "big")  # lfs f1, 0x1234(r2)
        bl = (18 << 26 | 0x123456 << 2 | 1).to_bytes(4, "big")
        out = rvc.normalise(lfs + bl, {2: (4, 0, 0), 4: (rvc.R_PPC_REL24, 0, 0)})
        self.assertEqual(out[:4], (48 << 26 | 1 << 21 | 2 << 16).to_bytes(4, "big"))
        self.assertEqual(out[4:], (18 << 26 | 1).to_bytes(4, "big"))

    def test_sda21_also_drops_the_base_register(self):
        lfs = (48 << 26 | 1 << 21 | 2 << 16 | 0x10).to_bytes(4, "big")
        self.assertEqual(rvc.normalise(lfs, {0: (109, 0, 0)}), (48 << 26 | 1 << 21).to_bytes(4, "big"))

    def test_load_widths_come_from_the_opcode(self):
        self.assertEqual((rvc.WIDTH[34], rvc.WIDTH[40], rvc.WIDTH[48], rvc.WIDTH[50]), (1, 2, 4, 8))


if __name__ == "__main__":
    unittest.main()

class Partial(unittest.TestCase):
    def test_structural_values_are_not_literals(self):
        self.assertTrue(rvc.structural((8, bytes.fromhex("4330000080000000"))))  # int->float conversion double
        self.assertTrue(rvc.structural((4, bytes.fromhex("000000ff"))))  # colour word (denormal)
        self.assertTrue(rvc.structural((4, bytes.fromhex("ffffffff"))))  # mask (NaN)
        self.assertFalse(rvc.structural((4, bytes.fromhex("3fc90fdb"))))  # pi/2
        self.assertFalse(rvc.structural((4, bytes(4))))  # 0.0f is a literal

    def test_only_small_two_sided_exchanges_are_reported(self):
        class Fake:
            def __init__(self, vals):
                self.vals = vals

        def run(t, o):
            old = rvc.loaded_values
            rvc.loaded_values = lambda obj, code, rel: obj.vals
            try:
                return rvc.check_partial(Fake(t), Fake(o), "f", b"", {}, b"", {})
            finally:
                rvc.loaded_values = old
        pi = (4, bytes.fromhex("3fc90fdb"))
        rounded = (4, bytes.fromhex("3fc90ff9"))
        one = (4, bytes.fromhex("3f800000"))
        self.assertEqual(len(run({pi: "a", one: "b"}, {rounded: "c", one: "d"})), 2)
        self.assertEqual(run({pi: "a", one: "b"}, {one: "d"}), [])  # one-sided: base addressing or a computed value
