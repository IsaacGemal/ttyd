"""Rewrite data tables in a C file as typed struct initializers, rendered from the original object.

    python3 tools/struct_render.py FILE.c SYMBOL=TYPE [SYMBOL=TYPE ...]
    python3 tools/struct_render.py FILE.c --auto        (type what the battle graph reaches, see auto_types)
    python3 tools/struct_render.py --all [SKIP.c ...]   (--auto on every file, then fix references, externs, casts)
    python3 tools/struct_render.py --check-layouts      (compile LAYOUTS against the headers)

The bytes and pointers come from dtk's disassembly of the original (build/G8MJ01/[MOD/]asm/<unit>.s), so a
rendered table holds exactly the original data; the link (tools/agent_check.py --final, or the checksum) proves
it. Layouts are in LAYOUTS below, one entry per struct, checked against the headers by --check-layouts."""
import re, struct, sys, os, subprocess

# ---------------------------------------------------------------- layouts
# field: (offset, kind, name[, extra]); kinds: s8 u8 s16 u16 s32 u32 x32 (u32 in hex) f32 vec,
# ptr (extra: pointee type or None), str (const char*), arr (extra: (kind, count)), enum (extra: enum name)
LAYOUTS = {
    'BattleUnitKind': (0xC4, [
        (0x0, 'enum', 'mUnitType', 'BattleUnitType'), (0x4, 'str', 'name'), (0x8, 's16', 'maxHp'),
        (0xA, 's16', 'maxFp'), (0xC, 'u8', 'mDangerHP'), (0xD, 'u8', 'mPerilHP'), (0xE, 'u8', 'mLevel'),
        (0xF, 'u8', 'mBonusExp'), (0x10, 'u8', 'bonusCoinRolls'), (0x11, 'u8', 'bonusCoinRate'),
        (0x12, 'u8', 'baseCoin'), (0x13, 'u8', 'field_0x13'), (0x14, 's16', 'field_0x14'), (0x16, 's16', 'width'),
        (0x18, 's16', 'height'), (0x1A, 'arr', 'statusIconOffset', ('s16', 2)), (0x20, 'vec', 'centerOffset'),
        (0x2C, 'arr', 'hpGaugeOffset', ('s16', 2)), (0x30, 'vec', 'talkTogeBaseOffset'), (0x3C, 'vec', 'field_0x3C'),
        (0x48, 'vec', 'burnFlameOffset'), (0x54, 'f32', 'field_0x54'), (0x58, 'f32', 'field_0x58'),
        (0x5C, 'vec', 'bintaHitOffset'), (0x68, 'vec', 'kissHitOffset'), (0x74, 'vec', 'cutBaseOffset'),
        (0x80, 'f32', 'cutWidth'), (0x84, 'f32', 'cutHeight'), (0x88, 's8', 'turnOrder'),
        (0x89, 's8', 'turnOrderVariance'), (0x8A, 's8', 'swallowRate'), (0x8B, 's8', 'swallowAttribute'),
        (0x8C, 'u8', 'field_0x8C'), (0x8D, 'u8', 'itemStealParam'), (0x90, 'vec', 'starPointDispOffset'),
        (0x9C, 'str', 'damageSfxName'), (0xA0, 'str', 'fireDamageSfxName'), (0xA4, 'str', 'iceDamageSfxName'),
        (0xA8, 'str', 'electricDamageSfxName'), (0xAC, 'x32', 'attributeFlags'),
        (0xB0, 'ptr', 'vulnStatus', 'BattleVulnerableStatus'), (0xB4, 'u8', 'numParts'),
        (0xB8, 'ptr', 'mParts', 'BattleUnitKindPart'), (0xBC, 'ptr', 'initEvtScript', None),
        (0xC0, 'ptr', 'dataTable', 'BattleDataEntry')]),
    'BattleUnitKindPart': (0x4C, [
        (0x0, 's32', 'partNum'), (0x4, 'str', 'partName'), (0x8, 'str', 'modelName'),
        (0xC, 'vec', 'positionOffset'), (0x18, 'vec', 'hitBaseOffset'), (0x24, 'vec', 'hitCursorBaseOffset'),
        (0x30, 's16', 'field_0x30'), (0x32, 's16', 'field_0x32'), (0x34, 's16', 'baseAlpha'),
        (0x38, 'ptr', 'defense', 'BattleUnitDefense'), (0x3C, 'ptr', 'defenseAttr', 'BattleUnitDefenseAttr'),
        (0x40, 'x32', 'attributes'), (0x44, 'x32', 'counterAttributes'),
        (0x48, 'ptr', 'poseTable', 'BattleDispPose')]),
    'BattleUnitDefense': (0x5, [(0x0, 'arr', 'defenses', ('u8', 5))]),
    'BattleUnitDefenseAttr': (0x5, [(0x0, 'arr', 'defenseAttrs', ('u8', 5))]),
    'BattleVulnerableStatus': (0x16, [(i, 'u8', n) for i, n in enumerate(
        'sleepRate stopRate dizzyRate poisonRate confuseRate electricRate burnRate freezeRate hugeRate tinyRate '
        'attackUpRate attackDownRate defenseUpRate defenseDownRate allergicRate frightRate galeForceRate fastRate '
        'slowRate dodgyRate invisibleRate instaKillRate'.split())]),
    'BattleDataEntry': (0x8, [(0x0, 'hex', 'tag'), (0x4, 'ptr', 'data', None)]),
    'BattleDispPose': (0x8, [(0x0, 'hex', 'poseId'), (0x4, 'str', 'poseName')]),
    'BattleUnitSetup': (0x30, [
        (0x0, 'ptr', 'mUnitKindParams', 'BattleUnitKind'), (0x4, 's8', 'alliance'), (0x8, 'x32', 'field_0x8'),
        (0xC, 'vec', 'position'), (0x18, 's32', 'field_0x18'), (0x1C, 'arr', 'unitWork', ('s32', 4)),
        (0x2C, 'ptr', 'itemDropTable', 'ItemDropData')]),
    'ItemDropData': (0x8, [(0x0, 'enum', 'type', 'ItemType'), (0x4, 's16', 'holdWeight'), (0x6, 's16', 'dropWeight')]),
    'BattleGroupSetup': (0x20, [
        (0x0, 's32', 'numEnemies'), (0x4, 'ptr', 'enemyData', 'BattleUnitSetup'), (0x8, 's32', 'field_0x8'),
        (0xC, 's32', 'field_0xC'), (0x10, 's32', 'field_0x10'), (0x14, 'ptr', 'heartDropParams', 's32'),
        (0x18, 'ptr', 'flowerDropParams', 's32'), (0x1C, 's32', 'field_0x1C')]),
    'NpcBattleSetupEntry': (0xC, [
        (0x0, 's32', 'weight'), (0x4, 'ptr', 'setup', 'BattleGroupSetup'),
        (0x8, 'ptr', 'stageData', 'BattleStageData')]),
    'BattleStageData': (0x10, [
        (0x0, 'str', 'mGlobalStageDataDir'), (0x4, 'str', 'mSpecificStageDataDir'), (0x8, 'u32', 'mNumObjects'),
        (0xC, 'ptr', 'mObjects', 'BattleStageObjectData')]),
    'BattleStageObjectData': (0x18, [
        (0x0, 'str', 'name'), (0x4, 'u16', 'mSpawnSpecialUnitType'), (0x6, 'u16', 'mLayer'),
        (0x8, 'vec', 'mPosition'), (0x14, 'u8', 'mNumFramesToStartFalling'), (0x15, 'u8', 'mNumFramesToFall'),
        (0x16, 'arr', 'pad_16', ('u8', 2))]),
    'BattleSetupData': (0x44, [
        (0x0, 'str', 'name'), (0x4, 'str', 'field_0x4'), (0x8, 's32', 'field_0x8'), (0xC, 's32', 'flag'),
        (0x10, 'ptr', 'onLoadouts', 'NpcBattleSetupEntry'), (0x14, 'ptr', 'offLoadouts', 'NpcBattleSetupEntry'),
        (0x18, 's32', 'field_0x18'), (0x1C, 's32', 'field_0x1C'), (0x20, 'arr', 'field_0x20', ('s8', 32)),
        (0x40, 'str', 'music')]),
    'BeroEntry': (0x3C, [
        (0x0, 'str', 'hitName'), (0x4, 'u16', 'kinddirArg1'), (0x6, 'u16', 'kinddirArg2'), (0x8, 'hex', 'kinddirArg3'),
        (0xC, 's32', 'field_0xC'), (0x10, 's32', 'unknown1'), (0x14, 's32', 'unknown2'), (0x18, 's32', 'field_0x18'),
        (0x1C, 'ptr', 'enterEvent', None), (0x20, 's32', 'field_0x20'), (0x24, 'ptr', 'exitEvent', None),
        (0x28, 'str', 'nextareaArg1'), (0x2C, 'str', 'nextareaArg2'), (0x30, 'u16', 'animeArg1'),
        (0x32, 'u16', 'animeArg2'), (0x34, 'ptr', 'animeArg3', None), (0x38, 'ptr', 'animeArg4', None)]),
    'BattleWeapon': (0xC0, [
        (0x0, 'str', 'name'), (0x4, 's16', 'icon'), (0x8, 'enum', 'item', 'ItemType'), (0xC, 'str', 'description'),
        (0x10, 'u8', 'baseAccuracy'), (0x11, 'u8', 'baseFpCost'), (0x12, 'u8', 'baseSpCost'),
        (0x13, 'u8', 'superguardType'), (0x14, 'f32', 'field_0x14'), (0x18, 's8', 'mStylishCommandMultiplier'),
        (0x19, 's8', 'field_0x19'), (0x1A, 's8', 'mBingoSlotChance'), (0x1B, 's8', 'field_0x1B'),
        (0x1C, 'ptr', 'hpDamageFn', None), (0x20, 'arr', 'hpDamageWork', ('u32', 8)),
        (0x40, 'ptr', 'fpDamageFn', None), (0x44, 'arr', 'fpDamageWork', ('u32', 8)),
        (0x64, 'x32', 'targetClassFlags'), (0x68, 'x32', 'targetPropertyFlags'), (0x6C, 'u8', 'element'),
        (0x6D, 's8', 'field_0x6D'), (0x6E, 'u8', 'actionLevel'), (0x6F, 's8', 'field_0x6F'),
        (0x70, 'str', 'acHelpMsg'), (0x74, 'x32', 'special'), (0x78, 'x32', 'counterResistanceFlags'),
        (0x7C, 'x32', 'targetWeightingFlags')] +
        [(0x80 + i, 'u8' if n.endswith('Chance') else 's8', n) for i, n in enumerate(
            'sleepChance sleepTime stopChance stopTime dizzyChance dizzyTime poisonChance poisonTime poisonStrength '
            'confuseChance confuseTime electricChance electricTime dodgyChance dodgyTime burnChance burnTime '
            'freezeChance freezeTime sizeChangeChance sizeChangeTime sizeChangeStrength atkChangeChance '
            'atkChangeTime atkChangeStrength defChangeChance defChangeTime defChangeStrength allergicChance '
            'allergicTime ohkoChance chargeStrength fastChance fastTime slowChance slowTime frightChance '
            'galeForceChance paybackTime holdFastTime invisibleChance invisibleTime hpRegenTime hpRegenStrength '
            'fpRegenTime fpRegenStrength'.split())] +
        [(0xB0, 'ptr', 'mAttackEvtScript', None), (0xB4, 'arr', 'field_B4', ('u8', 12))]),
}
LAYOUTS['char*'] = (0x4, [(0x0, 'str', '')])  # a raw table of nothing but strings (and NULLs); const trips MWCC in PTR()
STRUCTS_RE = '|'.join(t for t in LAYOUTS if t.isidentifier())  # the struct names, for patterns
SIZES = {'hex': 4, 's8': 1, 'u8': 1, 's16': 2, 'u16': 2, 's32': 4, 'u32': 4, 'x32': 4, 'f32': 4, 'enum': 4, 'ptr': 4,
         'str': 4, 'vec': 12}
