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
import header_patch  # noqa: E402
import claims  # noqa: E402
import name_offsets  # noqa: E402


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


class OffsetStructs(unittest.TestCase):
    HEADER = (
        "typedef struct A {\n    u32 chunkCount; // 0x0\n} A;\n\n"
        "typedef struct B {\n    s16 x; // 0x0\n    struct {\n        s32 inner; // 0x0\n    } sub; // 0x4\n} B;\n"
    )

    def test_body_is_only_the_named_struct(self):
        # found in the cleanup pass: a lazy regex from the first `typedef struct` in the header made MapObject's
        # fields include the structs defined above it, and named offsets after fields MapObject doesn't have
        body = "\n".join(name_offsets.struct_body(self.HEADER, "B"))
        self.assertNotIn("chunkCount", body)
        self.assertIn("s16 x;", body)

    def test_nested_members_are_not_top_level_fields(self):
        body = "\n".join(name_offsets.struct_body(self.HEADER, "B"))
        self.assertNotIn("inner", body)

    def test_missing_struct(self):
        self.assertIsNone(name_offsets.struct_body(self.HEADER, "C"))


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


class HeaderPatch(unittest.TestCase):
    PATCH = """--- a/include/x.h
+++ b/include/x.h
@@ -1,2 +1,4 @@
 #pragma once
+struct HitEntry;
+s32 foo(struct HitEntry* hit, f32 x);
"""

    def test_added_prototypes_pass(self):
        self.assertEqual(header_patch.problems(self.PATCH), [])

    def test_changed_line_fails(self):
        self.assertTrue(header_patch.problems(self.PATCH + "-s32 old(void);\n"))

    def test_struct_definition_fails(self):
        self.assertTrue(header_patch.problems(self.PATCH + "+typedef struct X { s32 a; } X;\n"))


class Claims(unittest.TestCase):
    def setUp(self):
        import tempfile
        self.dir = tempfile.TemporaryDirectory()
        self.con = claims.connect(Path(self.dir.name) / "c.sqlite")

    def tearDown(self):
        self.con.close()
        self.dir.cleanup()

    def test_second_wave_skips_claimed_units(self):
        claims.claim(self.con, "w9", ["event/evt_npc", "motion/mot_hip"], now=0)
        got = claims.claim(self.con, "w10", ["event/evt_npc", "koopa"], now=1)
        self.assertEqual(got, {"event/evt_npc": "w9", "koopa": "w10"})

    def test_release_frees_units(self):
        claims.claim(self.con, "w9", ["koopa"], now=0)
        claims.release(self.con, "w9")
        self.assertEqual(claims.claim(self.con, "w10", ["koopa"], now=1), {"koopa": "w10"})

    def test_expired_claims_are_dropped(self):
        claims.claim(self.con, "w9", ["koopa"], hours=1, now=0)
        self.assertEqual(claims.claim(self.con, "w10", ["koopa"], now=2 * 3600), {"koopa": "w10"})


class HeaderPatchApply(unittest.TestCase):
    def setUp(self):
        import tempfile
        self.dir = tempfile.TemporaryDirectory()
        self.root = Path(self.dir.name)
        (self.root / "include").mkdir()
        (self.root / "include/x.h").write_text("#pragma once\ns32 old(void);\n")

    def tearDown(self):
        self.dir.cleanup()

    def test_rejects_paths_outside_include(self):
        bad = "--- a/src/x.c\n+++ b/src/x.c\n@@ -1 +1,2 @@\n+void f(void);\n"
        self.assertTrue(header_patch.problems(bad))
        up = "+++ b/include/../tools/ci.py\n+void f(void);\n"
        self.assertTrue(header_patch.problems(up))

    def test_rejects_include_of_a_c_file(self):
        self.assertTrue(header_patch.problems(HeaderPatch.PATCH + '+#include "../src/battle/battle.c"\n'))

    def test_apply_appends_once_and_is_idempotent(self):
        patch = "--- a/include/x.h\n+++ b/include/x.h\n@@ -2 +2,3 @@\n+s32 old(void);\n+void f(s32 a);\n"
        self.assertEqual(header_patch.apply(patch, self.root), ["include/x.h"])
        self.assertEqual(header_patch.apply(patch, self.root), [])
        text = (self.root / "include/x.h").read_text()
        self.assertEqual(text.count("void f(s32 a);"), 1)
        self.assertEqual(text.count("s32 old(void);"), 1)

    def test_apply_creates_a_new_header(self):
        patch = "--- /home/x/ttyd/include/n.h\n+++ b/include/n.h\n@@ -0,0 +1,2 @@\n+#pragma once\n+void g(void);\n"
        header_patch.apply(patch, self.root)
        self.assertEqual((self.root / "include/n.h").read_text(), "#pragma once\nvoid g(void);\n")


