"""tools/agent_audit.py: what it flags in an agent's event log."""
import io
import json
import os
import subprocess
import sys
import tempfile
import unittest
from unittest import mock
from pathlib import Path

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import agent_audit  # noqa: E402

WS = "/home/isaac/ttyd-agents/w70a_battle_battle_camera"


def ev(tool, **args):
    return json.dumps({"type": "tool_execution_start", "toolName": tool, "args": args})


def kinds(*events):
    return sorted({k for k, _, _ in agent_audit.audit(list(events), WS)})


class AgentAudit(unittest.TestCase):
    def test_the_escape_on_record(self):
        # the commands the battle_camera agent ran on 2026-10-03
        self.assertEqual(kinds(ev("bash", command=f"cd {WS} && mkdir -p /home/isaac/par && ls /home/isaac/par")), ["WRITE-OUTSIDE"])
        self.assertEqual(kinds(ev("bash", command="cd /home/isaac/par && for i in 0 1; do cp -a . /home/isaac/par/w$i; done")),
                         ["WRITE-OUTSIDE"])
        self.assertIn("BACKGROUND", kinds(ev("bash", command="for i in 0 1; do nohup python3 worker.py w$i >/dev/null 2>&1 & done")))
        self.assertEqual(kinds(ev("bash", command="pkill -f 'worker.py' 2>/dev/null")), ["SIGNAL"])
        self.assertEqual(kinds(ev("write", path="/home/isaac/par/worker.py", content="x")), ["WRITE-OUTSIDE"])

    def test_ordinary_work_is_clean(self):
        self.assertEqual(kinds(
            ev("bash", command=f"cd {WS} && python3 check.py battle/battle_camera 2>&1 | tail -40 && echo done"),
            ev("bash", command=f"cd {WS} && cp src/battle/battle_camera.c tmp/backup.c && python3 - <<'EOF'\n"
                               "import os; print(os.listdir('.'))\nprint('/home/isaac/elsewhere')\nEOF"),
            ev("read", path=f"{WS}/src/battle/battle_camera.c"),
            ev("write", path=f"{WS}/REPORT.md", content="Model: x"),
            ev("bash", command=f"cd {WS} && make 2>&1 >/dev/null; ls /tmp/x &>/dev/null"),
        ), [])

    def test_reads_outside_are_reported_not_flagged(self):
        flags = agent_audit.audit([ev("bash", command="cat /home/isaac/permuter/settings.toml")], WS)
        self.assertEqual([k for k, _, _ in flags], ["READ-OUTSIDE"])

    def test_background_inside_the_workspace_is_still_flagged(self):
        self.assertEqual(kinds(ev("bash", command=f"cd {WS} && setsid python3 tmp/search.py > tmp/log 2>&1")), ["BACKGROUND"])

    def test_quoted_newline_home_and_relative_paths(self):
        for cmd in ['cp x "/home/isaac/par/y"', f"cd {WS}\nmkdir /home/isaac/par", "mkdir $HOME/par", "mkdir ${HOME}/par",
                    "touch ~/x", "cd .. && touch x", "cp -a . ../w1", f"cd /home/isaac/par && cp -a {WS}/src w0"]:
            self.assertEqual(kinds(ev("bash", command=cmd)), ["WRITE-OUTSIDE"], cmd)

    def test_wrapped_commands(self):
        for cmd in ["timeout -k 10 60 cp a /home/isaac/x", "env A=1 cp a /home/isaac/x", "/bin/cp a /home/isaac/x",
                    "ls | xargs -I{} cp {} /home/isaac/x", "find . -name '*.c' -exec cp {} /home/isaac/x \\;",
                    "bash -c 'mkdir /home/isaac/x'", "eval 'touch /home/isaac/x'", "nice -n 5 rsync -a . /home/isaac/x",
                    "python3 -c \"open('/home/isaac/x', 'w').write('1')\"",
                    "python3 - <<'EOF'\nimport shutil\nshutil.copytree('.', '/home/isaac/par/w0')\nEOF",
                    "bash <<'EOF'\nmkdir -p /home/isaac/par\nEOF"]:
            self.assertIn("WRITE-OUTSIDE", kinds(ev("bash", command=cmd)), cmd)

    def test_background_signals_and_services(self):
        self.assertIn("BACKGROUND", kinds(ev("bash", command="python3 - <<'EOF'\nimport os\nos.system('nohup sleep 9 &')\nEOF")))
        self.assertIn("BACKGROUND", kinds(ev("bash", command="python3 -c \"import subprocess; subprocess.Popen(['sleep','9'], start_new_session=True)\"")))
        self.assertIn("SIGNAL", kinds(ev("bash", command="kill $(pgrep -f worker.py)")))
        self.assertIn("SIGNAL", kinds(ev("bash", command="pgrep -f worker | xargs kill -9")))
        self.assertIn("SIGNAL", kinds(ev("bash", command="kill -9 -1")))
        self.assertEqual(kinds(ev("bash", command="sleep 5 & kill $!")), ["BACKGROUND"])
        self.assertEqual(kinds(ev("bash", command="docker run -v /:/host alpine true")), ["HOST-SERVICE"])
        self.assertEqual(kinds(ev("bash", command="systemctl --user list-units")), ["HOST-SERVICE"])

    def test_round_three_bypasses(self):
        for cmd in ["bash -lc 'mkdir /home/isaac/x'", "sh -ec 'touch ~/x'", "bash -c 'true > /home/isaac/x'",
                    "sed -i 's/a/b/' /home/isaac/.bashrc", "perl -pi -e 's/a/b/' ~/.profile", "dd if=/dev/zero of=/home/isaac/x bs=1 count=1"]:
            self.assertIn("WRITE-OUTSIDE", kinds(ev("bash", command=cmd)), cmd)
        self.assertIn("EXEC-OUTSIDE", kinds(ev("bash", command="cd ~/permuter; python3 -m permuter tmp/x")))
        self.assertIn("EXEC-OUTSIDE", kinds(ev("bash", command="cd /home/isaac/par && python3 worker.py w0")))
        self.assertEqual(kinds(ev("bash", command="command -v nohup; sed -i 's/a/b/' src/a.c; python3 -m json.tool tmp/x.json")), [])

    def test_round_four(self):
        self.assertEqual(kinds(ev("bash", command="sed -i '/^#/d' src/a.c"), ev("bash", command="sed -i -e 's/a/b/' src/a.c"),
                               ev("bash", command="perl -pi -e 's/a/b/' src/a.c")), [])
        for cmd in ["bash -c 'echo hi' > /home/isaac/x", "command -v x > /home/isaac/y", "cd /home/isaac; sed -i 's/a/b/' .bashrc",
                    "cd ~ && perl -pi -e 's/a/b/' .profile"]:
            self.assertIn("WRITE-OUTSIDE", kinds(ev("bash", command=cmd)), cmd)
        self.assertIn("ASSIGN-OUTSIDE", kinds(ev("bash", command='p=/home/isaac/x; touch "$p"')))
        self.assertEqual(kinds(ev("bash", command=f"p={WS}/tmp/x; touch \"$p\""),
                               ev("bash", command="TTYD_BASE=~/ttyd python3 check.py a/b"),
                               ev("bash", command="PATH=$HOME/.local/bin:$PATH python3 check.py a/b")), [])

    def test_corrupted_event_log(self):
        good = ev("bash", command="ls")
        marker = lambda rc: json.dumps({"type": "launcher_pass_end", "step": "run", "rc": rc})
        # Legacy logs retain one damaged line immediately before a timeout marker.
        self.assertEqual(kinds(good, good, '{"type": "tool_exec'), [])
        self.assertEqual(kinds(good, 'garbage', '', '  '), [])
        self.assertIn("EVENTS-CORRUPT", kinds(good, ev("bash", command="ls") + 'garbage'))
        self.assertIn("EVENTS-CORRUPT", kinds(good, '{"type": "tool_exec', good))
        self.assertIn("EVENTS-CORRUPT", kinds('garbage', good, 'more garbage'))
        self.assertEqual(kinds(good, '{"type": "tool_exec', "", marker(124), good, marker(0)), [])
        self.assertIn("EVENTS-CORRUPT", kinds(good, '{"type": "tool_exec', marker(0)))
        self.assertIn("EVENTS-CORRUPT", kinds(good, 'garbage', good, marker(124)))
        self.assertEqual(kinds(good, 'garbage', marker(137)), [])
        self.assertIn("EVENTS-CORRUPT", kinds('garbage', 'more garbage', marker(124)))
        self.assertIn("EVENTS-CORRUPT", kinds(good, marker(0), '{"type": "tool_exec'))

    def pass_kinds(self, *passes):
        data = b""
        boundaries = []
        for p in passes:
            text, rc = p[:2]
            fired = p[2] if len(p) == 3 else int(rc in (124, 137))
            data += text.encode() if isinstance(text, str) else text
            boundaries.append((len(data), rc, fired))
        return sorted({k for k, _, _ in agent_audit.audit(io.BytesIO(data), WS, boundaries)})

    def test_host_timeout_boundaries_split_joined_passes_by_bytes(self):
        # The first pass ends mid-UTF-8 and without a newline; the next event still needs its own audit.
        partial = b'{"type":"message","text":"\xc3'
        self.assertEqual(self.pass_kinds((partial, 124), (ev("bash", command="echo ok") + "\n", 0)), [])
        self.assertEqual(self.pass_kinds((partial, 137), (b"", 0), (partial, 124)), [])
        for rc in (0, 1, 143):
            self.assertIn("EVENTS-CORRUPT", self.pass_kinds((partial, rc)))
        self.assertIn("WRITE-OUTSIDE", self.pass_kinds((partial, 124), (ev("write", path="/home/isaac/x"), 0)))

    def test_only_one_unterminated_final_line_per_timeout_pass(self):
        partial = '{"type":"message'
        for data in (partial + "\n", partial + "\n" + partial, partial + "\n" + ev("bash", command="ls")):
            self.assertIn("EVENTS-CORRUPT", self.pass_kinds((data, 124)), data)
        self.assertEqual(self.pass_kinds((partial, 124), (partial, 137)), [])

    def test_corrupt_lines_recover_every_embedded_tool_call(self):
        calls = ev("write", path="/home/isaac/x") + ev("bash", command="nohup python3 tmp/a.py &")
        for prefix in ("junk ", '{"type":"message","text":"broken ', "{}"):
            found = self.pass_kinds((prefix + calls + " garbage", 124))
            self.assertIn("WRITE-OUTSIDE", found)
            self.assertIn("BACKGROUND", found)
            self.assertIn("EVENTS-CORRUPT", found)
        self.assertIn("EVENTS-CORRUPT", self.pass_kinds(('{"type":"tool_execution_start","args":', 124)))
        self.assertIn("EVENTS-CORRUPT", self.pass_kinds(("junk " + ev("bash", command="ls"), 124)))

    def test_exit_codes_alone_do_not_exempt_truncation(self):
        partial = '{"type":"message'
        for rc in (124, 137):
            self.assertIn("EVENTS-CORRUPT", self.pass_kinds((partial, rc, 0)))
            data = partial.encode()
            self.assertIn("EVENTS-CORRUPT", {k for k, _, _ in agent_audit.audit(io.BytesIO(data), WS, [(len(data), rc)])})
        # Even an in-stream timeout marker does not override a boundary file.
        marker = json.dumps({"type": "launcher_pass_end", "rc": 124})
        self.assertIn("EVENTS-CORRUPT", self.pass_kinds((partial + "\n" + marker, 124, 0)))

    def test_all_final_byte_timeout_cuts_are_tolerated(self):
        for partial in ('{"type":"message","n":0.', '{"type":"message","n":1e',
                        '{"type":"message","text":"\\u00', '{"type":"message","text":"\\u00ff',
                        '{"type":"message","text":"mentions tool_execution_start',
                        '{"type":"message","text":"mentions \\"type\\":\\"tool_execution_start\\"'):
            self.assertEqual(self.pass_kinds((partial, 124, 1)), [], partial)
            self.assertIn("EVENTS-CORRUPT", self.pass_kinds((partial, 124, 0)), partial)
            self.assertIn("EVENTS-CORRUPT", self.pass_kinds((partial + "\n", 124, 1)), partial)

    def test_legacy_marker_tolerance_only_applies_without_boundary_file(self):
        with tempfile.TemporaryDirectory() as d:
            event_file = Path(d) / "events.jsonl"
            event_file.write_text('{"type":"message\n\n' + json.dumps({"type": "launcher_pass_end", "rc": 124}) + "\n")
            command = [sys.executable, agent_audit.__file__, str(event_file), WS]
            result = subprocess.run(command, capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            (Path(d) / "passes.txt").write_text(f"{event_file.stat().st_size} 124 0\n")
            result = subprocess.run(command, capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
            self.assertIn("EVENTS-CORRUPT", result.stdout)

    def test_escaped_event_types_and_keys_cannot_hide_tool_events(self):
        for key in ('type', r'\u0074ype'):
            for value in ('tool_execution_start', r'tool_execution_\u0073tart'):
                partial = '{"' + key + '":"' + value + '","args":'
                self.assertIn("EVENTS-CORRUPT", self.pass_kinds((partial, 124, 1)), partial)
        for partial in ('{"type":"tool_execution_start', r'{"\u0074ype":"tool_execution_\u0073tart'):
            self.assertIn("EVENTS-CORRUPT", self.pass_kinds((partial, 124, 1)), partial)
        call = ev("write", path="/home/isaac/x").replace('tool_execution_start', r'tool_execution_\u0073tart')
        found = self.pass_kinds(("junk " + call, 124, 1))
        self.assertIn("WRITE-OUTSIDE", found)
        self.assertIn("EVENTS-CORRUPT", found)

    def test_recovery_decode_work_is_bounded_on_large_broken_lines(self):
        for line in ('{' * 500_000, '{"a":' * 100_000):
            decoder = json.JSONDecoder()
            with mock.patch.object(agent_audit.json, "JSONDecoder", return_value=decoder), \
                 mock.patch.object(decoder, "raw_decode", wraps=decoder.raw_decode) as decode:
                found = self.pass_kinds((line, 124, 1))
                work = sum(len(line) - call.args[1] for call in decode.call_args_list)
                self.assertLessEqual(work, 4 * len(line))
                if line.startswith('{"'):
                    self.assertEqual(found, [])  # a valid nested prefix remains exempt even when recovery is capped
                else:
                    self.assertIn("EVENTS-CORRUPT", found)

    def test_realistic_nested_pi_events_at_many_timeout_cuts(self):
        # Pi's end events repeat complete messages, content blocks, usage and model metadata. Even a normal
        # dozen-message agent_end exhausts suffix-charged recovery long before the end of the first message.
        usage = {"input": 41082, "output": 234, "cacheRead": 39555, "cacheWrite": 0,
                 "totalTokens": 41316, "cost": {"input": 0.123, "output": 0.004, "total": 0.127}}
        assistant = {"role": "assistant", "content": [
            {"type": "thinking", "thinking": "Inspect nested events and then run the checks. " * 12,
             "thinkingSignature": "signature"},
            {"type": "toolCall", "id": "call_01", "name": "bash", "arguments": {"command": "ls tools"}},
            {"type": "text", "text": "Nested objects, Unicode é, and JSON escapes \\u0074. " * 12}],
            "api": "anthropic-messages", "provider": "anthropic", "model": "claude-sonnet-4-5",
            "usage": usage, "stopReason": "toolUse", "timestamp": 1791012345678}
        result = {"role": "toolResult", "toolCallId": "call_01", "toolName": "bash", "content": [
            {"type": "text", "text": "a.py\nb.py\nc.py\n" * 80}], "details": {"exitCode": 0},
            "isError": False, "timestamp": 1791012346000}
        events = [{"type": "agent_end", "messages": [assistant, result] * 6},
                  {"type": "turn_end", "message": assistant, "toolResults": [result]},
                  {"type": "message_end", "message": assistant}]
        for event in events:
            line = json.dumps(event, ensure_ascii=True, separators=(",", ":"))
            cuts = set(range(1, len(line), max(1, len(line) // 160)))
            cuts.update(i for i, c in enumerate(line) if c in '{}[],:')
            cuts.update(range(len(line) - 12, len(line)))
            for cut in sorted(cuts - {0}):
                partial = line[:cut]
                with self.subTest(event=event["type"], cut=cut):
                    self.assertEqual(self.pass_kinds((partial, 124, 1)), [])
            # Demonstrate that this exercises the old recovery-budget failure, rather than only small prefixes.
            self.assertTrue(agent_audit.recover_events(line[:-1], WS, 1)[2])

    def test_host_timeout_rejects_non_prefix_corruption(self):
        corrupt = ['{}garbage', '{"type":bad}', '{"type":bad', '{"a":trueX', '{"a":falsee',
                   '{"a":nulX', '{"a":01', '{"a":1.e', '{"a":--1', '{"a":+1', '{"a":1,}',
                   '{"a":[1,]', '{"a" 1', '{"a":}', '{"a":[}', '{"a":"bad\\q',
                   '{"a":"bad\\u00Z', '{"a":"raw\t', 'garbage', '[1,']
        for line in corrupt:
            with self.subTest(line=line):
                self.assertIn("EVENTS-CORRUPT", self.pass_kinds((line, 124, 1)))

    def test_unquoted_and_escaped_tool_types_never_get_legacy_or_timeout_tolerance(self):
        for key in ('"type"', 'type', r'"\u0074ype"', r'\u0074ype'):
            for value in ('"tool_execution_start"', 'tool_execution_start',
                          r'"tool_execution_\u0073tart"', r'tool_execution_\u0073tart'):
                line = '{' + key + ':' + value + ',"args":'
                with self.subTest(line=line):
                    self.assertTrue(agent_audit.recover_events(line, WS, 1)[1])
                    self.assertIn("EVENTS-CORRUPT", self.pass_kinds((line, 124, 1)))
                    self.assertIn("EVENTS-CORRUPT", kinds(line))
                    self.assertIn("EVENTS-CORRUPT", kinds(line, '{"type":"launcher_pass_end","rc":124}'))

    def test_tool_type_scan_survives_exhausted_recovery_budget(self):
        line = '{"nested":' * 100 + '{"type":"tool_execution_start","args":'
        self.assertTrue(agent_audit.recover_events(line, WS, 1)[2])
        self.assertIn("EVENTS-CORRUPT", self.pass_kinds((line, 124, 1)))

    def test_marker_free_legacy_tolerance_requires_one_final_non_tool_bad_line(self):
        good = '{"type":"message_end"}'
        self.assertEqual(kinds(good, 'garbage'), [])
        self.assertEqual(kinds(good, '{"type":"message_end",', '', '  '), [])
        for lines in [('garbage', good), ('garbage', good, 'more garbage'),
                      (good, '{"type":"launcher_pass_end","rc":0}', 'garbage')]:
            self.assertIn("EVENTS-CORRUPT", kinds(*lines))
        self.assertIn("EVENTS-CORRUPT", self.pass_kinds((good + '\ngarbage', 124, 1)))

    def test_boundary_metadata_must_be_ordered_and_cover_the_log(self):
        data = (ev("write", path="/home/isaac/x") + "\n").encode()
        for boundaries in ([], [(len(data) + 1, 124)], [(len(data) - 1, 124)],
                           [(len(data), 124), (0, 0)], [(-1, 124)], [(len(data), 999)]):
            found = {k for k, _, _ in agent_audit.audit(io.BytesIO(data), WS, boundaries)}
            self.assertIn("EVENTS-CORRUPT", found)
            # Invalid metadata never prevents recovering calls.
            self.assertIn("WRITE-OUTSIDE", found)

    def test_cli_reads_sibling_host_boundary_file(self):
        with tempfile.TemporaryDirectory() as d:
            event_file = Path(d) / "events.jsonl"
            event_file.write_bytes(b'{"type":"message')
            command = [sys.executable, agent_audit.__file__, str(event_file), WS]
            self.assertEqual(subprocess.run(command, capture_output=True, timeout=5).returncode, 0)
            boundary_file = Path(d) / "passes.txt"
            boundary_file.write_text(f"{event_file.stat().st_size} 124 1\n")
            result = subprocess.run(command, capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            for text in ("not numbers\n", "0 124 extra\n", "0 124 1 1\n", "0 124 2\n", f"{event_file.stat().st_size} 124\n", ""):
                boundary_file.write_text(text)
                result = subprocess.run(command, capture_output=True, text=True, timeout=5)
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn("EVENTS-CORRUPT", result.stdout)

    def test_copying_in_from_outside_is_a_read(self):
        self.assertNotIn("WRITE-OUTSIDE", kinds(ev("bash", command="cp /home/isaac/ttyd-w71base/build/x.o build/x.o")))
        self.assertNotIn("WRITE-OUTSIDE", kinds(ev("bash", command="rsync -a /home/isaac/base/src/ src/")))
        for cmd in ["cp build/x.o /home/isaac/x.o", "cp -t /home/isaac/par a b", "mv /home/isaac/x tmp/x",
                    "ln -s ../x /home/isaac/y",
                    # target-directory forms: bundled, attached, long with and without =
                    "cp -at /home/isaac/par a b", "install -Dt /home/isaac/x f", "cp -t/home/isaac/par a b",
                    "cp --target-directory=/home/isaac/par a b", "cp --target-directory /home/isaac/par a b",
                    # another host is outside
                    "rsync -a src/ otherhost:backup/", "scp tmp/x.o host:/tmp/x.o"]:
            self.assertIn("WRITE-OUTSIDE", kinds(ev("bash", command=cmd)), cmd)

    def test_running_tools_from_outside(self):
        self.assertIn("EXEC-OUTSIDE", kinds(ev("bash", command="python3 ~/permuter/permuter.py tmp/perm")))
        self.assertIn("EXEC-OUTSIDE", kinds(ev("bash", command="/home/isaac/permuter/import.py src/x.c")))

    def test_words_in_arguments_are_not_commands(self):
        self.assertEqual(kinds(
            ev("bash", command="grep -n 'at' src/a.c; echo look at this; grep screen src/a.c | head"),
            ev("bash", command="git diff --stat; echo 'nohup is not allowed' > tmp/note"),
            ev("bash", command="python3 tools/rodatalayout.py battle/battle_camera .data 2>&1 | grep -c '|'"),
        ), [])


if __name__ == "__main__":
    unittest.main()