def _align(fields):
    def one(kind, extra):
        k = extra[0] if kind == 'arr' else kind
        return 4 if k == 'vec' else min(SIZES[k], 4)
    return max(one(k, x[0] if x else None) for (_, k, _, *x) in fields)


ALIGN = {t: _align(fs) for t, (sz, fs) in LAYOUTS.items()}


def enum_names(name):
    """Values of a C enum in include/, as {value: NAME}; first name wins."""
    out = {}
    for path, _, files in os.walk('include'):
        for f in files:
            s = open(os.path.join(path, f), errors='replace').read()
            m = re.search(r'typedef enum %s \{(.*?)\} %s;' % (name, name), s, re.S)
            if not m:
                continue
            v = -1
            for line in m.group(1).split('\n'):
                line = re.sub(r'//.*', '', line).strip().rstrip(',')
                if not line or line.startswith('#'):
                    continue
                mm = re.match(r'(\w+)\s*(?:=\s*(-?(?:0x[0-9A-Fa-f]+|\d+)))?$', line)
                if not mm:
                    return {}
                v = int(mm.group(2), 0) if mm.group(2) else v + 1
                out.setdefault(v, mm.group(1))
            return out
    return out


# ---------------------------------------------------------------- original objects
def asm_path(cfile):
    cfile = os.path.relpath(os.path.abspath(cfile))
    m = re.match(r'rels/common/src/(v\d+)/(.+)\.c$', cfile)
    if m:  # a variant: its asm is the first module configure.py builds from it
        mods = re.search(r'modules=\["(\w+)"\][^)]*source="%s/%s\.c"' % (m.group(1), re.escape(m.group(2))), open('configure.py').read())
        return f'build/G8MJ01/{mods.group(1)}/asm/{m.group(2)}.s' if mods else None
    m = re.match(r'rels/common/src/(.+)\.c$', cfile)
    if m:
        import glob
        found = sorted(glob.glob(f'build/G8MJ01/*/asm/{m.group(1)}.s'))
        return found[0] if found else None
    m = re.match(r'rels/(\w+)/src/(.+)\.c$', cfile)
    if m:
        return f'build/G8MJ01/{m.group(1)}/asm/{m.group(2)}.s'
    m = re.match(r'src/(.+)\.c$', cfile)
    return f'build/G8MJ01/asm/{m.group(1)}.s' if m else None


