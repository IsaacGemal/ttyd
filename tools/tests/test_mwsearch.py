"""Token mutation and regression gates; no MWCC installation is needed."""
import sys
import contextlib
import io
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import mwsearch


class MutationTests(unittest.TestCase):
    def candidates(self, source):
        return list(mwsearch.mutations(source, "sample"))

    def test_literals_comments_and_siblings_are_opaque(self):
        source = 'int sibling(void) { return 7; }\nint sample(int a, int b) {\n' \
                 '/* a + b */ const char *text = "a + b; { }";\nint x; x = a + b; return x; }'
        candidates = self.candidates(source)
        self.assertTrue(candidates)
        for _, candidate in candidates:
            self.assertTrue(candidate.startswith('int sibling(void) { return 7; }'))
            self.assertIn('/* a + b */', candidate)
            self.assertIn('"a + b; { }"', candidate)
            mwsearch.pairs(mwsearch.tokenize(candidate))

    def test_function_not_call_or_prototype(self):
        source = 'int sample(int x); int other(void) { return sample(2); } int sample(int x) { return x; }'
        tokens = mwsearch.tokenize(source)
        function = mwsearch.find_function(tokens, "sample")
        self.assertEqual(tokens[function.start].text, "int")
        self.assertEqual(function.params, {"x": "int"})

    def test_swap_flip_and_compound(self):
        source = 'int sample(int a, int b) { int x; x = a + b; x = x * b; x = a < b; return x; }'
        candidates = self.candidates(source)
        self.assertTrue(any('(b) + (a)' in candidate for _, candidate in candidates))
        self.assertTrue(any('(b) > (a)' in candidate for _, candidate in candidates))
        self.assertTrue(any('x *= b' in candidate for _, candidate in candidates))
        self.assertTrue(any('expand +=' == name for name, _ in self.candidates('int sample(int a) { a += 1; return a; }')))

    def test_no_swap_of_calls(self):
        candidates = self.candidates('int sample(int a) { return call(a) + call(2); }')
        self.assertFalse(any(name.startswith('swap') for name, _ in candidates))

    def test_via_temp_and_reciprocal(self):
        candidates = self.candidates('float sample(float a) { float x; x = (float)table[2] * a; x = x / 2.0f; return x; }')
        self.assertTrue(any('via-temp' in name and 'mwsearch_temp = (float)table[2]' in candidate for name, candidate in candidates))
        self.assertTrue(any('float reciprocal' == name and '* 0.5f' in candidate for name, candidate in candidates))

    def test_via_temp_for_direct_call_preserves_destination_type(self):
        source = 'int sample(int a) { u32 width; width = get_width(a); return width; }'
        self.assertTrue(any(name.startswith('via-temp get_width') and 'u32 mwsearch_temp;' in candidate
                            and 'mwsearch_temp = get_width(a);' in candidate for name, candidate in self.candidates(source)))

    def test_inline_single_use_and_parameter_reuse(self):
        candidates = self.candidates('float sample(float angle) { float base = angle + 1.0f; return base; }')
        self.assertTrue(any(name == 'inline base' for name, _ in candidates))
        self.assertTrue(any(name == 'reuse angle for base' and 'angle = angle + 1.0f' in candidate
                            for name, candidate in candidates))
        candidates = self.candidates('float sample(float angle) { float base = angle + 1.0f; return base + angle; }')
        self.assertFalse(any(name == 'reuse angle for base' for name, _ in candidates))

    def test_inline_assignment(self):
        candidates = self.candidates('int sample(int x) { int width; width = x + 2; consume(width); return x; }')
        self.assertTrue(any(name == 'inline assignment to width' and 'consume(((int)(x + 2)))' in candidate
                            for name, candidate in candidates))

    def test_reorder_only_independent_locals(self):
        source = 'int sample(int a, int b) { int x; int y; x = a + 1; y = b + 2; return x + y; }'
        self.assertTrue(any(name == 'reorder independent statements' for name, _ in self.candidates(source)))
        source = 'int sample(int a) { int x; int y; x = a + 1; y = x + 2; return y; }'
        self.assertFalse(any(name == 'reorder independent statements' for name, _ in self.candidates(source)))

    def test_scope_and_live_range_split(self):
        source = 'int sample(int a) { int x; { x = a; use(x); x = 2; use(x); } return a; }'
        candidates = self.candidates(source)
        self.assertTrue(any(name.startswith('scope x') for name, _ in candidates))
        self.assertTrue(any(name == 'split live range of x' for name, _ in candidates))

    def test_integer_types_need_bounded_definitions(self):
        self.assertTrue(any(name.startswith('type x:') for name, _ in self.candidates('int sample(void) { s32 x = 2; return x; }')))
        self.assertFalse(any(name.startswith('type x:') for name, _ in self.candidates('int sample(int a) { s32 x = a; return x; }')))
        self.assertFalse(any(name.startswith('type x:') for name, _ in self.candidates('int sample(void) { s32 x = 2; use(&x); return x; }')))

    def test_whitespace_dedup_does_not_merge_literals(self):
        self.assertEqual(mwsearch.normalized_hash('x = a + b;'), mwsearch.normalized_hash('x=a/* note */+b;'))
        self.assertNotEqual(mwsearch.normalized_hash('use("a b");'), mwsearch.normalized_hash('use("ab");'))

    def test_preprocessor_is_opaque(self):
        source = '#define BODY "{ a + b }"\nint sample(int a, int b) {\n#define VALUE(a) ((a) + 2)\nint x; x = a + b; return x; }'
        for _, candidate in self.candidates(source):
            self.assertIn('#define VALUE(a) ((a) + 2)', candidate)

    def test_mutually_exclusive_braces_and_inactive_branch(self):
        source = 'int other(int a) {\n#ifdef MUST_MATCH\nif(a) {\n#else\nif(!a) {\n#endif\nreturn a; } return 0; }\n' \
                 'int sample(int a, int b) {\n#ifdef MUST_MATCH\nreturn a + b;\n#else\nreturn a * b;\n#endif\n}'
        candidates = self.candidates(source)
        self.assertTrue(any('(b) + (a)' in candidate for _, candidate in candidates))
        for _, candidate in candidates:
            self.assertIn('return a * b;', candidate)
            self.assertEqual(candidate[:candidate.index('int sample')], source[:source.index('int sample')])

    def test_control_expression_and_shadowed_local(self):
        source = 'int sample(int a, int b) { int x = 1; if (a < b) { int x = 2; use(x); } return x; }'
        candidates = self.candidates(source)
        self.assertTrue(any('(b) > (a)' in candidate for _, candidate in candidates))
        self.assertFalse(any(name.startswith(('type x:', 'reuse', 'inline x', 'scope x')) for name, _ in candidates))

    def test_type_changes_reject_promotions_and_sizeof(self):
        for use in ['return x - 1U;', 'return sizeof(x);', 'consume(x); return x;']:
            source = 'int sample(void) { u32 x = 2; ' + use + ' }'
            self.assertFalse(any(name.startswith('type x:') for name, _ in self.candidates(source)))

    def test_four_shop_style_call_inlines(self):
        source = 'int sample(void) { u32 width; width = get_width(); draw((s32)width); ' \
                 'width = get_width(); draw((s32)width); width = get_width(); draw((s32)width); ' \
                 'width = get_width(); { f32 w = width; draw_float(w); } return 0; }'
        candidates = self.candidates(source)
        self.assertEqual(sum(name == 'inline assignment to width' for name, _ in candidates), 4)

    def test_no_hoisting_out_of_short_circuit_or_sizeof(self):
        for expression in ['a && (b / a)', 'a ? (b / a) : 0', 'sizeof(b / a)']:
            source = 'int sample(int a, int b) { int x; x = ' + expression + '; return x; }'
            self.assertFalse(any(name.startswith('via-temp b / a') for name, _ in self.candidates(source)))
        source = 'int sample(int a) { int x; x = side_effect(); return a && x; }'
        self.assertFalse(any(name == 'inline assignment to x' for name, _ in self.candidates(source)))

    def test_temp_reuse_cannot_clobber_self_updated_value(self):
        source = 'int sample(int a, int b) { int x; int y; x = a; y = b * 2; x = x + y; return x; }'
        self.assertFalse(any(name == 'via existing x' for name, _ in self.candidates(source)))
        source = 'int sample(int a) { int x; x = a + 2; use(x); x = x + 1; return x; }'
        self.assertFalse(any(name == 'split live range of x' for name, _ in self.candidates(source)))
        self.assertFalse(any(name == 'inline assignment to x' for name, _ in self.candidates(source)))

    def test_parameter_reuse_with_macro_arguments(self):
        source = 'f32 sample(f32 angle) { f32 base = compAngle(MATCH_F32(zero), angle); ' \
                 'f32 bumped = base + MATCH_F32(forty_five); return bumped; }'
        self.assertTrue(any(name == 'reuse angle for base' for name, _ in self.candidates(source)))


