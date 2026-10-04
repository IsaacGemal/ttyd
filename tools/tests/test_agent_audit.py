"""tools/agent_audit.py: what it flags in an agent's event log."""
import io
import json
import os
import subprocess
import sys
import tempfile
import unittest
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
        # Logs without trusted host boundaries get no truncation exemption, even with a forged marker.
        self.assertIn("EVENTS-CORRUPT", kinds(good, good, '{"type": "tool_exec'))
        self.assertIn("EVENTS-CORRUPT", kinds(good, '{"type": "tool_exec', good))
        self.assertIn("EVENTS-CORRUPT", kinds('garbage', good, 'more garbage'))
        self.assertIn("EVENTS-CORRUPT", kinds(good, '{"type": "tool_exec', "", marker(124), good, marker(0)))
        self.assertIn("EVENTS-CORRUPT", kinds(good, '{"type": "tool_exec', marker(0)))
        self.assertIn("EVENTS-CORRUPT", kinds(good, 'garbage', good, marker(124)))
        self.assertIn("EVENTS-CORRUPT", kinds(good, marker(0), '{"type": "tool_exec'))

    def pass_kinds(self, *passes):
        data = b""
        boundaries = []
        for text, rc in passes:
            data += text.encode() if isinstance(text, str) else text
            boundaries.append((len(data), rc))
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
        for data in (partial + "\n", partial + "\n" + partial, partial + "\n" + ev("bash", command="ls"), "garbage",
                     '{}garbage', '{"type":bad}', '{"type":"bad\\q'):
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
            self.assertEqual(subprocess.run(command, capture_output=True, timeout=5).returncode, 1)
            boundary_file = Path(d) / "passes.txt"
            boundary_file.write_text(f"{event_file.stat().st_size} 124\n")
            result = subprocess.run(command, capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            for text in ("not numbers\n", "0 124 extra\n", ""):
                boundary_file.write_text(text)
                result = subprocess.run(command, capture_output=True, text=True, timeout=5)
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn("EVENTS-CORRUPT", result.stdout)

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