class ReviewTwoInputs(unittest.TestCase):
    """Inputs review 2 of PR #10 showed getting through."""

    def test_added_line_without_a_header_is_rejected(self):
        self.assertTrue(header_patch.problems("+void lost(void);\n"))

    def test_a_later_non_header_section_does_not_land_in_the_previous_header(self):
        patch = ("--- a/include/a.h\n+++ b/include/a.h\n@@ -1 +1,2 @@\n+void ok(void);\n"
                 "diff --git a/src/x.c b/src/x.c\n+void misplaced(void);\n")
        self.assertTrue(header_patch.problems(patch))
        self.assertEqual(header_patch.additions(patch), {"include/a.h": ["void ok(void);"]})

    def test_mismatched_include_delimiters_are_rejected(self):
        self.assertTrue(header_patch.problems(HeaderPatch.PATCH + '+#include "foo.h>\n'))

    def test_claim_duration_must_be_positive(self):
        import tempfile
        with tempfile.TemporaryDirectory() as d:
            con = claims.connect(Path(d) / "c.sqlite")
            for bad in (-1, 0, float("nan")):
                with self.assertRaises(SystemExit):
                    claims.claim(con, "w1", ["koopa"], hours=bad)
            con.close()

    def test_unit_spellings_are_one_unit(self):
        for spelling in ("event/evt_npc", "src/event/evt_npc.c", "./src/event/evt_npc.c", "src//event/evt_npc.c"):
            self.assertEqual(claims.unit_name(spelling), "event/evt_npc")
        with self.assertRaises(SystemExit):
            claims.unit_name("../outside")

    def test_comma_separated_prototypes_are_caught(self):
        import local_prototypes
        found = [p.name for p in local_prototypes.prototypes("extern int first(void), second(void);\n")]
        self.assertEqual(found, ["first", "second"])



class ReviewThreeInputs(unittest.TestCase):
    """Inputs review 3 of PR #10 showed getting through."""

    def check(self, src, scopes, exceptions=()):
        import local_prototypes
        return local_prototypes.check_sources({"src/a.c": src}, list(exceptions), lambda path: scopes)

    def test_static_is_accepted_only_for_a_function_local_in_the_original(self):
        self.assertEqual(self.check("static void helper(void);\n", {"helper": "local"}), [])
        self.assertTrue(self.check("static s32 elsewhere(s32 a);\n", {}))
        self.assertTrue(self.check("static void shared(void);\nvoid shared(void) {\n}\n", {"shared": "global"}))

    def test_static_for_an_inlined_function_the_file_defines_is_accepted(self):
        self.assertEqual(self.check("static void inl(void);\nstatic void inl(void) {\n}\n", {}), [])

    def test_an_any_file_exception_accepts_any_signature(self):
        exc = [{"path": "*", "name": "psndSFX_pit", "reason": "callers disagree"}]
        self.assertEqual(self.check("void psndSFX_pit(s32 id, s16 pitch);\n", {}, exc), [])
        self.assertTrue(self.check("void psndSFX_other(s32 id);\n", {}, exc))

    def test_more_declaration_spellings_are_found(self):
        import local_prototypes
        for src, name in (("USER_FUNC (x);\n", "x"), ('__declspec(section ".init") void f(void);\n', "f")):
            self.assertEqual([p.name for p in local_prototypes.prototypes(src)], [name])

    def test_header_patch_rejects_what_it_used_to_pass(self):
        self.assertTrue(header_patch.problems("Files a/include/x.h and b/include/x.h differ\n"))
        self.assertIn("static", header_patch.problems(HeaderPatch.PATCH + "+static void f(void);\n")[0])
        self.assertTrue(header_patch.problems(HeaderPatch.PATCH + "+// swallow the next line \\\n"))

    def test_apply_counts_only_lines_actually_added(self):
        import tempfile
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root / "include").mkdir()
            (root / "include/x.h").write_text("#pragma once\nvoid a(void);\n")
            patch = "--- a/include/x.h\n+++ b/include/x.h\n@@ -1 +1,3 @@\n+void a(void);\n+void b(void);\n"
            counts = {}
            self.assertEqual(header_patch.apply(patch, root, counts), ["include/x.h"])
            self.assertEqual(counts, {"include/x.h": 1})

    def test_empty_and_dot_unit_names_are_refused(self):
        for bad in ("", ".", "src", "src/"):
            with self.assertRaises(SystemExit):
                claims.unit_name(bad)



