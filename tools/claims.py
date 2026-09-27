#!/usr/bin/env python3
"""Claims: which wave owns which unit, so parallel waves never work on the same file.

  python3 tools/claims.py claim WAVE UNIT...   claim units for WAVE; prints each as CLAIMED or TAKEN (by whom)
  python3 tools/claims.py release WAVE [UNIT...]   release WAVE's claims (all of them if no units are given)
  python3 tools/claims.py list

A launcher claims its units before starting agents and works only on the ones it got; the orchestrator releases
the wave's claims once its PR is merged (or abandoned). Claims expire after --hours (default 24), so a wave that
crashed doesn't lock its files forever. UNIT is the source path without src/ and .c, as in the launchers.

The database is shared by every checkout of the repo (worktrees included): scratch/claims.sqlite in the main
checkout. Each claim runs in one transaction, so two launchers claiming at once can't both get a unit.
"""
import argparse
import math
import os
import re
import sqlite3
import subprocess
import time
from pathlib import Path


def db_path():
    r = subprocess.run(["git", "rev-parse", "--path-format=absolute", "--git-common-dir"],
                       cwd=Path(__file__).resolve().parent, capture_output=True, text=True)
    if r.returncode or not r.stdout.strip():
        raise SystemExit("claims.py: not inside the ttyd git repo, so there is no shared claims database to use")
    return Path(r.stdout.strip()).parent / "scratch" / "claims.sqlite"


def unit_name(u):
    """event/evt_npc, src/event/evt_npc.c, ./src/event/evt_npc.c, an absolute path and rels/pik/src/unit/x.c all
    name a unit the same way; anything that doesn't reduce to a plain relative unit path is refused. The REL
    name is dropped, so the same path in two RELs (or in a REL and src/) is one claim: the safe side."""
    root = str(Path(__file__).resolve().parent.parent) + "/"
    u = os.path.normpath(u)
    if u.startswith(root):
        u = u[len(root):]
    u = re.sub(r"^(?:rels/[^/]+/)?src/", "", u)
    u = u[:-2] if u.endswith(".c") else u
    if u in (".", "src") or u.startswith(("/", "..")) or not re.fullmatch(r"[\w./-]+", u):
        raise SystemExit(f"claims.py: not a unit name: {u}")
    return u


def connect(path=None):
    path = Path(path or db_path())
    path.parent.mkdir(parents=True, exist_ok=True)
    con = sqlite3.connect(path, isolation_level=None, timeout=30)
    con.execute("CREATE TABLE IF NOT EXISTS claims (unit TEXT PRIMARY KEY, wave TEXT NOT NULL, "
                "claimed REAL NOT NULL, expires REAL NOT NULL)")
    return con


def claim(con, wave, units, hours=24.0, now=None):
    """Claim units for wave. Returns {unit: owning wave}; a unit is ours if its owner is wave. Claiming a unit the
    wave already holds renews its expiry (a wave whose PR is still in review keeps its units)."""
    now = time.time() if now is None else now
    if not (isinstance(hours, (int, float)) and math.isfinite(hours) and hours > 0):
        raise SystemExit(f"claims.py: claim duration must be a positive number of hours, not {hours}")
    units = [unit_name(u) for u in units]
    out = {}
    con.execute("BEGIN IMMEDIATE")
    try:
        con.execute("DELETE FROM claims WHERE expires < ?", (now,))
        for u in units:
            row = con.execute("SELECT wave FROM claims WHERE unit = ?", (u,)).fetchone()
            if row is None:
                con.execute("INSERT INTO claims VALUES (?, ?, ?, ?)", (u, wave, now, now + hours * 3600))
                out[u] = wave
            else:
                if row[0] == wave:
                    con.execute("UPDATE claims SET expires = ? WHERE unit = ?", (now + hours * 3600, u))
                out[u] = row[0]
        con.execute("COMMIT")
    except BaseException:
        con.execute("ROLLBACK")
        raise
    return out


def release(con, wave, units=None):
    if units:
        units = [unit_name(u) for u in units]
        q = "DELETE FROM claims WHERE wave = ? AND unit IN (%s)" % ",".join("?" * len(units))
        return con.execute(q, (wave, *units)).rowcount
    return con.execute("DELETE FROM claims WHERE wave = ?", (wave,)).rowcount


def rows(con, now=None):
    now = time.time() if now is None else now
    return con.execute("SELECT unit, wave, claimed, expires FROM claims WHERE expires >= ? ORDER BY wave, unit",
                       (now,)).fetchall()


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("claim")
    c.add_argument("wave")
    c.add_argument("units", nargs="+")
    c.add_argument("--hours", type=float, default=24.0)
    r = sub.add_parser("release")
    r.add_argument("wave")
    r.add_argument("units", nargs="*")
    sub.add_parser("list")
    a = p.parse_args()
    con = connect()
    if a.cmd == "claim":
        for u, owner in claim(con, a.wave, a.units, a.hours).items():
            print(f"CLAIMED {u}" if owner == a.wave else f"TAKEN   {u} (by {owner})")
    elif a.cmd == "release":
        print(f"released {release(con, a.wave, a.units)} claim(s) of {a.wave}")
    else:
        for u, w, c_, e in rows(con):
            print(f"{w:10s} {u:40s} until {time.strftime('%m-%d %H:%M', time.localtime(e))}")


if __name__ == "__main__":
    main()