def is_string_object(o):
    """The object is its string: the bytes up to the NUL, then only padding."""
    d = o['data']
    if d is None or o['rel'] or b'\0' not in d:
        return False
    n = d.index(b'\0')
    return not any(d[n:]) and o['size'] - n <= 4


def cstr(s):
    b = s.encode('latin1', 'backslashreplace').decode('unicode_escape').encode('latin1').split(b'\0')[0]
    t = b.decode('shift_jis')
    return '"' + t.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'


def load_objects(path):
    """{name: dict(size, data bytearray, rel {offset: (symbol, addend)}, sec, string)}"""
    objs, labels, pending = {}, {}, []
    sec = None
    text = open(path, errors='replace').read()
    for m in re.finditer(r'^(\.(?:text|data|rodata|bss|sdata|sdata2|ctors|dtors)\b[^\n]*)$|'
                         r'^# \.\w+:0x[0-9A-F]+ \| (0x[0-9A-F]+) \| size: (0x[0-9A-F]+)\n\.obj (\S+?),? \w+\n(.*?)^\.endobj ',
                         text, re.M | re.S):
        if m.group(1):
            sec = m.group(1).split()[0]
            continue
        size, name, body = int(m.group(3), 16), m.group(4), m.group(5)
        data, rel, string = bytearray(), {}, None
        for line in body.split('\n'):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if re.fullmatch(r'\.L_\w+:', line):
                labels[line[:-1]] = (name, len(data))
                continue
            d, _, arg = line.partition(' ')
            if d == '.rel':
                sym, label = [x.strip() for x in arg.split(',')]
                pending.append((rel, len(data), sym, label))
                rel[len(data)] = (sym, 0)
                data += b'\0\0\0\0'
                continue
            if d == '.4byte':
                mm = re.fullmatch(r'0x([0-9A-Fa-f]+)', arg)
                if mm:
                    data += int(mm.group(1), 16).to_bytes(4, 'big')
                else:
                    mm = re.fullmatch(r'"?([^+"]+)"?(?:\+0x([0-9A-Fa-f]+))?', arg)
                    rel[len(data)] = (mm.group(1), int(mm.group(2) or '0', 16))
                    data += b'\0\0\0\0'
            elif d == '.float':
                data += struct.pack('>f', float(arg))
            elif d == '.double':
                data += struct.pack('>d', float(arg))
            elif d == '.skip':
                data += bytes(int(arg, 0))
            elif d == '.byte':
                data += bytes(int(x, 0) & 0xFF for x in arg.split(','))
            elif d == '.string':
                s = re.fullmatch(r'"((?:[^"\\]|\\.)*)"', arg).group(1)
                string = s if string is None else string
                data += s.encode('latin1', 'backslashreplace').decode('unicode_escape').encode('latin1') + b'\0'
            else:
                data = None
                break
        if data is not None:
            data += bytes(max(0, size - len(data)))
            if string is None and sec == '.rodata' and not rel and b'\0' in data[:size]:
                n = data.index(b'\0')
                if n and all(0x20 <= c < 0x7F for c in data[:n]) and not any(data[n:size]):
                    string = data[:n].decode('ascii')  # dtk sometimes calls a string a vec3/float
                elif n == 0 and size == 4 and not any(data):
                    string = ''  # an empty string, which dtk names zero_*
        objs[name] = dict(size=size, data=data, rel=rel, sec=sec, string=string)
    for rel, off, sym, label in pending:  # `.rel SYM, .L_x`: a pointer to a label inside SYM
        owner, loff = labels.get(label, (None, 0))
        rel[off] = (sym, loff) if owner == sym else (owner or sym, loff)
    return objs


# ---------------------------------------------------------------- the C file
DECL = re.compile(r'^((?:static )?(?:const )?)([A-Za-z_]\w*) (\*?)([A-Za-z_]\w*)((?:\[\w*\])?)( = \{|;)', re.M)


def declarations(src):
    """{name: (type, is_array)} for file-scope data declarations and definitions."""
    out = {}
    for m in DECL.finditer(src):
        if m.group(3) or m.group(2) in ('return', 'typedef', 'extern'):
            continue
        out[m.group(4)] = (m.group(2), bool(m.group(5)))
    for m in re.finditer(r'^extern (?:const )?([A-Za-z_]\w*) ([A-Za-z_]\w*)((?:\[\w*\])?);', src, re.M):
        out.setdefault(m.group(2), (m.group(1), bool(m.group(3))))
    return out


def fmt_float(b):
    """The shortest decimal that encodes to these four bytes, e.g. 1.0f, -0.555f, 3.1415927f."""
    v = struct.unpack('>f', b)[0]
    if v != v or v in (float('inf'), float('-inf')):
        raise ValueError(f'float {b.hex()} has no decimal form')
    cands = ['%.*f' % (d, v) for d in range(0, 10)] if 1e-4 <= abs(v) < 1e8 or v == 0 else []
    cands += ['%.*g' % (p, v) for p in range(1, 10)]
    for s in cands:
        if struct.pack('>f', float(s)) == b:
            break
    if 'e' not in s and '.' not in s:
        s += '.0'
    return s + 'f'


_HEADER_DECLS = None


def header_declarations():
    """extern declarations in every header, {name: (type, is_array)}."""
    global _HEADER_DECLS
    if _HEADER_DECLS is None:
        _HEADER_DECLS = {}
        for root in ('include', 'rels'):
            for path, _, files in os.walk(root):
                for f in files:
                    if f.endswith('.h'):
                        _HEADER_DECLS.update(declarations(open(os.path.join(path, f), errors='replace').read()))
    return _HEADER_DECLS