class CreatedHeaders(unittest.TestCase):
    """Headers may gain type definitions (effect work structs), applied as whole blocks."""
    NEW = ("--- a/include/eff/e.h\t1969-12-31\n+++ b/include/eff/e.h\t2026-09-27\n@@ -0,0 +1,5 @@\n"
           "+#pragma once\n+typedef struct effEData {\n+\ts32 a; //0x0\n+} effEData;\n+void effEEntry(void);\n")

    def test_a_created_header_may_define_types(self):
        self.assertEqual(header_patch.problems(self.NEW), [])

    def test_an_existing_header_takes_a_whole_type_but_no_data(self):
        patch = "--- a/include/x.h\n+++ b/include/x.h\n@@ -1 +1,4 @@\n+typedef struct Y {\n+\ts32 a;\n+} Y;\n"
        self.assertEqual(header_patch.problems(patch), [])
        self.assertTrue(header_patch.problems(patch + "+extern f32 lbl_8039CDA8[];\n"))

    def test_a_created_header_still_checks_includes(self):
        self.assertTrue(header_patch.problems(self.NEW + '+#include "../src/x.c"\n'))

    def test_a_created_header_takes_types_but_not_code(self):
        ok = ("+typedef struct K {\n+\t/* 0x00 */ char text[8];\n+} K;\n+typedef enum E {\n+\tE_A = 0x1, // a\n+\tE_B\n+} E;\n+struct S {\n+\tu8 flags : 4;\n"
              "+\tvoid (*cb)(s32 a);\n+\tVec pos[2]; //0x4\n+};\n")
        self.assertEqual(header_patch.problems(self.NEW + ok), [])
        for bad in ("+int injected = 1;\n", "+void body(void) { injected++; }\n", "+#define REWRITE(x) 0\n",
                    "+#error accepted\n", "+s32 g;\n", "+u8 buf[0x100];\n", "+void (*hook)(void);\n",
                    "+struct T {\n+\ts32 a;\n+} gObj;\n", "+bareword\n"):
            self.assertTrue(header_patch.problems(self.NEW + bad), bad)

    def test_static_is_refused_even_in_a_created_header(self):
        self.assertTrue(header_patch.problems(self.NEW + "+static void f(void);\n"))

    def test_types_are_applied_whole_or_skipped_whole(self):
        import tempfile
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            header_patch.apply(self.NEW, root)
            first = (root / "include/eff/e.h").read_text()
            header_patch.apply(self.NEW, root)  # the same header again (another agent created it too)
            self.assertEqual((root / "include/eff/e.h").read_text(), first)
            other = ("--- a/include/eff/e.h\n+++ b/include/eff/e.h\n@@ -1 +1,4 @@\n"
                     "+typedef struct effEData {\n+\ts32 a; //0x0\n+\tf32 b;\n+} effEData;\n"
                     "+typedef struct effF {\n+\ts32 a; //0x0\n+} effF;\n")
            with self.assertRaises(SystemExit):  # effEData with a different layout
                header_patch.apply(other, root)
            self.assertEqual((root / "include/eff/e.h").read_text(), first)
            anon = "--- a/include/eff/e.h\n+++ b/include/eff/e.h\n@@ -1 +1,4 @@\n+enum {\n+\tA,\n+\tB\n+};\n"
            header_patch.apply(anon, root)
            header_patch.apply(anon, root)
            self.assertEqual((root / "include/eff/e.h").read_text().count("enum {"), 1)

    def test_a_tag_or_a_commented_out_definition_is_checked(self):
        import tempfile
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root / "include").mkdir()
            (root / "include/x.h").write_text("typedef struct Existing {\n\ts32 a;\n} Existing;\n/* typedef struct Hidden {\n\ts32 f;\n} Hidden; */\n")
            alias = "--- a/include/x.h\n+++ b/include/x.h\n@@ -1 +1,3 @@\n+typedef struct Existing {\n+\ts32 other;\n+} Alias;\n"
            with self.assertRaises(SystemExit):
                header_patch.apply(alias, root)
            hidden = "--- a/include/x.h\n+++ b/include/x.h\n@@ -1 +1,3 @@\n+typedef struct Hidden {\n+\ts32 f;\n+} Hidden;\n"
            header_patch.apply(hidden, root)
            self.assertEqual(header_patch.code((root / "include/x.h").read_text()).count("struct Hidden {"), 1)

    def test_context_lines_inside_a_new_type_are_refused(self):
        patch = ("--- a/include/x.h\n+++ b/include/x.h\n@@ -1,3 +1,6 @@\n+typedef struct B {\n+\ts32 q;\n"
                 " typedef struct A {\n \ts32 a;\n } A;\n+\ts32 r;\n+} B;\n")
        self.assertTrue(header_patch.problems(patch))

    def test_an_unclosed_type_is_refused(self):
        self.assertTrue(header_patch.problems("--- a/include/x.h\n+++ b/include/x.h\n@@ -1 +1,2 @@\n+typedef struct Y {\n+\ts32 a;\n"))



