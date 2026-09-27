"""Regression tests for the local-prototype gate, including real TTYD syntax."""
import sys
from pathlib import Path
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from local_prototypes import check_sources, prototypes


class PrototypeParsing(unittest.TestCase):
    def names(self, text):
        return [p.name for p in prototypes(text)]

    def test_plain_multiline_pointer_and_callback_parameters(self):
        text = ('extern const char* lookup(const char* name);\n'
                's32\nrun(EventEntry* event,\n BOOL first);\n'
                'void sort(void* data, int (*compare)(void*, void*));\n')
        self.assertEqual(self.names(text), ['lookup', 'run', 'sort'])

    def test_same_line_and_block_scope(self):
        text = 'int first(void); int second(void);\nvoid caller(void) { extern int local(void); }'
        self.assertEqual(self.names(text), ['first', 'second', 'local'])

    def test_event_macro_prototypes_but_not_definitions(self):
        text = ('USER_FUNC(event);\nextern USER_FUNC(other);\n'
                'static USER_FUNC(private_event);\nUSER_FUNC(definition) { return 2; }')
        found = list(prototypes(text))
        self.assertEqual([p.name for p in found], ['event', 'other', 'private_event'])
        self.assertEqual([p.static for p in found], [False, False, True])

    def test_ignores_comments_strings_and_macro_definitions(self):
        text = ('/* int fake(void); */\n// void fake2(void);\n'
                'const char* text = "void fake3(void);";\n'
                '#define DECL int fake4(void);\n'
                '#define MULTI \\\n void fake5(void);\n'
                'void real(void);')
        self.assertEqual(self.names(text), ['real'])

    def test_ignores_calls_definitions_function_pointers_and_typedefs(self):
        text = ('void (*callback)(void);\ntypedef int (*Callback)(void);\n'
                'extern int data;\nvoid f(void) { if (x) { call(); } return other(); }')
        self.assertEqual(self.names(text), [])

    def test_event_script_opcodes_are_not_a_return_type(self):
        self.assertEqual(self.names('EVT_BEGIN(script)\n RETURN\n END\nEVT_END();'), [])

    def test_line_numbers_survive_block_comments(self):
        found = list(prototypes('/* comment\ncontinues */\n\nvoid f(void);'))
        self.assertEqual(found[0].line, 4)

    def test_static_forward_declarations_are_allowed(self):
        self.assertEqual(check_sources({'src/a.c': 'static inline int f(void);'}, []), [])


class PrototypeExceptions(unittest.TestCase):
    def entry(self, **kw):
        return dict(path='src/a.c', name='f', declaration='int f(void);', reason='Original ABI.', **kw)

    def test_new_prototype_fails_without_allowlist_entry(self):
        problems = check_sources({'src/a.c': 'int f(void);'}, [])
        self.assertEqual(len(problems), 1)
        self.assertIn('src/a.c:1: f:', problems[0])

    def test_comment_alone_cannot_bypass_gate(self):
        self.assertTrue(check_sources({'src/a.c': '// LOCAL_PROTO(f): arbitrary\nint f(void);'}, []))

    def test_exact_exception_allows_whitespace_and_comments(self):
        self.assertEqual(check_sources({'src/a.c': 'int f( /* args */ void );'}, [self.entry()]), [])

    def test_changed_signature_and_different_file_are_rejected(self):
        self.assertTrue(check_sources({'src/a.c': 'float f(void);'}, [self.entry()]))
        self.assertTrue(check_sources({'src/b.c': 'int f(void);'}, [self.entry()]))

    def test_stale_duplicate_and_unexplained_exceptions_fail(self):
        self.assertTrue(check_sources({}, [self.entry()]))
        self.assertTrue(check_sources({'src/a.c': 'int f(void);\nint f(void);'}, [self.entry()]))
        self.assertTrue(check_sources({'src/a.c': 'int f(void);'}, [self.entry(), self.entry()]))
        entry = self.entry()
        entry['reason'] = ''
        self.assertTrue(check_sources({'src/a.c': 'int f(void);'}, [entry]))


if __name__ == '__main__':
    unittest.main()