class ScoreTests(unittest.TestCase):
    def test_mask_only_relocation_bits(self):
        self.assertEqual(mwsearch.instruction_score(((0x38630000, 0xffff),), ((0x38631234, 0xffff),)), (0, 1))
        self.assertEqual(mwsearch.instruction_score(((0x38630000, 0xffff),), ((0x38831234, 0xffff),)), (1, 1))
        self.assertEqual(mwsearch.instruction_score(((0x38630000, 0xffff),), ()), (1, 1))

    def test_sibling_and_raw_data_regression_gates(self):
        baseline = mwsearch.ObjectImage({'sample': (), 'sibling': ()}, {'.sdata2': b'1234'})
        before = {'sample': (2, 4), 'sibling': (0, 2)}
        after = {'sample': (0, 4), 'sibling': (0, 2)}
        self.assertIsNone(mwsearch.acceptable(before, baseline, after, baseline, 'sample'))
        self.assertIn('sibling regressed', mwsearch.acceptable(before, baseline,
                      {**after, 'sibling': (1, 2)}, baseline, 'sample'))
        changed = mwsearch.ObjectImage(baseline.functions, {'.sdata2': b'1243'})
        self.assertEqual(mwsearch.acceptable(before, baseline, after, changed, 'sample'), 'data changed')