class LibrarySources(unittest.TestCase):
    def test_a_library_source_path_names_its_main_dol_unit(self):
        self.assertEqual(lint_patch.source_path("libs/metrowerks/src/MSL_C.PPCEABI.bare.H.a/printf.c"),
                         ("libs/metrowerks/src/MSL_C.PPCEABI.bare.H.a/printf.c", "MarioSt/MSL_C.PPCEABI.bare.H.a/printf"))
        self.assertEqual(lint_patch.source_path("rels/pik/src/unit/x.c"), ("rels/pik/src/unit/x.c", "pik/unit/x"))



class Provenance(unittest.TestCase):
    def run_on(self, events, workspace="/home/isaac/ws"):
        import json, os, tempfile, provenance
        from unittest import mock
        with tempfile.NamedTemporaryFile("w", suffix=".jsonl", delete=False) as f:
            f.write("\n".join(json.dumps(e) for e in events))
        with mock.patch.dict(os.environ, {"HOME": "/home/isaac", "TTYD_BASE": ""}):
            return provenance.findings(f.name, workspace)

    def bash(self, *cmds):
        return self.run_on([{"type": "tool_execution_start", "toolName": "bash", "args": {"command": c}} for c in cmds])

    def test_fetches_and_clones_are_listed(self):
        out = self.bash("curl -s https://raw.githubusercontent.com/x/y/main/a.c -o tmp/a.c",
                        "git clone https://github.com/x/z.git tmp/z", "gh repo clone doldecomp/melee tmp/m",
                        "wget github.com/doldecomp/melee/raw/master/src/scanf.c")
        self.assertIn("fetch  https://raw.githubusercontent.com/x/y/main/a.c", out)
        self.assertIn("clone  https://github.com/x/z.git tmp/z", out)
        self.assertIn("clone  doldecomp/melee tmp/m", out)
        self.assertIn("fetch  github.com/doldecomp/melee/raw/master/src/scanf.c", out)

    def test_paths_outside_the_workspace_are_listed_by_area(self):
        out = "\n".join(self.bash("grep -rn foo /home/other/src", "find /tmp -name printf.c", "cat /tmp/sfa/src/a.c",
                                   "find /home/isaac/ws2 -name x", "rg scanf ~/SFA-Decomp", "cp -r ../ws3/src tmp/"))
        for area in ("/home/other/src", "path   /tmp: /tmp", "/tmp/sfa/src/a.c", "/home/isaac/ws2",
                     "/home/isaac/SFA-Decomp", "/home/isaac/ws3"):
            self.assertIn(area, out)

    def test_the_forms_review_3_found(self):
        out = "\n".join(self.bash(
            "git clone --depth 1 git@github.com:doldecomp/melee.git", "R=https://raw.githubusercontent.com; curl $R/o/melee/a.c",
            "cat $HOME/melee/x.c", "diff a.c /tmp/sfa/a.c|head", "cat</tmp/in.c", "cat /tmp/q/{support,trk}.c",
            'cat "/tmp/with space/x.c"', "cp -r /opt/melee/src .", "cat /usr/local/src/melee/printf.c",
            "curl HTTPS://GITHUB.COM/doldecomp/melee", "sed -e '/a/p' -e '/b/,/c/d' x.s"))
        for want in ("clone  --depth 1 git@github.com:doldecomp/melee.git", "fetch  https://raw.githubusercontent.com/o/melee/a.c",
                     "/home/isaac/melee/x.c", "/tmp/sfa/a.c", "/tmp/in.c", "/tmp/q/support.c", "/tmp/q/trk.c",
                     "/tmp/with space/x.c", "/opt/melee/src", "/usr/local/src/melee/printf.c",
                     "fetch  HTTPS://GITHUB.COM/doldecomp/melee"):
            self.assertIn(want, out)
        self.assertNotIn("/a/p", out)
        self.assertNotIn("/b/", out)

    def test_the_workspace_system_paths_and_sed_addresses_are_not(self):
        self.assertEqual(self.bash("grep -rn foo /home/isaac/ws/src", "ls src > /dev/null", "ls /usr/bin/zstd",
                                   "sed -n '/fn a/,/endfn a/p' x.s", "awk '/^\\.fn/{print}' x.s", "ls tmp"), [])

    def test_other_tools_paths_are_checked(self):
        out = self.run_on([{"type": "tool_execution_start", "toolName": "read", "args": {"path": "/home/isaac/melee/a.c"}},
                           {"type": "tool_execution_start", "toolName": "edit", "args": {"path": "src/a.c"}}])
        self.assertEqual(out, ["path   /home/isaac/melee: /home/isaac/melee/a.c"])
        out = self.run_on([{"type": "tool_execution_start", "toolName": "fetch", "args": {"url": "https://github.com/doldecomp/melee"}},
                           {"type": "tool_execution_start", "toolName": "read", "args": {"file_path": "/tmp/m/a.c"}}])
        self.assertEqual(out, ["fetch  https://github.com/doldecomp/melee", "path   /tmp/m: /tmp/m/a.c"])

    def test_an_unreadable_or_empty_transcript_is_an_error(self):
        import provenance
        with self.assertRaises(OSError):
            provenance.findings("/nonexistent/events.jsonl.zst", "/home/isaac/ws")
        with self.assertRaises(OSError):
            self.run_on([{"type": "message_end"}])