class Renderer:
    def __init__(self, cfile, objs, types, alias=None):
        self.cfile, self.objs, self.types = cfile, objs, types  # types: {symbol: type being written}
        self.alias = alias or {}  # original symbol -> the name the C file defines it under (UNIT_DATA_SYM)
        self.decls = dict(header_declarations())
        self.decls.update(declarations(open(cfile).read()))
        self.enums = {}

    def ref(self, sym, add, pointee):
        sym = self.alias.get(sym, sym)
        o = self.objs.get(sym)
        if o and o['string'] is not None and o['sec'] in ('.rodata', '.data') and add == 0 and is_string_object(o):
            return cstr(o['string'])
        typ, arr = self.decls.get(sym, (None, True))
        typ = self.types.get(sym, typ)
        base = sym if arr else '&' + sym
        cast = f'(struct {pointee}*)' if pointee in LAYOUTS else f'({pointee}*)'
        if add:
            base = f'(void*)((u8*){base} + 0x{add:X})'
            return cast + base if pointee else base
        if pointee and typ != pointee and typ != 'void':
            return cast + base
        return base

    def value(self, kind, data, rel, off, extra, fname):
        if kind in ('ptr', 'str'):
            if off in rel:
                sym, add = rel[off]
                return self.ref(sym, add, extra if kind == 'ptr' else None)
            v = int.from_bytes(data[off:off + 4], 'big')
            if v:
                raise ValueError(f'{fname}: {v:#x} is not a pointer')
            return 'NULL'
        assert off not in rel, f'pointer where {fname} expects a {kind}'
        if kind == 'vec':
            return '{ ' + ', '.join(fmt_float(data[off + i:off + i + 4]) for i in (0, 4, 8)) + ' }'
        if kind == 'f32':
            return fmt_float(data[off:off + 4])
        if kind == 'arr':
            k, n = extra
            return '{ ' + ', '.join(self.value(k, data, rel, off + i * SIZES[k], None, fname) for i in range(n)) + ' }'
        sz = SIZES[kind]
        raw = int.from_bytes(data[off:off + sz], 'big')
        if kind == 'x32':
            return '0' if raw == 0 else f'0x{raw:08X}'
        if kind == 'hex':
            return f'0x{raw:02X}' if raw < 0x100 else f'0x{raw:X}'
        if kind == 'enum':
            if extra not in self.enums:
                self.enums[extra] = enum_names(extra)
            return self.enums[extra].get(raw, str(raw))
        if kind[0] == 's' and raw >= 1 << (sz * 8 - 1):
            raw -= 1 << (sz * 8)
        return str(raw)

    def element(self, typ, data, rel, base):
        size, fields = LAYOUTS[typ]
        covered = set()
        vals = []
        for off, kind, name, *extra in fields:
            n = SIZES[kind] if kind != 'arr' else SIZES[extra[0][0]] * extra[0][1]
            covered.update(range(base + off, base + off + n))
            vals.append((self.value(kind, data, rel, base + off, extra[0] if extra else None, name), name))
        for i in range(base, base + size):
            if i not in covered and data[i]:
                raise ValueError(f'{typ}: nonzero padding byte at {i - base:#x}')
        return vals

    def render(self, sym, typ, qual, is_array):
        o = self.objs[sym]
        size, _ = LAYOUTS[typ]
        stride = (size + ALIGN[typ] - 1) // ALIGN[typ] * ALIGN[typ]
        n = o['size'] // stride
        if n == 0 or o['size'] - n * stride >= ALIGN[typ] and o['size'] - n * stride >= 4:
            raise ValueError(f'{sym}: {o["size"]:#x} bytes is not a whole number of {typ} ({stride:#x})')
        for i in range(n * stride, o['size']):
            if o['data'][i] or i in o['rel']:
                raise ValueError(f'{sym}: data after the last whole {typ}')
        elems = [self.element(typ, o['data'], o['rel'], i * stride) for i in range(n)]
        many = len(LAYOUTS[typ][1]) > 8 and typ not in ONE_LINE
        def one(vals, ind):
            if len(vals) == 1 and not vals[0][1]:
                return vals[0][0]
            if typ in ONE_LINE:
                vs = [v for v, _ in vals]
                while vs and vs[-1] in ('0', 'NULL', '0x00'):
                    vs.pop()
                return '{' + ', '.join(vs or ['NULL']) + '}'
            if not many:
                return '{ ' + ', '.join(v for v, _ in vals) + ' }'
            w = max(len(v) for v, _ in vals) + 1
            inner = '\n'.join(f'{ind}    {(v + ","):<{w}} // {name}' for v, name in vals)
            return '{\n' + inner + '\n' + ind + '}'
        if is_array and len(LAYOUTS[typ][1]) == 1 and not LAYOUTS[typ][1][0][2]:
            lines, cur = [], ''
            for e in elems:  # plain values (a string list): as many to a line as fit in 100 columns
                v = one(e, '') + ','
                if cur and len(cur) + 1 + len(v) > 96:
                    lines.append(cur)
                    cur = ''
                cur = (cur + ' ' + v) if cur else v
            lines.append(cur)
            body = '{\n' + ''.join('    ' + l + '\n' for l in lines) + '}'
            head = f'{qual}{typ} {sym}[{n}] = '
        elif is_array:
            body = '{\n' + ''.join('    ' + one(e, '    ') + ',\n' for e in elems) + '}'
            head = f'{qual}{typ} {sym}[{n}] = '
        else:
            assert n == 1, f'{sym}: {n} elements but declared as a single object'
            body = one(elems[0], '')
            head = f'{qual}{typ} {sym} = '
        return head + body + ';'


# single objects are written as scalars, lists as arrays (even of one), whatever the old declaration was
ONE_LINE = {'BeroEntry'}  # rows written on one line, trailing zero fields left off (the style map files use)
RAW_ONLY = {'BeroEntry', 'char*'}  # rendered only where the table is still a raw array
LISTS = {'BattleSetupData', 'BeroEntry', 'ItemDropData', 'BattleUnitKindPart', 'BattleDispPose', 'BattleDataEntry', 'BattleUnitSetup', 'NpcBattleSetupEntry',
         'BattleStageObjectData'}
RESHAPED = {}  # (file, symbol): (now_array, is_static), for fix_refs
RETYPED = {}  # (file, symbol): (old element size, new stride, is_static), for fix_index
HEADERS = {'BattleSetupData': 'driver/npcdrv.h', 'BeroEntry': 'evt/evt_bero.h', 'ItemDropData': 'battle/battle_unit.h', 'BattleDispPose': 'battle/battle_disp.h', 'BattleWeapon': 'battle/battle_database_common.h',
           'BattleGroupSetup': 'driver/npcdrv.h', 'NpcBattleSetupEntry': 'driver/npcdrv.h',
           'BattleStageData': 'battle/battle_stage.h', 'BattleStageObjectData': 'battle/battle_stage_object.h'}


def add_includes(src, types):
    """Add the header of each rendered type the file doesn't include directly, after its last #include."""
    need = sorted({HEADERS.get(t, 'battle/battle_unit.h') for t in types} -
                  set(re.findall(r'^#include "([^"]+)"', src, re.M)))
    if not need:
        return src
    last = list(re.finditer(r'^#include .*\n', src, re.M))
    at = last[-1].end() if last else 0
    return src[:at] + ''.join(f'#include "{h}"\n' for h in need) + src[at:]


