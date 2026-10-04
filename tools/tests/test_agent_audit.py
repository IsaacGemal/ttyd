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
                               "import os; os.system('true &')\nprint('/home/isaac/elsewhere')\nEOF"),
            ev("read", path=f"{WS}/src/battle/battle_camera.c"),
            ev("write", path=f"{WS}/REPORT.md", content="Model: x"),
            ev("bash", command=f"cd {WS} && make 2>&1 >/dev/null; ls /tmp/x &>/dev/null"),
        ), [])

    def test_reads_outside_are_reported_not_flagged(self):
        flags = agent_audit.audit([ev("bash", command="cat /home/isaac/permuter/settings.toml")], WS)
        self.assertEqual([k for k, _, _ in flags], ["READ-OUTSIDE"])

    def test_background_inside_the_workspace_is_still_flagged(self):
        self.assertEqual(kinds(ev("bash", command=f"cd {WS} && setsid python3 tmp/search.py > tmp/log 2>&1")), ["BACKGROUND"])


if __name__ == "__main__":
    unittest.main()