class SearchTests(unittest.TestCase):
    def command(self):
        return mwsearch.CompileCommand(('timeout', '-k', '10', '300', 'build/tools/wibo',
                                       'build/compilers/GC/2.6/mwcceppc.exe', '-DMUST_MATCH', '-O4,p', '-MMD',
                                       '-c', 'src/sample.c', '-o', 'build/G8MJ01/src'),
                                      'src/sample.c', 'build/compilers/GC/2.6/mwcceppc.exe')

    def test_compile_command_only_redirects_source_and_output(self):
        command = self.command()
        rewritten = command.for_paths('/tmp/sample.c', '/tmp/sample.o')
        self.assertEqual([index for index, (left, right) in enumerate(zip(command.argv, rewritten)) if left != right], [10, 12])
        self.assertEqual(command.defines(), {'MUST_MATCH': '1'})
        self.assertIn('-E', command.preprocess('/tmp/sample.c'))
        self.assertNotIn('-MMD', command.preprocess('/tmp/sample.c'))

    def test_neutral_beam_and_regression_rejections(self):
        target = mwsearch.ObjectImage({'sample': ((0, 0),), 'sibling': ((0, 0),)}, {'.data': b'same'})

        def compile_fixture(command, source, directory, timeout=30):
            if source == 'broken':
                raise RuntimeError('compile failed')
            return source

        def image(source):
            return mwsearch.ObjectImage({'sample': ((0 if source == 'matched' else 1, 0),),
                                         'sibling': ((1 if source == 'sibling_bad' else 0, 0),)},
                                        {'.data': b'changed' if source == 'data_bad' else b'same'})

        def candidates(source, func, defines):
            if source == 'initial':
                return iter([('data regression', 'data_bad'), ('sibling regression', 'sibling_bad'),
                             ('compile error', 'broken'), ('neutral rewrite', 'neutral')])
            return iter([('matching rewrite', 'matched')])

        with tempfile.TemporaryDirectory() as directory, mock.patch.object(mwsearch, 'compile_source', compile_fixture), \
             mock.patch.object(mwsearch, 'read_object', image), mock.patch.object(mwsearch, 'mutations', candidates), \
             contextlib.redirect_stdout(io.StringIO()):
            summary = mwsearch.search(self.command(), target, 'initial', 'sample', 2, 1, Path(directory))
            self.assertEqual(summary['best']['sample'][0], 0)
            self.assertEqual(summary['stats']['improvements'], 1)
            self.assertEqual(summary['stats']['data_rejections'], 1)
            self.assertEqual(summary['stats']['sibling_rejections'], 1)
            self.assertEqual(summary['stats']['compile_failures'], 1)
            patches = list(Path(directory).glob('*.diff'))
            self.assertEqual(len(patches), 1)
            self.assertIn('-initial', patches[0].read_text())
            self.assertIn('+matched', patches[0].read_text())

    def test_bundle_has_context_labels_and_exact_flags(self):
        preprocessed = b'int helper(void) { return 2; }\nint sample(int a) { return helper() + a; }\n'

        def run_fixture(argv, timeout):
            if 'disasm' in argv:
                Path(argv[-1]).write_text('.fn sample, global\n.L_00000000:\nblr\n.endfn sample\n')
                return subprocess.CompletedProcess(argv, 0, b'', b'')
            return subprocess.CompletedProcess(argv, 0, preprocessed, b'')

        with tempfile.TemporaryDirectory() as directory, mock.patch.object(mwsearch, 'run_process', run_fixture), \
             contextlib.redirect_stdout(io.StringIO()):
            output = Path(directory)
            mwsearch.write_bundle(self.command(), Path('/tmp/target.o'), preprocessed.decode(), 'sample', output)
            self.assertEqual({path.name for path in output.iterdir()}, {'target.s', 'src.c', 'ctx.c', 'flags.txt'})
            self.assertIn('.L_00000000:', (output / 'target.s').read_text())
            self.assertIn('int sample(int a);', (output / 'ctx.c').read_text())
            self.assertNotIn('int sample(int a) {', (output / 'ctx.c').read_text())
            self.assertNotIn('int helper(void) {', (output / 'src.c').read_text())
            self.assertIn('compiler: mwcc_247_107', (output / 'flags.txt').read_text())
            self.assertIn('-DMUST_MATCH -O4,p -MMD', (output / 'flags.txt').read_text())


if __name__ == '__main__':
    unittest.main()