def update_externs(typed, roots=('include', 'src', 'rels')):
    """typed: {symbol: (type, count or None)}. Rewrite `extern T sym[...]`/`extern T sym;` everywhere to match,
    adding the type's header where it's missing. Returns the files changed."""
    changed = []
    pat = re.compile(r'^extern (?:const )?[A-Za-z_]\w* (\w+)(\[\w*\])?;', re.M)
    for root in roots:
        for path, _, files in os.walk(root):
            for f in files:
                if not f.endswith(('.c', '.h')):
                    continue
                p = os.path.join(path, f)
                src = open(p, errors='replace').read()
                if 'extern' not in src:
                    continue
                used = set()
                def fix(m):
                    if m.group(1) not in typed:
                        return m.group(0)
                    typ, n = typed[m.group(1)]
                    used.add(typ)
                    return f'extern {typ} {m.group(1)}' + (f'[{n}];' if n is not None else ';')
                new = pat.sub(fix, src)
                if new != src:
                    new = add_includes(new, used)
                    open(p, 'w').write(new)
                    changed.append(p)
    return changed


def fix_index(src, sym, old_size, stride):
    """Uses of SYM counted in the old raw element size, rewritten for the new type: `&sym[N]` and `sym + N` /
    `sym - N` (N decimal or hex) become `&sym[k]` where N lands on an element, otherwise a byte offset. A plain use
    (`sym`, `&sym`) stays. Every use must be a whole operand: after it (past any closing parentheses) only `)`, `,`,
    `;` or `}` may follow, and no operator may precede it. Anything else (`&(sym)[N]`, `sym + N * 2`, `sym[N]` as a
    value, a variable index) raises: its meaning changes with the type. After a cast, `(u8*)sym + 0x8` is a byte
    offset and is left alone; comments, strings, declarations and struct members of the same name are skipped."""
    num = r'(0[xX][0-9A-Fa-f]+|\d+)'
    code = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:[^"\\\n]|\\.)*"|(&\s*)?\b' + re.escape(sym) + r'\b(\s*\[\s*' + num +
                      r'\s*\]|\s*([-+])\s*' + num + r'\b)?', re.S)
    decl = re.compile(r'^\s*(?:extern |static |const |struct )*(?!return\b|sizeof\b|case\b)[A-Za-z_]\w*\s*\*?\s*' +
                      re.escape(sym) + r'\s*(?:\[|=|;)')
    out, pos = [], 0
    for m in code.finditer(src):
        if not m.group(0).lstrip('&').lstrip().startswith(sym):
            continue  # a comment or string
        line_start = src.rfind('\n', 0, m.start()) + 1
        line_end = src.find('\n', m.end())
        if decl.match(src[line_start:line_end if line_end >= 0 else len(src)]):
            continue  # its declaration or definition
        before = src[line_start:m.start()].rstrip()
        near = src[max(line_start, m.start() - 20):m.end() + 12]
        if re.search(r'(\.|->)$', before):
            continue  # a struct member with the same name
        if re.search(r'\(\s*(?:const\s+)?[A-Za-z_]\w*\s*\*+\s*\)$', before):
            continue  # after a cast, e.g. (u8*)sym + 0x8: a byte offset, whatever the element type
        # a whole operand: no operator before it, only `)` `,` `;` `}` after it (past closing parentheses)
        b, wraps = before, 0
        while b.endswith('('):
            b, wraps = b[:-1].rstrip(), wraps + 1
        if re.search(r'[A-Za-z_]\w*$', b) and wraps:
            wraps -= 1  # the last `(` opens a call (PTR(sym), f(sym)), not a wrap around sym
        rest = src[m.end():]
        k = 0
        while True:
            k += len(rest[k:]) - len(rest[k:].lstrip())
            if wraps and rest[k:k + 1] == ')':
                k, wraps = k + 1, wraps - 1
                continue
            break
        if re.search(r'[-+*/%&|^]$', b.rstrip('&').rstrip()) and not b.endswith('->') or \
                not re.match(r'[),;}]|$', rest[k:k + 1]):
            raise ValueError(f'{sym}: a use fix_index can\'t convert near {near!r}')
        amp, idx, sign, add = m.group(1), m.group(3), m.group(4), m.group(5)
        if m.group(2) is None:
            continue  # a plain use: sym or &sym
        if idx is not None and not amp:
            raise ValueError(f'{sym}[{idx}] read as a value near {near!r}')
        if amp and idx is None:
            raise ValueError(f'{sym}: `&{sym} +/- N` near {near!r}')
        n = int(idx if idx is not None else add, 0) * (-1 if sign == '-' else 1)
        off = n * old_size
        rep = f'&{sym}[{off // stride}]' if off % stride == 0 and off >= 0 else f'(void*)((u8*){sym} + {off:#x})'
        out.append(src[pos:m.start()])
        out.append(rep)
        pos = m.end()
    out.append(src[pos:])
    return ''.join(out)


def fix_refs(src, sym, now_array):
    """Adjust uses of SYM whose declaration changed between array and scalar: `x` <-> `&x`. Declarations,
    comments, strings and sizeof are left alone; arithmetic, indexing and member access on SYM raise, since
    their meaning changes with the shape."""
    decl = re.compile(r'^\s*(?:extern |static |const |struct )*[A-Za-z_]\w*\s*\*?\s*' + re.escape(sym) + r'\s*(\[\w*\])?\s*[;=,)]')
    out, pos = [], 0
    code = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:[^"\\\n]|\\.)*"|\'(?:[^\'\\\n]|\\.)*\'|(&\s*)?\b' + re.escape(sym) + r'\b', re.S)
    for m in code.finditer(src):
        if not m.group(0).lstrip('&').lstrip().startswith(sym):
            continue  # a comment, string or char literal
        line_start = src.rfind('\n', 0, m.start()) + 1
        line_end = src.find('\n', m.end())
        if decl.match(src[line_start:line_end if line_end >= 0 else None]):
            continue
        before = src[max(0, m.start() - 8):m.start()]
        after = src[m.end():m.end() + 3]
        if re.search(r'(sizeof\s*\(?\s*|[\w.]|->)$', before):
            continue
        if re.match(r'\s*(\[|->|\.|[-+](?![>=]))', after) or re.search(r'[-+]\s*$', before.rstrip('&').rstrip()):
            raise ValueError(f'{sym}: arithmetic, index or member use near {src[line_start:line_end]!r}')
        out.append(src[pos:m.start()])
        if now_array and m.group(1):
            out.append(sym)
        elif not now_array and not m.group(1):
            out.append('&' + sym)
        else:
            out.append(m.group(0))
        pos = m.end()
    out.append(src[pos:])
    return ''.join(out)


def find_definition(src, sym):
    """(start, end, qualifiers, is_array) of SYM's definition, with a `// TODO: type` line above it and the
    newline after its `;`; braces matched, strings and comments skipped."""
    m = re.search(r'^(// TODO: type\n)?((?:static )?(?:const )?)[A-Za-z_]\w* ' + re.escape(sym) +
                  r'(\[\w*\])? = \{', src, re.M)
    if not m:
        return None
    i, depth = m.end() - 1, 0
    while i < len(src):
        c = src[i]
        if c == '"':
            i += 1
            while src[i] != '"':
                i += 2 if src[i] == '\\' else 1
        elif src.startswith('//', i):
            i = src.index('\n', i) - 1
        elif src.startswith('/*', i):
            i = src.index('*/', i) + 1
        elif c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                break
        i += 1
    end = src.index(';', i) + 1
    if src[end:end + 1] == '\n':
        end += 1
    return m.start(), end, m.group(2), bool(m.group(3))


def rewrite(cfile, mapping, objs=None, alias=None):
    """Replace each SYMBOL's definition (and forward declarations) in cfile with its TYPE rendering."""
    objs = objs or load_objects(asm_path(cfile))
    src = open(cfile).read()
    mapping = dict(mapping)
    while True:  # drop what can't render first, so nothing that points at it assumes its new type
        r = Renderer(cfile, objs, mapping, alias)
        bad = []
        for sym, typ in mapping.items():
            dm = find_definition(src, sym)
            try:
                if dm and sym in objs and objs[sym]['data'] is not None:
                    r.render(sym, typ, dm[2], True)
            except (ValueError, AssertionError):
                bad.append(sym)
        if not bad:
            break
        for sym in bad:
            del mapping[sym]
    r = Renderer(cfile, objs, mapping, alias)
    done, failed = [], []
    for sym, typ in mapping.items():
        dm = find_definition(src, sym)
        if not dm or sym not in objs or objs[sym]['data'] is None:
            failed.append((sym, 'definition or original object not found'))
            continue
        try:
            size = LAYOUTS[typ][0]
            stride = (size + ALIGN[typ] - 1) // ALIGN[typ] * ALIGN[typ]
            want_array = objs[sym]['size'] // stride > 1 or (typ in LISTS and dm[3])
            text = r.render(sym, typ, dm[2], want_array)
            if want_array != dm[3]:
                RESHAPED[(cfile, sym)] = (want_array, dm[2].startswith('static'))
            old = re.match(r'(?:// TODO: type\n)?(?:static |const )*(\w+) ', src[dm[0]:])
            old_size = {'u32': 4, 's32': 4, 'u16': 2, 's16': 2, 'u8': 1, 's8': 1}.get(old.group(1) if old else '')
            if old_size:
                RETYPED[(cfile, sym)] = (old_size, stride, dm[2].startswith('static'))
        except (ValueError, AssertionError) as e:
            failed.append((sym, str(e)))
            continue
        start = dm[0]
        lines = src[:start].split('\n')[:-1]
        k = len(lines)
        while k and lines[k - 1].startswith('//'):
            k -= 1
        block = lines[k:]
        if any(STALE.search(l) for l in block):
            keep = [l for l in block if not STALE.search(l)]
            head = '\n'.join(lines[:k] + keep)
            src = head + ('\n' if head else '') + text + '\n' + src[dm[1]:]
        else:
            src = src[:start] + text + '\n' + src[dm[1]:]
        n = re.search(r'\[(\d+)\] = ', text)
        src = re.sub(r'^((?:static )?(?:const )?)[A-Za-z_]\w* ' + re.escape(sym) + r'(\[\w*\])?;$',
                     lambda mm: f'{mm.group(1)}{typ} {sym}{"[" + n.group(1) + "]" if n else ""};', src, flags=re.M)
        done.append(sym)
    if done:
        src = add_includes(src, {mapping[d] for d in done if mapping[d].isidentifier()})
    open(cfile, 'w').write(src)
    return done, failed


STALE = re.compile(r'opaque|field_0x24|field_4\[|header (struct|has|hides|names|only|can)|byte-identical|verbatim|'
                   r'kept as-is|bytes kept|byte-for-byte|5-byte|0x16 bytes|no existing type|casts? point|stays? raw|'
                   r'left raw|pad(ded)? to 8|u8 field|TODO: type', re.I)
ROOTS = [(re.compile(r'^unit_\w+$|^UNIT_DATA_SYM$'), 'BattleUnitKind'), (re.compile(r'^btlgrp_\w+$'), 'BattleGroupSetup'),
         (re.compile(r'^btlparty_\w+$'), 'BattleUnitSetup'), (re.compile(r'^btlgrpsel\w*$'), 'NpcBattleSetupEntry'),
         (re.compile(r'weapon', re.I), 'BattleWeapon'), (re.compile(r'_btlstage_data$'), 'BattleStageData'),
         # tables scripts hand to btlevtcmd_* (turn, guard and form changes), by their names in the original
         (re.compile(r'(^|_)defence(_[a-z]+)*_attr(_[a-z]+)*$'), 'BattleUnitDefenseAttr'),
         (re.compile(r'(^|_)defence(_[a-z]+)*$'), 'BattleUnitDefense'),
         (re.compile(r'(^|_)regist(_[a-z]+)*$'), 'BattleVulnerableStatus'),
         (re.compile(r'(^|_)pose_table(_\w+)?$'), 'BattleDispPose'), (re.compile(r'(^|_)data_table(_\w+)?$'), 'BattleDataEntry'),
         (re.compile(r'(^|_)parts(_[a-z]+)*$'), 'BattleUnitKindPart'), (re.compile(r'^battle_item_\w+$'), 'ItemDropData'),
         (re.compile(r'(^|_)bero(_\w+)?$'), 'BeroEntry'), (re.compile(r'^btlsetup_\w+_tbl$'), 'BattleSetupData'),
         (re.compile(r'^entry_\w+$'), 'BattleUnitSetup')]


def macro_aliases(cfile):
    """A shared file (rels/common) names its own and other units through macros configure.py sets per module
    (-DUNIT_DATA_SYM=unit_x_9_data_..., -DUNIT_REF_X=...). {original symbol: macro} for the module whose
    disassembly asm_path picked."""
    m = re.match(r'rels/common/src/(.+\.c)$', cfile)
    ap = asm_path(cfile) if m else None
    if not ap:
        return {}
    mod = ap.split('/')[2]
    for line in open('configure.py'):
        if f'"{m.group(1)}"' in line and f'modules=["{mod}"]' in line:
            return {v: k for k, v in re.findall(r'-D(\w+)=(\w+)', line)}
    return {}


def _blank(src):
    """src with comments and string/char literals blanked out (same length, so offsets stay valid)."""
    return re.sub(r'//[^\n]*|/\*.*?\*/|"(?:[^"\\\n]|\\.)*"|\'(?:[^\'\\\n]|\\.)*\'',
                  lambda m: re.sub(r'[^\n]', ' ', m.group(0)), src, flags=re.S)


def function_bodies(src):
    """The text of every C function body in src: each top-level `{` right after a `)` (a definition, USER_FUNC
    included, however its signature is laid out), to its matching `}`. Initializers (`= {`) and scripts aren't."""
    b = _blank(src)
    out, depth, i = [], 0, 0
    while i < len(b):
        c = b[i]
        if c == '{':
            if depth == 0 and b[:i].rstrip().endswith(')'):
                j, d = i, 0
                while j < len(b):
                    d += (b[j] == '{') - (b[j] == '}')
                    if d == 0:
                        break
                    j += 1
                out.append(b[i + 1:j])
                i = j + 1
                continue
            depth += 1
        elif c == '}':
            depth -= 1
        i += 1
    return '\n'.join(out)


_CODE_INDEX = None


def code_everywhere():
    """Every function body in the tree's C files, for globals another file's code may use."""
    global _CODE_INDEX
    if _CODE_INDEX is None:
        _CODE_INDEX = '\n'.join(function_bodies(open(f, errors='replace').read()) for f in c_files())
    return _CODE_INDEX


def auto_types(cfile, objs):
    """{symbol: type} for the battle graph in cfile: roots by name and size, then every typed pointer followed
    into objects this file defines. A symbol reached as two types, or not a whole number of its type, is left out."""
    src = open(cfile).read()
    defined = {m.group(1) for m in re.finditer(r'^(?:static |const )*[A-Za-z_]\w* (\w+)(?:\[\w*\])? = \{', src, re.M)}
    raw = {m.group(1) for m in re.finditer(r'^(?:static |const )*[su]32 (\w+)(?:\[\w*\])? = \{', src, re.M)}
    # a raw table that C code uses stays raw: retyping it would change what indexing it means
    code = function_bodies(src)
    statics = {m.group(1) for m in re.finditer(r'^static (?:const )?[su]32 (\w+)', src, re.M)}
    raw_in_code = {n for n in raw if re.search(r'\b' + re.escape(n) + r'\b', code) or
                   (n not in statics and re.search(r'\b' + re.escape(n) + r'\b', code_everywhere()))}
    defined -= raw_in_code
    raw -= raw_in_code
    alias = macro_aliases(cfile)
    if 'UNIT_DATA_SYM' in defined and not any(v == 'UNIT_DATA_SYM' and k in objs for k, v in alias.items()):
        units = [n for n, o in objs.items() if n.startswith('unit_') and o['size'] == 0xC4]
        if len(units) == 1:
            alias[units[0]] = 'UNIT_DATA_SYM'
    def fits(sym, typ):
        o = objs.get(sym)
        if o is not None and o['sec'] in ('.bss', '.sbss'):
            return False  # uninitialized: nothing to render, and an initializer would move it to .data
        size = LAYOUTS[typ][0]
        stride = (size + ALIGN[typ] - 1) // ALIGN[typ] * ALIGN[typ]
        return o is not None and o['data'] is not None and o['size'] >= size and o['size'] % stride < 4
    types, bad, todo = {}, set(), []
    for sym in objs:
        c = alias.get(sym, sym)
        if c not in defined:
            continue
        for pat, typ in ROOTS:
            if typ in RAW_ONLY and c not in raw:
                continue
            if pat.search(c) and fits(sym, typ) and (typ not in ('BattleWeapon', 'BattleStageData') or
                                                     objs[sym]['size'] == LAYOUTS[typ][0]):
                todo.append((sym, typ))
                break
    for sym, o in objs.items():
        c = alias.get(sym, sym)
        if c in raw and o['data'] is not None and o['size'] % 4 == 0 and o['sec'] == '.data' and o['rel'] and \
                all((i in o['rel'] and o['rel'][i][1] == 0 and objs.get(o['rel'][i][0], {}).get('string') is not None) or
                    (i not in o['rel'] and not any(o['data'][i:i + 4])) for i in range(0, o['size'], 4)):
            todo.append((sym, 'char*'))
    while todo:
        sym, typ = todo.pop()
        if sym in bad:
            continue
        if sym in types:
            if types[sym] != typ:
                bad.add(sym)
            continue
        types[sym] = typ
        o = objs[sym]
        size = LAYOUTS[typ][0]
        stride = (size + ALIGN[typ] - 1) // ALIGN[typ] * ALIGN[typ]
        for base in range(0, o['size'] - size + 1, stride):
            for off, kind, name, *x in LAYOUTS[typ][1]:
                if kind == 'ptr' and x[0] in LAYOUTS and base + off in o['rel']:
                    t, add = o['rel'][base + off]
                    if add == 0 and alias.get(t, t) in defined and fits(t, x[0]):
                        todo.append((t, x[0]))
    return {alias.get(k, k): v for k, v in types.items() if k not in bad}, alias


def c_files():
    import glob
    return sorted(glob.glob('rels/**/*.c', recursive=True)) + sorted(glob.glob('src/**/*.c', recursive=True))


# files the renderer must not touch: hand-built before the generators (strings as named char arrays, hex scripts)
SKIP = {'rels/pik/src/unit/unit_purple_teresa.c'}


def run_all(skip=()):
    """Type the battle graph in every file with raw tables, then make the rest of the tree agree: references to
    tables that became scalars or arrays, extern declarations, and casts the new declarations made unneeded."""
    import collections, glob
    counts = collections.Counter()
    snap, exclude = {}, collections.defaultdict(set)

    def render_file(f):
        ap = asm_path(f)
        if not ap or not os.path.exists(ap):
            return
        try:
            objs = load_objects(ap)
            mapping, alias = auto_types(f, objs)
            mapping = {k: v for k, v in mapping.items() if k not in exclude[f]}
            if not mapping:
                return
            for a, c in alias.items():
                if a in objs:
                    objs[c] = objs[a]
            snap.setdefault(f, open(f).read())
            done, failed = rewrite(f, mapping, objs, alias)
        except Exception as e:  # one file's problem shouldn't stop the rest
            print('FAILED', f, repr(e))
            return
        counts.update(mapping[d] for d in done)
        for sym, why in failed:
            print('FAILED', f, sym, why)

    for f in c_files():
        if f in skip or f in SKIP or (f.startswith('src/') and '// TODO: type' not in open(f).read() and
                         not re.search(r'^(?:static |const )*(' + STRUCTS_RE + r') \w+', open(f).read(), re.M)):
            continue
        render_file(f)
    # shared units: UNIT_DATA_SYM is a different global per module (configure.py -D flags)
    cfg = open('configure.py').read()
    for f in glob.glob('rels/common/src/**/*.c', recursive=True):
        if re.search(r'^BattleUnitKind UNIT_DATA_SYM = ', open(f).read(), re.M):
            rel = f[len('rels/common/src/'):]
            for name in re.findall(r'"' + re.escape(rel) + r'", modules=\[[^]]*\], extra_cflags=\["-DUNIT_DATA_SYM=(\w+)"\]', cfg):
                RESHAPED[('<shared>', name)] = (False, False)
    everything = glob.glob('rels/**/*.[ch]', recursive=True) + glob.glob('src/**/*.[ch]', recursive=True) + \
        glob.glob('include/**/*.h', recursive=True)
    users = lambda f, sym, static: [f] if static else [p for p in everything if sym in open(p, errors='replace').read()]
    # Check every conversion the retyped tables need before writing any. A table whose uses can't all be converted
    # stays raw: its file is restored and rendered again without it, until everything converts.
    for _ in range(4):
        bad = set()
        for (f, sym), (old_size, stride, static) in RETYPED.items():
            for p in users(f, sym, static):
                try:
                    fix_index(open(p, errors='replace').read(), sym, old_size, stride)
                except ValueError as e:
                    print('LEFT RAW', f, sym, '-', p, e)
                    bad.add((f, sym))
        for (f, sym), (arr, static) in RESHAPED.items():
            if f == '<shared>':
                continue
            for p in users(f, sym, static):
                try:
                    fix_refs(open(p, errors='replace').read(), sym, arr)
                except ValueError as e:
                    print('LEFT RAW', f, sym, '-', p, e)
                    bad.add((f, sym))
        if not bad:
            break
        for f, sym in bad:
            exclude[f].add(sym)
        for f in {f for f, _ in bad}:
            open(f, 'w').write(snap.pop(f))
            for k in [k for k in RETYPED if k[0] == f]:
                del RETYPED[k]
            for k in [k for k in RESHAPED if k[0] == f]:
                del RESHAPED[k]
            render_file(f)
    for (f, sym), (old_size, stride, static) in RETYPED.items():
        for p in users(f, sym, static):
            text = open(p, errors='replace').read()
            new = fix_index(text, sym, old_size, stride)
            if new != text:
                open(p, 'w').write(new)
    for (f, sym), (arr, static) in RESHAPED.items():
        for p in users(f, sym, static):
            text = open(p, errors='replace').read()
            try:
                new = fix_refs(text, sym, arr)
            except ValueError as e:
                print('FIX BY HAND', p, e)
                continue
            if new != text:
                open(p, 'w').write(new)
    typed, seen = {}, collections.defaultdict(set)
    for f in c_files():
        for m in re.finditer(r'^(const )?(' + STRUCTS_RE + r') (\w+)(?:\[(\d+)\])? = ', open(f).read(), re.M):
            v = (m.group(2), int(m.group(4)) if m.group(4) else None)
            seen[m.group(3)].add(v)
            typed[m.group(3)] = v
    for k in [k for k, v in seen.items() if len(v) > 1]:
        typed.pop(k)
    for (f, name), _ in RESHAPED.items():
        if f == '<shared>':
            typed[name] = ('BattleUnitKind', None)
    update_externs(typed)
    global _HEADER_DECLS
    _HEADER_DECLS = None
    hd = header_declarations()
    defined_anywhere = set()
    for f in c_files():
        defined_anywhere.update(re.findall(r'^(?:static |const )*[A-Za-z_]\w* (\w+)(?:\[\w*\])? = ', open(f).read(), re.M))
    for f in c_files():
        text = open(f).read()
        if '(struct ' not in text:
            continue
        decls = dict(hd)
        decls.update(declarations(text))
        def uncast(m):
            t, name = m.group(1), m.group(3)
            if name in decls and decls[name][0] == t:
                return name if decls[name][1] else '&' + name
            return m.group(0)
        text = re.sub(r'\(struct (\w+)\*\)(&?)(\w+)\b(?!\s*[+\[])', uncast, text)
        # a file's own extern of a raw array that every use casts to one struct: declare it as that struct
        uses = collections.defaultdict(set)
        for m in re.finditer(r'\(struct (\w+)\*\)(&?)(\w+)\b(?!\s*[+\[)])', text):
            uses[m.group(3)].add(m.group(1))
        for name, ts in uses.items():
            if len(ts) != 1:
                continue
            t = ts.pop()
            em = re.search(r'^extern (?:s32|u32) ' + re.escape(name) + r'\[\w*\];$', text, re.M)
            if not em or name in defined_anywhere:
                continue
            rest = re.sub(r'\(struct ' + t + r'\*\)&?' + re.escape(name) + r'\b', '', text[:em.start()] + text[em.end():])
            if re.search(r'\b' + re.escape(name) + r'\b', rest):
                continue
            arr = t in LISTS
            text = text[:em.start()] + f'extern {t} {name}' + ('[];' if arr else ';') + text[em.end():]
            text = re.sub(r'\(struct ' + t + r'\*\)&?' + re.escape(name) + r'\b', name if arr else '&' + name, text)
            text = add_includes(text, {t})
        open(f, 'w').write(text)
    for f in c_files():
        text = open(f).read()
        text = re.sub(r'^// TODO: type\n(?=(?:static |const )*(?![su]32 |u8 |s8 |u16 |s16 |char )[A-Za-z_]\w* \w+(?:\[\w*\])? = )', '',
                      text, flags=re.M)
        used = {t for t in LAYOUTS if t.isidentifier() and re.search(r'^(?:static |const )*' + t + r' \w+', text, re.M)}
        new = add_includes(text, used) if used else text
        if new != open(f).read():
            open(f, 'w').write(new)
    return counts


def check_layouts():
    """Compile offsetof/sizeof assertions for every LAYOUTS field with the project's compiler."""
    names = {'arr': None}
    lines = ['#include "battle/battle_unit.h"', '#include "battle/battle_disp.h"', '#include "battle/battle_stage.h"',
             '#include "battle/battle_stage_object.h"', '#include "battle/battle_database_common.h"',
             '#include "driver/npcdrv.h"', '#include "evt/evt_bero.h"', '#include <stddef.h>']
    for t, (size, fields) in LAYOUTS.items():
        if not t.isidentifier():
            continue
        lines.append(f'char chk_{t}_size[sizeof({t}) == {size:#x} || sizeof({t}) == {(size + 3) & ~3:#x} ? 1 : -1];')
        for off, kind, name, *x in fields:
            lines.append(f'char chk_{t}_{name}[offsetof({t}, {name}) == {off:#x} ? 1 : -1];')
    path = 'build/G8MJ01/layout_check.c'
    open(path, 'w').write('\n'.join(lines) + '\n')
    cmd = subprocess.run(['ninja', '-t', 'commands', 'build/G8MJ01/src/battle/battle_unit.o'], capture_output=True,
                         text=True).stdout.strip().split('\n')[-1]
    cmd = re.sub(r' -c \S+ -o \S+', f' -c {path} -o build/G8MJ01/layout_check.o', cmd.split(' && ')[0])
    cmd = cmd.replace(' -MMD', '')
    r = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    print('layouts match the headers' if r.returncode == 0 else r.stdout + r.stderr)
    return r.returncode == 0


if __name__ == '__main__':
    if sys.argv[1:] == ['--check-layouts']:
        sys.exit(0 if check_layouts() else 1)
    if sys.argv[1] == '--all':
        print(dict(run_all(skip=set(sys.argv[2:]))))
        RESHAPED.clear()
        RETYPED.clear()
        _HEADER_DECLS = None
        run_all(skip=set(sys.argv[2:]))  # again, so casts and `&` are rendered from the updated declarations
        sys.exit(0)
    cfile = os.path.relpath(os.path.abspath(sys.argv[1]))
    if sys.argv[2:] == ['--auto']:
        objs = load_objects(asm_path(cfile))
        mapping, alias = auto_types(cfile, objs)
        for a, c in alias.items():
            if a in objs:
                objs[c] = objs[a]
        done, failed = rewrite(cfile, mapping, objs, alias)
    else:
        mapping = dict(a.split('=') for a in sys.argv[2:])
        done, failed = rewrite(cfile, mapping)
    for s in done:
        print('typed', s, mapping[s])
    for s, why in failed:
        print('FAILED', s, why)