class Semcheck(unittest.TestCase):
    """tools/semcheck.py's passes over (instruction, source line) lists: original first, ours second."""
    def test_a_wrong_call_argument_is_named_with_its_line(self):
        import semcheck
        a = [("li r3, 0x1", 5), ("bl GXSetTevColor", 5)]
        b = [("li r3, 0x2", 9), ("bl GXSetTevColor", 9)]
        self.assertEqual(semcheck.check_calls(a, b), ["CALL   GXSetTevColor (line 9): r3: original 0x1, yours 0x2"])

    def test_a_missing_call_is_flagged(self):
        import semcheck
        a = [("bl camGetPtr", 1), ("bl PSMTXConcat", 2)]
        b = [("bl PSMTXConcat", 7)]
        self.assertTrue(any("camGetPtr" in f for f in semcheck.check_calls(a, b)))

    def test_a_wrong_constant_is_flagged_but_a_register_swap_or_reorder_is_not(self):
        import semcheck
        self.assertEqual(semcheck.check_values([("addi r3, r4, 0x100", 1)], [("addi r5, r6, 0xe8", 3)]),
                         ["VALUE  original `addi R, R, 0x100`, yours `addi R, R, 0xe8` (line 3)"])
        self.assertEqual(semcheck.check_values([("addi r3, r4, 0x10", 1)], [("addi r5, r6, 0x10", 3)]), [])
        swapped = semcheck.check_values([("li r3, 0x3c", 1), ("li r4, 0x28", 1)], [("li r3, 0x28", 2), ("li r4, 0x3c", 2)])
        self.assertEqual(swapped, [])

    def test_field_accesses_and_operation_counts(self):
        import semcheck
        a = [("lwz r3, 0x28(r4)", 1), ("bge 0x10", 1)]
        b = [("lwz r5, 0x1c(r6)", 2), ("blt 0x10", 2), ("lwz r0, 0x8(r1)", 2)]
        mem = semcheck.check_mem(a, b)
        self.assertIn("MEM    `lwz ..., 0x28(base)`: original 1, yours 0", mem)
        self.assertFalse(any("0x8(base)" in f for f in mem))  # stack frame traffic is not a field access
        self.assertEqual(semcheck.check_ops(a, b), ["OPS    original has more: bge x1; yours has more: blt x1, lwz x1"])


if __name__ == "__main__":
    unittest.main()
