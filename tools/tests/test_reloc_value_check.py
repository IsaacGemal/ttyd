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
