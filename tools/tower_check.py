#!/usr/bin/env python3
"""Look for signs of an agent getting past its sandbox on the build host. Parses no agent commands: it looks at the host.

    python3 tools/tower_check.py [--allow FILE] [--since MINUTES]   run on the host

Exit 0 clean, 1 findings, 2 the check itself failed (no allowlist, a crash): the launcher reports 2 as FAILED.

Findings (exit 1):
  HOME-ENTRY     an entry of ~ matching no allowlist pattern (fnmatch, one per line in FILE, default
                 ~/.config/ttyd-tower-allowlist, # comments; kept on the host, not in the repo: it lists the home dir)
  STARTUP-WRITE  a shell startup file, ~/.ssh, ~/.config/systemd or ~/.config/autostart changed in the last MINUTES
  UNBOXED        a process in ttyd-agents.slice (where only sandboxes run) outside a sandbox's mount namespace, or a
                 process in a workspace that is neither in a sandbox nor part of the launcher's chain that starts one
  NEW-CONTAINER  a docker container created in the last MINUTES
  NEW-UNIT       a transient user unit (systemd-run's run-*) started in the last MINUTES
Information (printed, not counted): recent writes per directory under ~ outside the workspaces and archives, running
containers, stale agent slot files. Run by tools/wave_launch.py at the end of each wave with MINUTES covering the
whole wave; it defaults to 60."""
import fnmatch
import glob
import os
import subprocess
import sys
import time

HOME = os.path.expanduser("~")
DEFAULT_ALLOW = os.path.join(HOME, ".config", "ttyd-tower-allowlist")
STARTUP = [".bashrc", ".bash_profile", ".bash_login", ".profile", ".zshrc", ".zshenv", ".zprofile", ".zlogin",
           ".ssh", ".config/systemd", ".config/autostart", ".config/environment.d", ".pam_environment", ".xprofile"]
# written by ordinary work all the time: recent-write counts skip them
QUIET = ["ttyd-agents", "ttyd-agents-out", "ttyd-agents-priv", "ttyd-agents-slots", "ttyd-archive", ".cache", ".npm",
         ".nvm", ".local", ".mozilla", ".thunderbird", ".claude", ".codex", ".pi", ".config", ".docker", ".triton"]


def arg(name, default):
    return sys.argv[sys.argv.index(name) + 1] if name in sys.argv else default


def allowlist(path):
    if not os.path.exists(path):
        print(f"no allowlist at {path}: list the expected entries of ~ there, one fnmatch pattern per line", file=sys.stderr)
        sys.exit(2)
    return [l.strip() for l in open(path) if l.strip() and not l.lstrip().startswith("#")]


def procs():
    out = {}
    for pid in os.listdir("/proc"):
        if not pid.isdigit():
            continue
        try:
            stat = open(f"/proc/{pid}/stat").read()
            ppid = int(stat[stat.rindex(")") + 2:].split()[1])
            comm = stat[stat.index("(") + 1:stat.rindex(")")]
            args = open(f"/proc/{pid}/cmdline", "rb").read().split(b"\0")
            cwd = os.readlink(f"/proc/{pid}/cwd")
            mnt = os.readlink(f"/proc/{pid}/ns/mnt")
            cg = open(f"/proc/{pid}/cgroup").read()
        except (OSError, ValueError):
            continue
        out[int(pid)] = (ppid, comm, " ".join(a.decode(errors="replace") for a in args if a), cwd, mnt, cg)
    return out


def sh(cmd):
    try:
        return subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=60).stdout
    except subprocess.TimeoutExpired:
        return ""


