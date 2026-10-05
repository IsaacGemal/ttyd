"""tools/ci.py: game-code asm only for the functions tools/asm_exceptions.json lists."""
import json
import os
import re
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import ci  # noqa: E402

LIST = [{"file": "rels/gor/src/event/evt_yuuranking.c", "function": "yuu_kesan_chk_copy"}]


class AsmException(unittest.TestCase):
    def test_listed_function_in_its_file(self):
        self.assertTrue(ci.asm_exception("rels/gor/src/event/evt_yuuranking.c",
                                         "static asm s32 yuu_kesan_chk_copy(s32 type, s32 value) {", LIST))

    def test_other_function_or_file_refused(self):
        self.assertFalse(ci.asm_exception("rels/gor/src/event/evt_yuuranking.c", "static asm s32 other(void) {", LIST))
        self.assertFalse(ci.asm_exception("src/mario.c", "static asm s32 yuu_kesan_chk_copy(void) {", LIST))
        self.assertFalse(ci.asm_exception("rels/gor/src/event/evt_yuuranking.c", "asm { nop }", LIST))

    def test_the_list_is_complete_and_true(self):
        entries = json.load(open(ci.ASM_EXCEPTIONS))
        for e in entries:
            for k in ("file", "function", "class", "reason", "approved_by", "date"):
                self.assertTrue(e.get(k), (e, k))
            self.assertIn(e["class"], ("compiler-artifact", "original-asm"), e)
            text = (ci.ROOT / e["file"]).read_text(errors="replace")
            self.assertRegex(text, r"\basm\b[^(\n]*\b" + re.escape(e["function"]) + r"\s*\(", e)


if __name__ == "__main__":
    unittest.main()
