"""tools/agent_audit.py: what it flags in an agent's event log."""
import json
import os
import sys
import unittest

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
        self.assertEqual(kinds(ev("bash", command=f"p={WS}/tmp/x; touch \"$p\"")), [])

    def test_corrupted_event_log(self):
        good = ev("bash", command="ls")
        self.assertEqual(kinds(good, good, '{"type": "tool_exec'), [])  # a cut-off last line
        self.assertIn("EVENTS-CORRUPT", kinds(good, '{"type": "tool_exec', good))
        self.assertIn("EVENTS-CORRUPT", kinds('garbage', good, 'more garbage'))

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