def main():
    since = float(arg("--since", 60))
    t0 = time.time() - since * 60
    allow = allowlist(arg("--allow", DEFAULT_ALLOW))
    found, info = [], []
    for name in sorted(os.listdir(HOME)):
        if not any(fnmatch.fnmatch(name, pat) for pat in allow):
            found.append(f"HOME-ENTRY    ~/{name}  (modified {time.ctime(os.lstat(os.path.join(HOME, name)).st_mtime)})")
    for rel in STARTUP:
        p = os.path.join(HOME, rel)
        for q in [p] + (glob.glob(p + "/**", recursive=True) if os.path.isdir(p) else []):
            try:
                if os.lstat(q).st_mtime > t0:
                    found.append(f"STARTUP-WRITE ~/{os.path.relpath(q, HOME)}  (modified {time.ctime(os.lstat(q).st_mtime)})")
            except OSError:
                pass

    ps = procs()
    host_mnt = os.readlink("/proc/self/ns/mnt")
    agents_root = os.path.join(HOME, "ttyd-agents") + "/"
    # the launcher's chain for each sandbox: ancestors of the outer bwrap, and the pipe readers beside it
    chain = set()
    for pid, (ppid, comm, args, cwd, mnt, cg) in ps.items():
        if comm == "bwrap" and mnt == host_mnt:
            p = ppid
            while p in ps and p > 1:
                chain.add(p)
                p = ps[p][0]
    for pid, (ppid, comm, args, cwd, mnt, cg) in ps.items():
        if ppid in chain and comm in ("cat", "timeout", "bash", "sh"):
            chain.add(pid)
    for pid, (ppid, comm, args, cwd, mnt, cg) in ps.items():
        boxed = mnt != host_mnt
        if "ttyd-agents.slice" in cg and not boxed and comm != "bwrap":
            found.append(f"UNBOXED       pid {pid} in ttyd-agents.slice outside a sandbox: {args[:160]}")
        elif cwd.startswith(agents_root) and not boxed and pid not in chain and comm != "bwrap":
            found.append(f"UNBOXED       pid {pid} cwd {cwd} outside a sandbox: {args[:160]}")

    for line in sh("docker ps --format '{{.ID}}\t{{.Names}}\t{{.Image}}\t{{.CreatedAt}}' 2>/dev/null").splitlines():
        cid, name, image, created = (line.split("\t") + ["", "", "", ""])[:4]
        try:
            ts = time.mktime(time.strptime(created[:19], "%Y-%m-%d %H:%M:%S"))
        except ValueError:
            ts = 0
        (found if ts > t0 else info).append(f"{'NEW-CONTAINER' if ts > t0 else 'container':13s} {name} ({image}) created {created[:19]}")
    for line in sh("systemctl --user list-units --plain --no-legend --state=active 'run-*' 2>/dev/null").splitlines():
        unit = line.split()[0] if line.split() else ""
        ts = sh(f"systemctl --user show -p ActiveEnterTimestampMonotonic --value {unit}").strip()
        try:
            age = time.clock_gettime(time.CLOCK_MONOTONIC) - int(ts) / 1e6
        except ValueError:
            age = 1e9
        if age < since * 60:
            found.append(f"NEW-UNIT      {line[:160]}")

    counts = {}
    for name in os.listdir(HOME):
        if name in QUIET:
            continue
        for root, dirs, files in os.walk(os.path.join(HOME, name)):
            dirs[:] = [d for d in dirs if d not in (".git", "node_modules", "__pycache__")]
            for f in files:
                try:
                    if os.lstat(os.path.join(root, f)).st_mtime > t0:
                        counts[name] = counts.get(name, 0) + 1
                except OSError:
                    pass
    for name, n in sorted(counts.items(), key=lambda x: -x[1]):
        info.append(f"recent writes ~/{name}: {n} file(s) in the last {since:.0f} min")
    for f in glob.glob(os.path.join(HOME, "ttyd-agents-slots", "*", "*.slot")):
        if os.path.getmtime(f) < time.time() - 6 * 3600:
            info.append(f"stale slot    {os.path.relpath(f, HOME)}")

    for f in found + info:
        print(f)
    print(f"{len(found)} finding(s)")
    sys.exit(1 if found else 0)


if __name__ == "__main__":
    try:
        main()
    except SystemExit:
        raise
    except BaseException as e:
        print(f"tower_check failed: {e!r}", file=sys.stderr)
        sys.exit(2)
