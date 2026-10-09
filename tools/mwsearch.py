#!/usr/bin/env python3
"""Bounded, single-function MWCC source-shape search (no game sources are written).

    uv run --with pyelftools python tools/mwsearch.py --unit event/evt_shop \
        --func list_disp --minutes 15 --out /tmp/shop-search

The score is objdiff-cli's match percent for the function (build/tools/objdiff-cli
diff, target vs candidate object), so it agrees with objdiff: rows are aligned and
registers, operands and relocation targets all count. Siblings must not lose percent,
and each data section must stay byte-identical to the input's or become the target's
(up to the extracted section's trailing alignment zeros).
A match is not a proof of C equivalence or a linked checksum:
review every emitted patch, especially floating-point operand swaps.

Each generation expands the best-or-tied candidates (distinct generated code, code not yet
expanded first) plus up to --explore candidates that kept their own parent's score but changed
its code, taken round-robin across parents; --stall bounds consecutive non-improving steps on
one line of descent. This crosses plateaus where the greedy beam settles on a local optimum.
"""

import argparse
import concurrent.futures
import dataclasses
import difflib
import fractions
import hashlib
import json
import math
import os
import re
from pathlib import Path
import shlex
import shutil
import struct
import subprocess
import tempfile
import time

import perm_unit


ROOT = Path(__file__).resolve().parent.parent
SCALAR_TYPES = {"s32", "u32", "s16", "u16", "int", "f32", "float", "f64", "double"}
INTEGER_TYPES = {"s32", "u32", "s16", "u16", "int"}
FLOAT_TYPES = {"f32", "float", "f64", "double"}
ASSIGNMENTS = {"=", "+=", "-=", "*=", "/=", "&=", "|=", "^=", "<<=", ">>="}
PRECEDENCE = {",": 1, **{operator: 2 for operator in ASSIGNMENTS}, "?": 3,
              "||": 4, "&&": 5, "|": 6, "^": 7, "&": 8, "==": 9, "!=": 9,
              "<": 10, ">": 10, "<=": 10, ">=": 10, "<<": 11, ">>": 11,
              "+": 12, "-": 12, "*": 13, "/": 13, "%": 13}
OPERATORS = sorted({*PRECEDENCE, "->", "++", "--", "...", "##", "(", ")", "[", "]",
                    "{", "}", ";", ":", ".", "!", "~", "#"}, key=len, reverse=True)


@dataclasses.dataclass(frozen=True)
class Token:
    text: str
    start: int
    end: int
    kind: str = "punct"


def tokenize(source):
    """Keep original offsets; comments, literals and preprocessor lines are opaque."""
    tokens = []
    position = 0
    while position < len(source):
        start = position
        character = source[position]
        if character.isspace():
            position += 1
            continue
        if source.startswith("//", position):
            end = source.find("\n", position)
            position = len(source) if end < 0 else end
            continue
        if source.startswith("/*", position):
            end = source.find("*/", position + 2)
            if end < 0:
                raise ValueError("unterminated C comment")
            position = end + 2
            continue
        if character == "#" and not source[source.rfind("\n", 0, position) + 1:position].strip():
            while True:
                end = source.find("\n", position)
                if end < 0:
                    position = len(source)
                    break
                position = end + 1
                if source[end - 1:end] != "\\":
                    break
            tokens.append(Token(source[start:position], start, position, "directive"))
            continue
        if character in "\"'" or (character == "L" and source[position + 1:position + 2] in {"\"", "'"}):
            if character == "L":
                position += 1
            quote = source[position]
            position += 1
            while position < len(source):
                if source[position] == "\\":
                    position += 2
                elif source[position] == quote:
                    position += 1
                    break
                else:
                    position += 1
            else:
                raise ValueError("unterminated C literal")
            tokens.append(Token(source[start:position], start, position, "literal"))
            continue
        if character.isalpha() or character in "_$":
            position += 1
            while position < len(source) and (source[position].isalnum() or source[position] in "_$"):
                position += 1
            tokens.append(Token(source[start:position], start, position, "ident"))
            continue
        if character.isdigit() or (character == "." and source[position + 1:position + 2].isdigit()):
            position += 1
            while position < len(source):
                next_character = source[position]
                if next_character.isalnum() or next_character in "._":
                    position += 1
                elif next_character in "+-" and source[position - 1] in "eEpP":
                    position += 1
                else:
                    break
            tokens.append(Token(source[start:position], start, position, "number"))
            continue
        operator = next((item for item in OPERATORS if source.startswith(item, position)), character)
        position += len(operator)
        tokens.append(Token(operator, start, position))
    return tokens


def pairs(tokens):
    stack = []
    matching = {}
    closing = {")": "(", "]": "[", "}": "{"}
    for index, token in enumerate(tokens):
        if token.kind != "punct":
            continue
        if token.text in {"(", "[", "{"}:
            stack.append(index)
        elif token.text in closing:
            if not stack or tokens[stack[-1]].text != closing[token.text]:
                raise ValueError("unbalanced C delimiters")
            opening = stack.pop()
            matching[opening] = index
            matching[index] = opening
    if stack:
        raise ValueError("unbalanced C delimiters")
    return matching


def active_tokens(source, defines=None):
    """Select simple C preprocessor branches without expanding or editing macros.

    Includes are not read. Unknown #if expressions fail closed, rather than
    combining mutually exclusive braces. Exact build -D values are supplied by
    the CLI; direct mutation API callers default to this project's MUST_MATCH.
    """
    definitions = dict(defines if defines is not None else {"MUST_MATCH": "1"})
    output = []
    stack = []
    active = True

    def condition(parts):
        normalized = []
        position = 0
        while position < len(parts):
            token = parts[position]
            if token.text == "defined":
                position += 1
                grouped = parts[position].text == "("
                if grouped:
                    position += 1
                name = parts[position].text
                normalized.append(Token("1" if name in definitions else "0", 0, 1, "number"))
                position += 2 if grouped else 1
                continue
            if token.kind == "ident":
                value = definitions.get(token.text, "0")
                if not value.strip().isdigit():
                    raise ValueError(f"unsupported preprocessor value for {token.text}: {value}")
                token = Token(value, 0, len(value), "number")
            normalized.append(token)
            position += 1
        parser = ExpressionParser(normalized, 0, len(normalized), set())
        expression = parser.parse()
        if parser.position != len(normalized):
            raise ValueError("unsupported #if expression")
        def evaluate(node):
            if node.kind == "number":
                return int(node.operator.rstrip("uUlL"), 0)
            if node.kind == "group":
                return evaluate(node.children[0])
            if node.kind == "unary" and node.operator == "!":
                return not evaluate(node.children[0])
            if node.kind == "binary":
                left, right = (evaluate(child) for child in node.children)
                operators = {"&&": lambda: bool(left and right), "||": lambda: bool(left or right),
                             "==": lambda: left == right, "!=": lambda: left != right, "<": lambda: left < right,
                             ">": lambda: left > right, "<=": lambda: left <= right, ">=": lambda: left >= right}
                if node.operator in operators:
                    return operators[node.operator]()
            raise ValueError("unsupported #if expression")
        return bool(evaluate(expression))

    for token in tokenize(source):
        if token.kind != "directive":
            if active:
                output.append(token)
            continue
        parts = tokenize(token.text[1:].replace("\\\n", " "))
        if not parts:
            continue
        directive = parts[0].text
        if directive in {"if", "ifdef", "ifndef"}:
            selected = False
            if active:
                selected = condition(parts[1:]) if directive == "if" else parts[1].text in definitions
                if directive == "ifndef":
                    selected = not selected
            stack.append((active, selected))
            active = active and selected
        elif directive in {"else", "elif"}:
            if not stack:
                raise ValueError("unbalanced preprocessor conditional")
            parent, taken = stack[-1]
            selected = not taken and parent and (directive == "else" or condition(parts[1:]))
            stack[-1] = (parent, taken or selected)
            active = selected
        elif directive == "endif":
            if not stack:
                raise ValueError("unbalanced preprocessor conditional")
            active = stack.pop()[0]
        elif active and directive == "define" and len(parts) >= 2:
            definitions[parts[1].text] = " ".join(part.text for part in parts[2:]) or "1"
        elif active and directive == "undef" and len(parts) >= 2:
            definitions.pop(parts[1].text, None)
        output.append(token)
    if stack:
        raise ValueError("unbalanced preprocessor conditional")
    return output


@dataclasses.dataclass(frozen=True)
class Function:
    start: int
    opening: int
    closing: int
    params: dict


def find_function(tokens, name):
    matching = pairs(tokens)
    depth = 0
    boundary = 0
    for index, token in enumerate(tokens):
        if depth == 0 and token.text == name and token.kind == "ident":
            after = index + 1
            if after < len(tokens) and tokens[after].text == "(":
                end = matching[after]
                opening = end + 1
                if opening < len(tokens) and tokens[opening].text == "{":
                    params = {}
                    for param in range(after + 1, end - 1):
                        if tokens[param].text in SCALAR_TYPES and tokens[param + 1].kind == "ident":
                            params[tokens[param + 1].text] = tokens[param].text
                    return Function(boundary, opening, matching[opening], params)
        if token.text == "{":
            depth += 1
        elif token.text == "}":
            depth -= 1
            if depth == 0:
                boundary = index + 1
        elif depth == 0 and (token.text == ";" or token.kind == "directive"):
            boundary = index + 1
    raise ValueError(f"function definition {name!r} not found")


@dataclasses.dataclass(frozen=True)
class Expression:
    kind: str
    first: int
    last: int
    operator: str = ""
    children: tuple = ()

    def walk(self):
        yield self
        for child in self.children:
            yield from child.walk()


class ExpressionParser:
    def __init__(self, tokens, first, last, types):
        self.tokens = tokens
        self.position = first
        self.last = last
        self.types = types | SCALAR_TYPES | {"u8", "s8", "char", "unsigned", "signed", "long", "short", "void"}

    def parse(self, minimum=1):
        if self.position >= self.last:
            raise ValueError("missing expression")
        first = self.position
        token = self.tokens[self.position]
        self.position += 1
        if token.text in {"+", "-", "!", "~", "*", "&", "++", "--", "sizeof"}:
            child = self.parse(14)
            node = Expression("unary", first, child.last, token.text, (child,))
        elif token.text == "(":
            end = self.position
            while end < self.last and (self.tokens[end].text in self.types or self.tokens[end].text in {"*", "const", "volatile"}):
                end += 1
            if end > self.position and end < self.last and self.tokens[end].text == ")":
                type_name = " ".join(item.text for item in self.tokens[self.position:end])
                self.position = end + 1
                child = self.parse(14)
                node = Expression("cast", first, child.last, type_name, (child,))
            else:
                child = self.parse()
                self.expect(")")
                node = Expression("group", first, self.position, children=(child,))
        elif token.kind in {"ident", "number", "literal"}:
            node = Expression(token.kind, first, self.position, token.text)
        else:
            raise ValueError("unsupported expression")
        while self.position < self.last:
            operator = self.tokens[self.position].text
            if operator in {"(", "[", ".", "->", "++", "--"}:
                self.position += 1
                if operator == "(":
                    arguments = []
                    while self.position < self.last and self.tokens[self.position].text != ")":
                        arguments.append(self.parse(2))
                        if self.tokens[self.position].text != ",":
                            break
                        self.position += 1
                    self.expect(")")
                    node = Expression("call", first, self.position, children=(node, *arguments))
                elif operator == "[":
                    child = self.parse()
                    self.expect("]")
                    node = Expression("index", first, self.position, children=(node, child))
                elif operator in {".", "->"}:
                    self.position += 1
                    node = Expression("member", first, self.position, operator, (node,))
                else:
                    node = Expression("postfix", first, self.position, operator, (node,))
                continue
            precedence = PRECEDENCE.get(operator, 0)
            if precedence < minimum:
                break
            self.position += 1
            if operator == "?":
                middle = self.parse()
                self.expect(":")
                right = self.parse(precedence)
                node = Expression("ternary", first, right.last, operator, (node, middle, right))
            else:
                right = self.parse(precedence if operator in ASSIGNMENTS else precedence + 1)
                node = Expression("binary", first, right.last, operator, (node, right))
        return node

    def expect(self, text):
        if self.position >= self.last or self.tokens[self.position].text != text:
            raise ValueError("unsupported expression delimiters")
        self.position += 1


@dataclasses.dataclass(frozen=True)
class Statement:
    first: int
    last: int
    block: int
    declaration: str = ""
    type_name: str = ""
    expression: object = None


class Body:
    def __init__(self, source, func, defines=None):
        self.source = source
        self.tokens = active_tokens(source, defines)
        self.function = find_function(self.tokens, func)
        self.matching = pairs(self.tokens)
        self.parents = {}
        self.statements = []
        self.conditions = []
        self.declarations = {}
        self.types = dict(self.function.params)
        self.ambiguous = set()
        self.owners = {}
        blocks = [self.function.opening]
        start = self.function.opening + 1
        nesting = 0
        for index in range(start, self.function.closing):
            token = self.tokens[index]
            self.owners[index] = blocks[-1]
            if token.kind == "directive":
                start = index + 1
            elif token.text in {"(", "["}:
                nesting += 1
            elif token.text in {")", "]"}:
                nesting -= 1
            elif token.text == "{" and nesting == 0:
                self.parents[index] = blocks[-1]
                blocks.append(index)
                start = index + 1
            elif token.text == "}" and nesting == 0:
                if len(blocks) > 1:
                    blocks.pop()
                start = index + 1
            elif token.text == ";" and nesting == 0:
                self.add_statement(start, index + 1, blocks[-1])
                start = index + 1
        for index in range(self.function.opening + 1, self.function.closing - 1):
            if self.tokens[index].text not in {"if", "while", "for", "switch"} or self.tokens[index + 1].text != "(":
                continue
            last = self.matching[index + 1]
            pieces = []
            first = index + 2
            cursor = first
            while cursor < last:
                if self.tokens[cursor].text in {"(", "["}:
                    cursor = self.matching[cursor] + 1
                    continue
                if self.tokens[cursor].text == ";":
                    pieces.append((first, cursor))
                    first = cursor + 1
                cursor += 1
            pieces.append((first, last))
            for first, end in pieces:
                try:
                    parser = ExpressionParser(self.tokens, first, end, set(self.types.values()))
                    expression = parser.parse()
                    if parser.position == end:
                        self.conditions.append(Statement(index, last + 1, self.owners[index], expression=expression))
                except (ValueError, IndexError):
                    pass

    def add_statement(self, first, last, block):
        if first >= last - 1:
            return
        declaration = type_name = ""
        expression_first = first
        if self.tokens[first].text in SCALAR_TYPES and self.tokens[first + 1].kind == "ident":
            if self.tokens[first + 2].text in {";", "="}:
                type_name = self.tokens[first].text
                declaration = self.tokens[first + 1].text
                if declaration in self.types:
                    self.ambiguous.add(declaration)
                    return
                self.types[declaration] = type_name
                expression_first = first + 3 if self.tokens[first + 2].text == "=" else last - 1
        elif self.tokens[first].text == "return":
            expression_first += 1
        expression = None
        if expression_first < last - 1:
            try:
                parser = ExpressionParser(self.tokens, expression_first, last - 1, set(self.types.values()))
                expression = parser.parse()
                if parser.position != last - 1:
                    return
            except (ValueError, IndexError):
                return
        statement = Statement(first, last, block, declaration, type_name, expression)
        self.statements.append(statement)
        if declaration:
            self.declarations[declaration] = statement

    def text(self, first, last):
        return self.source[self.tokens[first].start:self.tokens[last - 1].end]

    def expr_text(self, expression):
        return self.text(expression.first, expression.last)

    def span(self, first, last):
        return self.tokens[first].start, self.tokens[last - 1].end

    def uses(self, name):
        return [index for index in range(self.function.opening + 1, self.function.closing)
                if self.tokens[index].kind == "ident" and self.tokens[index].text == name
                and self.tokens[index - 1].text not in {".", "->"}]

    def ancestors(self, block):
        while True:
            yield block
            if block not in self.parents:
                break
            block = self.parents[block]

    def indent(self, index):
        position = self.tokens[index].start
        prefix = self.source[self.source.rfind("\n", 0, position) + 1:position]
        return prefix if not prefix.strip() else "    "


def edits(source, changes):
    previous = len(source) + 1
    for start, end, replacement in sorted(changes, reverse=True):
        if end > previous or start > end:
            raise ValueError("overlapping source edits")
        source = source[:start] + replacement + source[end:]
        previous = start
    return source


DECLARATION_STARTS = {"const", "volatile", "static", "register", "struct", "union", "enum", "unsigned", "signed",
                      "char", "short", "long", "void", "typedef", *SCALAR_TYPES}
CONTROL_STARTS = {"if", "for", "while", "switch", "do", "else", "return", "break", "continue", "goto", "case", "default"}


def block_items(tokens, matching, opening):
    """Top-level items (first, last) of one block: `;`-terminated statements, compound statements, control."""
    items = []
    index = opening + 1
    closing = matching[opening]
    while index < closing:
        if tokens[index].kind == "directive":
            index += 1
            continue
        first = index
        while index < closing:
            text = tokens[index].text
            if text in {"(", "["}:
                index = matching[index] + 1
                continue
            if text == "{":
                compound = index == first or tokens[first].text in CONTROL_STARTS
                index = matching[index] + 1
                if not compound or tokens[first].text == "do" or (index < closing and tokens[index].text == "else"):
                    continue
                break
            index += 1
            if text == ";" or (text == ":" and tokens[first].text in {"case", "default"}):
                break
        items.append((first, index))
    return items


def looks_like_declaration(tokens, first, last):
    if tokens[first].text in CONTROL_STARTS:
        return False
    if tokens[first].text in DECLARATION_STARTS:
        return True
    cursor = first + 1
    while cursor < last and tokens[cursor].text == "*":
        cursor += 1
    return (tokens[first].kind == "ident" and cursor + 1 < last and tokens[cursor].kind == "ident"
            and tokens[cursor + 1].text in {";", "=", "[", ","})


def c89_declarations(source, func, defines=None, keep=frozenset()):
    """Hoist scalar declarations that a rewrite left after a statement (MWCC's C rejects them).

    `T x = e;` after a statement becomes `x = e;` in place with `T x;` after the block's leading
    declarations, so evaluation order is unchanged. Shadowed names are never declarations here.
    Names in `keep` (already "late" in the compiling input, e.g. after a declaring macro) stay put.
    """
    body = Body(source, func, defines)
    tokens = body.tokens
    changes = []
    for opening in [body.function.opening, *body.parents]:
        items = block_items(tokens, body.matching, opening)
        leading = 0
        while leading < len(items) and looks_like_declaration(tokens, *items[leading]):
            leading += 1
        insertion = tokens[items[leading - 1][1] - 1].end if leading else tokens[opening].end
        hoisted = []
        for first, last in items[leading:]:
            statement = next((item for item in body.declarations.values() if item.first == first and item.last == last), None)
            if statement is None or statement.declaration in keep:
                continue
            name = statement.declaration
            replacement = f"{name} = {body.expr_text(statement.expression)};" if tokens[first + 2].text == "=" else ""
            changes.append((*body.span(first, last), replacement))
            hoisted.append(f"{statement.type_name} {name};")
        if hoisted:
            indent = body.indent(items[0][0]) if items else "    "
            changes.append((insertion, insertion, "".join(f"\n{indent}{text}" for text in hoisted)))
    return edits(source, changes) if changes else source


def late_declarations(source, func, defines=None):
    hoisted = c89_declarations(source, func, defines)
    before = Body(source, func, defines)
    after = Body(hoisted, func, defines)
    return {name for name, statement in before.declarations.items()
            if name not in after.declarations or after.text(after.declarations[name].first, after.declarations[name].last)
            != before.text(statement.first, statement.last)}


def normalized_hash(source):
    tokens = tokenize(source)
    text = "\0".join(token.kind + ":" + token.text for token in tokens)
    return hashlib.sha256(text.encode("utf-8", "surrogateescape")).hexdigest()


def pure(expression):
    return all(node.kind not in {"call", "postfix"}
               and not (node.kind == "unary" and node.operator in {"++", "--", "*", "&", "sizeof"})
               and not (node.kind == "binary" and node.operator in ASSIGNMENTS | {","})
               for node in expression.walk())


def names(expression):
    return {node.operator for node in expression.walk() if node.kind == "ident"}


def unevaluated_sites(expression):
    delayed = set()
    for node in expression.walk():
        branches = ()
        if node.kind == "ternary":
            branches = node.children[1:]
        elif node.kind == "binary" and node.operator in {"&&", "||"}:
            branches = node.children[1:]
        elif node.kind == "unary" and node.operator == "sizeof":
            branches = node.children
        for branch in branches:
            for child in branch.walk():
                delayed.update(range(child.first, child.last))
    return delayed


def expression_type(expression, body):
    if expression.kind == "cast":
        return expression.operator if expression.operator in SCALAR_TYPES else None
    if expression.kind == "ident":
        return body.types.get(expression.operator)
    if expression.kind == "number":
        value = expression.operator.lower()
        if "." in value or ("e" in value and not value.startswith("0x")):
            return "f32" if value.endswith("f") else "double"
        return "u32" if value.endswith("u") else "int"
    if expression.kind in {"group", "unary"}:
        return expression_type(expression.children[0], body)
    if expression.kind == "binary":
        types = [expression_type(child, body) for child in expression.children]
        if expression.operator in {"==", "!=", "<", ">", "<=", ">="}:
            return "int"
        if None in types:
            return None
        if any(item in {"double", "f64"} for item in types):
            return "double"
        if any(item in FLOAT_TYPES for item in types):
            return "f32"
        return "u32" if "u32" in types else "int"
    return None


def replace_name(body, name, replacement, first=0, last=None):
    return [(body.tokens[index].start, body.tokens[index].end, replacement) for index in body.uses(name)
            if index >= first and (last is None or index < last)]


def float_literal(text):
    """(value, "f32" | "double") for a decimal floating literal token, else None."""
    lowered = text.lower()
    if lowered.startswith("0x") or not ("." in lowered or "e" in lowered):
        return None
    kind = "f32" if lowered.endswith("f") else "double"
    try:
        return float(lowered.rstrip("fl")), kind
    except ValueError:
        return None


def same_float(first, second, kind):
    if kind == "f32":
        return struct.pack(">f", first) == struct.pack(">f", second)
    return first == second


@dataclasses.dataclass(frozen=True)
class FloatConstant:
    name: str
    kind: str
    value: float
    literal: str
    visible_from: int


def float_constants(tokens):
    """Named floating constants of known value at file scope: `[static] const T NAME = [-]literal;`
    definitions, plus `extern const T NAME;` declarations whose name spells an exact value
    (`float_0p0078125_8041c6f8`, `float_neg2_...`; rounded spellings such as 6p2832 are not exact
    and are skipped). `visible_from` is the token index after the first declaration of the name.
    """
    found = {}
    declared = {}
    depth = 0
    start = True
    for index, token in enumerate(tokens):
        if token.text == "{":
            depth += 1
        elif token.text == "}":
            depth -= 1
            start = depth == 0
            continue
        if depth or token.kind == "directive" or token.text == ";":
            start = depth == 0 and token.text in {";"} or token.kind == "directive"
            continue
        if not start:
            continue
        start = False
        cursor = index
        if tokens[cursor].text in {"static", "extern"}:
            cursor += 1
        if cursor + 3 >= len(tokens) or tokens[cursor].text != "const" or tokens[cursor + 1].text not in FLOAT_TYPES:
            continue
        kind = "f32" if tokens[cursor + 1].text in {"f32", "float"} else "double"
        name_token = tokens[cursor + 2]
        if name_token.kind != "ident":
            continue
        name = name_token.text
        declared.setdefault(name, cursor + 3)
        following = tokens[cursor + 3].text
        if following == "=":
            negative = tokens[cursor + 4].text == "-"
            literal_index = cursor + 5 if negative else cursor + 4
            if literal_index + 1 >= len(tokens) or tokens[literal_index + 1].text != ";":
                continue
            parsed = float_literal(tokens[literal_index].text)
            if not parsed or (kind == "f32") != (parsed[1] == "f32"):
                continue
            value = -parsed[0] if negative else parsed[0]
            literal = ("-" if negative else "") + tokens[literal_index].text
            found[name] = (kind, value, literal)
        elif following == ";" and name not in found:
            match = re.fullmatch(r"(?:float|double)_(neg)?(\d+)(?:p(\d+))?_[0-9a-f]{8}", name)
            if not match:
                continue
            decimal = ("-" if match[1] else "") + match[2] + "." + (match[3] or "0")
            value = float(decimal)
            exact = struct.unpack(">f", struct.pack(">f", value))[0] if kind == "f32" else value
            if fractions.Fraction(decimal) != fractions.Fraction(exact):
                continue
            found[name] = (kind, value, repr(value) + ("f" if kind == "f32" else ""))
    return {name: FloatConstant(name, kind, value, literal, declared[name]) for name, (kind, value, literal) in found.items()}


def mutations(source, func, defines=None):
    """Yield (description, full unit) candidates; unsupported C is left untouched.

    Dataflow rewrites are restricted to straight-line, non-address-taken scalar
    locals. No operator edits comments, string contents, or preprocessor lines.
    """
    body = Body(source, func, defines)
    tokens = body.tokens
    fresh = "mwsearch_temp"
    identifiers = {token.text for token in tokens if token.kind == "ident"}
    suffix = 0
    while fresh in identifiers:
        suffix += 1
        fresh = f"mwsearch_temp_{suffix}"

    try:
        original_late = frozenset(late_declarations(source, func, defines))
    except (ValueError, IndexError, KeyError, StopIteration):
        original_late = None

    def candidate(description, changes):
        result = edits(source, changes)
        try:
            if original_late is not None:
                result = c89_declarations(result, func, defines, original_late)
        except (ValueError, IndexError, KeyError, StopIteration):
            pass
        return description, result

    def safe_local(name):
        uses = body.uses(name)
        return name not in body.ambiguous and uses and all(tokens[index - 1].text not in {"&", "++", "--"}
                            and tokens[index + 1].text not in {"++", "--"} for index in uses)

    def can_clobber(name, after, block):
        future = [index for index in body.uses(name) if index >= after]
        if not future:
            return True
        definition = next((item for item in body.statements if item.first == future[0]), None)
        if not definition or definition.block != block or not definition.expression:
            return False
        expression = definition.expression
        return (expression.kind == "binary" and expression.operator == "="
                and expression.children[0].kind == "ident" and expression.children[0].operator == name
                and name not in names(expression.children[1]))

    constants = {name: constant for name, constant in float_constants(tokens).items()
                 if constant.visible_from <= body.function.start}

    scaled_sites = {}
    for statement in [*body.statements, *body.conditions]:
        expression = statement.expression
        if not expression:
            continue
        delayed = unevaluated_sites(expression)
        for node in expression.walk():
            if node.kind == "binary":
                left, right = node.children
                left_text, right_text = body.expr_text(left), body.expr_text(right)
                operator = node.operator
                if operator in {"+", "*", "&", "|", "==", "!=", "<", ">", "<=", ">="} and pure(left) and pure(right):
                    flipped = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}.get(operator, operator)
                    yield candidate(f"swap {operator} at {tokens[node.first].start}",
                                    [(*body.span(node.first, node.last), f"({right_text}) {flipped} ({left_text})")])
                if operator == "=" and left.kind == "ident" and right.kind == "binary" and right.operator in {"+", "-", "*", "/", "&", "|", "^", "<<", ">>"}:
                    if body.expr_text(right.children[0]) == left_text:
                        yield candidate(f"compound {left_text}", [(*body.span(node.first, node.last),
                                        f"{left_text} {right.operator}= {body.expr_text(right.children[1])}")])
                elif operator in ASSIGNMENTS - {"="} and left.kind == "ident":
                    yield candidate(f"expand {operator}", [(*body.span(node.first, node.last),
                                    f"{left_text} = {left_text} {operator[:-1]} ({right_text})")])
                if operator in {"/", "*"} and right.kind == "number" and right.operator.lower() in {"2.0f", "0.5f"}:
                    if (operator, right.operator.lower()) in {("/", "2.0f"), ("*", "0.5f")}:
                        replacement = f"({left_text}) {'* 0.5f' if operator == '/' else '/ 2.0f'}"
                        yield candidate("float reciprocal", [(*body.span(node.first, node.last), replacement)])
                parsed = float_literal(right.operator) if operator == "/" and right.kind == "number" else None
                if (parsed and pure(left) and node.first not in delayed and not statement.declaration
                        and tokens[statement.first].text not in {"return", "if", "while", "for", "switch"}):
                    value, kind = parsed
                    mantissa = math.frexp(value)[0] if value else 0
                    for name, constant in constants.items():
                        if abs(mantissa) == 0.5 and constant.kind == kind and same_float(constant.value, 1.0 / value, kind):
                            statement_start = tokens[statement.first].start
                            indent = body.indent(statement.first)
                            yield candidate(f"via-temp {left_text[:50]} * {name}", [
                                (statement_start, statement_start, f"{kind} {fresh};\n{indent}{fresh} = {left_text};\n{indent}"),
                                (*body.span(node.first, node.last), f"{fresh} * {name}")])
                            scaled_sites.setdefault((right.operator, name, kind), []).append((statement, node, left_text))
            direct_call = (node.kind == "call" and expression.kind == "binary" and expression.operator == "="
                           and expression.children[0].kind == "ident" and expression.children[1] is node)
            type_name = body.types.get(expression.children[0].operator) if direct_call else expression_type(node, body)
            if node.kind not in {"cast", "binary", "index", "member", "call"} or not type_name or (not pure(node) and not direct_call):
                continue
            if node.kind == "binary" and node.operator in ASSIGNMENTS | {","}:
                continue
            if node.first in delayed:
                continue
            if statement.declaration or tokens[statement.first].text in {"return", "if", "while", "for"}:
                continue
            calls = [item for item in expression.walk() if item.kind == "call"]
            if any(item is not expression for item in calls) and not direct_call:
                continue
            node_start, node_end = body.span(node.first, node.last)
            statement_start = tokens[statement.first].start
            assignment = f"{type_name} {fresh};\n{body.indent(statement.first)}{fresh} = {body.expr_text(node)};\n{body.indent(statement.first)}"
            yield candidate(f"via-temp {body.expr_text(node)[:65]}",
                            [(statement_start, statement_start, assignment), (node_start, node_end, fresh)])
            for local, local_type in body.types.items():
                if local_type != type_name or local in names(node) or not safe_local(local):
                    continue
                declaration = body.declarations.get(local)
                if declaration and (declaration.first >= statement.first or declaration.block not in set(body.ancestors(statement.block))):
                    continue
                if can_clobber(local, statement.first, statement.block):
                    insertion = f"{local} = {body.expr_text(node)};\n{body.indent(statement.first)}"
                    yield candidate(f"via existing {local}", [(statement_start, statement_start, insertion), (node_start, node_end, local)])

    for declaration_name, declaration in body.declarations.items():
        if not safe_local(declaration_name):
            continue
        all_uses = body.uses(declaration_name)
        uses = [index for index in all_uses if index >= declaration.last]
        if len(uses) == 1 and declaration.expression and (pure(declaration.expression) or declaration.expression.kind == "call"):
            use = uses[0]
            consumer = next((item for item in body.statements if item.first <= use < item.last), None)
            if consumer and consumer.block == declaration.block and consumer.first == declaration.last:
                allowed = body.expr_text(declaration.expression) != declaration_name
                if not pure(declaration.expression) and consumer.expression and any(node.kind == "call" for node in consumer.expression.walk()):
                    allowed = False
                if consumer.expression and use in unevaluated_sites(consumer.expression):
                    allowed = False
                if allowed:
                    replacement = f"(({declaration.type_name})({body.expr_text(declaration.expression)}))"
                    yield candidate(f"inline {declaration_name}", [(*body.span(declaration.first, declaration.last), ""),
                                    (tokens[use].start, tokens[use].end, replacement)])
        for destination, destination_type in body.types.items():
            if destination == declaration_name or destination_type != declaration.type_name or not safe_local(destination):
                continue
            other = body.declarations.get(destination)
            if other and (other.first >= declaration.first or other.block not in set(body.ancestors(declaration.block))):
                continue
            if not declaration.expression or len(uses) == 0:
                continue
            end = max(uses)
            destination_uses = [index for index in body.uses(destination) if index >= declaration.last]
            if any(index <= end for index in destination_uses):
                continue
            if not can_clobber(destination, declaration.last, declaration.block):
                continue
            scope_uses = [body.owners[index] for index in uses]
            if any(declaration.block not in set(body.ancestors(block)) for block in scope_uses):
                continue
            changes = [(*body.span(declaration.first, declaration.first + 2), destination)]
            changes += replace_name(body, declaration_name, destination, declaration.last)
            yield candidate(f"reuse {destination} for {declaration_name}", changes)
        if not declaration.expression and uses:
            common = set(body.ancestors(body.owners[uses[0]]))
            for use in uses[1:]:
                common &= set(body.ancestors(body.owners[use]))
            innermost = next(block for block in body.ancestors(body.owners[uses[0]]) if block in common)
            for destination in {body.function.opening, innermost} - {declaration.block}:
                if destination > min(uses):
                    continue
                opening = tokens[destination].end
                replacement = "\n" + body.indent(declaration.first) + body.text(declaration.first, declaration.last)
                yield candidate(f"scope {declaration_name} to block {destination}",
                                [(*body.span(declaration.first, declaration.last), ""), (opening, opening, replacement)])
        elif declaration.expression and declaration.block != body.function.opening:
            insertion = tokens[body.function.opening].end
            yield candidate(f"scope initialized {declaration_name} to function", [
                (*body.span(declaration.first, declaration.last), f"{declaration_name} = {body.expr_text(declaration.expression)};"),
                (insertion, insertion, f"\n{body.indent(declaration.first)}{declaration.type_name} {declaration_name};")])
        assignments = [item for item in body.statements if item.expression and item.expression.kind == "binary"
                       and item.expression.operator == "=" and item.expression.children[0].kind == "ident"
                       and item.expression.children[0].operator == declaration_name]
        if not declaration.expression and assignments and uses:
            first_write = assignments[0]
            if all(index >= first_write.first for index in uses) and all(body.owners[index] == first_write.block for index in uses):
                for destination, destination_type in body.types.items():
                    if destination == declaration_name or destination_type != declaration.type_name or not safe_local(destination):
                        continue
                    other = body.declarations.get(destination)
                    if other and (other.first >= first_write.first or other.block not in set(body.ancestors(first_write.block))):
                        continue
                    future = [index for index in body.uses(destination) if index >= first_write.last]
                    if any(index <= max(uses) for index in future) or not can_clobber(destination, first_write.last, first_write.block):
                        continue
                    changes = [(*body.span(declaration.first, declaration.last), "")]
                    changes += replace_name(body, declaration_name, destination, declaration.last)
                    yield candidate(f"reuse disjoint {destination} for {declaration_name}", changes)
        for assignment in assignments:
            rhs = assignment.expression.children[1]
            next_definition = next((item for item in assignments if item.first > assignment.first), None)
            next_write = next_definition.first if next_definition else body.function.closing
            if next_definition and declaration_name in names(next_definition.expression.children[1]):
                continue
            live = [index for index in uses if assignment.last <= index < next_write]
            region = [item for item in body.statements if assignment.first <= item.first < next_write]
            if not live or declaration_name in names(rhs):
                continue
            if all(item.block == assignment.block for item in region):
                changes = [(tokens[assignment.first].start, tokens[assignment.first].start,
                            f"{declaration.type_name} {fresh};\n{body.indent(assignment.first)}")]
                changes += replace_name(body, declaration_name, fresh, assignment.first, next_write)
                yield candidate(f"split live range of {declaration_name}", changes)
            if len(live) != 1:
                continue
            consumer = next((item for item in region if item.first <= live[0] < item.last), None)
            if not consumer:
                continue
            if consumer.expression and live[0] in unevaluated_sites(consumer.expression):
                continue
            intervening = tokens[assignment.last:consumer.first]
            if any(token.text != "{" and token.kind != "directive" for token in intervening):
                continue
            if any(item.first < consumer.first and item.first > assignment.first for item in region):
                continue
            if not pure(rhs) and consumer.expression and any(node.kind == "call" for node in consumer.expression.walk()
                                                           if not (node.first == consumer.expression.first and node.last == consumer.expression.last)):
                continue
            use = live[0]
            changes = [(*body.span(assignment.first, assignment.last), ""),
                       (tokens[use].start, tokens[use].end, f"(({declaration.type_name})({body.expr_text(rhs)}))")]
            if len(assignments) == 1 and len(uses) == 2 and not declaration.expression:
                changes.append((*body.span(declaration.first, declaration.last), ""))
            yield candidate(f"inline assignment to {declaration_name}", changes)
        if declaration.type_name in INTEGER_TYPES:
            if not type_uses_safe(declaration_name, body):
                continue
            value = integer_range(declaration.expression, body) if declaration.expression else None
            writes = [integer_range(item.expression.children[1], body) for item in assignments]
            ranges = ([value] if declaration.expression else []) + writes
            if not ranges or any(item is None for item in ranges):
                continue
            covered = {declaration.first + 1} | {item.expression.children[0].first for item in assignments}
            if any(tokens[index + 1].text in ASSIGNMENTS and index not in covered for index in all_uses):
                continue
            low, high = min(item[0] for item in ranges), max(item[1] for item in ranges)
            limits = {"s32": (-2147483648, 2147483647), "int": (-2147483648, 2147483647),
                      "u32": (0, 4294967295), "s16": (-32768, 32767), "u16": (0, 65535)}
            for type_name, (minimum, maximum) in limits.items():
                if type_name != declaration.type_name and minimum <= low <= high <= maximum:
                    yield candidate(f"type {declaration_name}: {declaration.type_name}->{type_name}",
                                    [(tokens[declaration.first].start, tokens[declaration.first].end, type_name)])


    def fixed_site(index):
        depth = 0
        for cursor in range(index - 1, body.function.opening, -1):
            text = tokens[cursor].text
            if text in {")", "]"}:
                depth += 1
            elif text in {"(", "["}:
                if depth == 0:
                    caller = tokens[cursor - 1]
                    if text == "(" and caller.kind == "ident" and caller.text.isupper():
                        return True
                else:
                    depth -= 1
            elif text == "{":
                return tokens[cursor - 1].text in {"=", ",", "{"}
            elif text in {";", "}"}:
                return False
        return False

    for (literal, name, kind), sites in scaled_sites.items():
        statements = [statement for statement, _, _ in sites]
        if len(set(statements)) != len(statements):
            continue
        groups = {}
        for site in sites:
            groups.setdefault(site[0].block, []).append(site)
        selections = [(f"(block {block})", group) for block, group in groups.items() if 1 < len(group) < len(sites)]
        if len(sites) > 1:
            selections.append((f"(all {len(sites)})", sites))
        insertion = tokens[body.function.opening].end
        for label, group in selections:
            changes = [(insertion, insertion, f"\n{body.indent(group[0][0].first)}{kind} {fresh};")]
            for statement, node, left_text in group:
                statement_start = tokens[statement.first].start
                changes.append((statement_start, statement_start, f"{fresh} = {left_text};\n{body.indent(statement.first)}"))
                changes.append((*body.span(node.first, node.last), f"{fresh} * {name}"))
            yield candidate(f"via-temp / {literal} -> * {name} {label}", changes)

    literal_sites = {}
    reciprocal_sites = {}
    named_sites = {}
    for index in range(body.function.opening + 1, body.function.closing):
        token = tokens[index]
        if fixed_site(index) if token.kind in {"number", "ident"} else True:
            continue
        if token.kind == "ident" and token.text in constants and tokens[index - 1].text not in {".", "->", "&"}:
            named_sites.setdefault(token.text, []).append(index)
        parsed = float_literal(token.text) if token.kind == "number" else None
        if not parsed:
            continue
        value, kind = parsed
        for name, constant in constants.items():
            if constant.kind != kind:
                continue
            if same_float(constant.value, value, kind):
                literal_sites.setdefault((token.text, name), []).append(index)
            mantissa, _ = math.frexp(value) if value else (0, 0)
            if (tokens[index - 1].text in {"/", "/="} and abs(mantissa) == 0.5
                    and same_float(constant.value, 1.0 / value, kind)):
                reciprocal_sites.setdefault((token.text, name), []).append(index)
    for (literal, name), sites in literal_sites.items():
        for index in sites:
            yield candidate(f"literal {literal} -> {name} at {tokens[index].start}",
                            [(tokens[index].start, tokens[index].end, name)])
        if len(sites) > 1:
            yield candidate(f"literal {literal} -> {name} (all {len(sites)})",
                            [(tokens[index].start, tokens[index].end, name) for index in sites])
    for (literal, name), sites in reciprocal_sites.items():
        def multiply(index):
            return (tokens[index - 1].start, tokens[index].end, f"{'*=' if tokens[index - 1].text == '/=' else '*'} {name}")
        for index in sites:
            yield candidate(f"/ {literal} -> * {name} at {tokens[index].start}", [multiply(index)])
        if len(sites) > 1:
            yield candidate(f"/ {literal} -> * {name} (all {len(sites)})", [multiply(index) for index in sites])
    for name, sites in named_sites.items():
        literal = constants[name].literal
        literal = f"({literal})" if literal.startswith("-") else literal
        for index in sites:
            yield candidate(f"{name} -> literal {literal} at {tokens[index].start}",
                            [(tokens[index].start, tokens[index].end, literal)])
        if len(sites) > 1:
            yield candidate(f"{name} -> literal {literal} (all {len(sites)})",
                            [(tokens[index].start, tokens[index].end, literal) for index in sites])

    for first, second in zip(body.statements, body.statements[1:]):
        if first.last != second.first or first.block != second.block or first.declaration or second.declaration:
            continue
        def accesses(statement):
            expression = statement.expression
            if not expression or expression.kind != "binary" or expression.operator not in ASSIGNMENTS:
                return None
            left, right = expression.children
            if left.kind != "ident" or left.operator not in body.types or not pure(right):
                return None
            reads = names(right) | ({left.operator} if expression.operator != "=" else set())
            if not reads <= body.types.keys():
                return None
            return {left.operator}, reads
        first_access, second_access = accesses(first), accesses(second)
        if first_access and second_access:
            first_writes, first_reads = first_access
            second_writes, second_reads = second_access
            if not (first_writes & (second_writes | second_reads) or second_writes & first_reads):
                yield candidate("reorder independent statements", [(*body.span(first.first, first.last), body.text(second.first, second.last)),
                                (*body.span(second.first, second.last), body.text(first.first, first.last))])


def integer_range(expression, body):
    if not expression:
        return None
    if expression.kind == "number":
        try:
            value = int(expression.operator.rstrip("uUlL"), 0)
            return value, value
        except ValueError:
            return None
    if expression.kind == "group":
        return integer_range(expression.children[0], body)
    if expression.kind == "unary" and expression.operator in {"+", "-"}:
        interval = integer_range(expression.children[0], body)
        if interval:
            return interval if expression.operator == "+" else (-interval[1], -interval[0])
    if expression.kind == "binary" and expression.operator in {"==", "!=", "<", ">", "<=", ">="}:
        return 0, 1
    return None


def type_uses_safe(name, body):
    """Do not change integer promotions, sizeof, varargs, or address-taking.

    Bounded definitions and direct returns/assignments or comparisons against
    nonnegative integer literals are the deliberately small supported subset.
    """
    declaration = body.declarations[name]
    allowed = {declaration.first + 1}
    for statement in body.statements:
        expression = statement.expression
        if not expression or name not in names(expression):
            continue
        if expression.kind == "ident" and expression.operator == name and body.tokens[statement.first].text == "return":
            allowed.add(expression.first)
        elif expression.kind == "binary" and expression.operator == "=":
            left, right = expression.children
            if left.kind != "ident":
                return False
            if left.operator == name:
                allowed.add(left.first)
            if right.kind == "ident" and right.operator == name:
                allowed.add(right.first)
            elif name in names(right):
                return False
        else:
            return False
    for statement in body.conditions:
        expression = statement.expression
        if name not in names(expression):
            continue
        if expression.kind != "binary" or expression.operator not in {"==", "!=", "<", ">", "<=", ">="}:
            return False
        left, right = expression.children
        if left.kind != "ident" or left.operator != name:
            left, right = right, left
        interval = integer_range(right, body)
        if left.kind != "ident" or left.operator != name or not interval or interval[0] < 0:
            return False
        allowed.add(left.first)
    return set(body.uses(name)) <= allowed


@dataclasses.dataclass(frozen=True)
class ObjectImage:
    functions: dict
    data: dict


def read_object(path):
    """Function names/sizes and raw data sections; instruction scoring is objdiff's (see objdiff_scores)."""
    try:
        from elftools.elf.elffile import ELFFile
    except ImportError as error:
        raise RuntimeError("pyelftools required: uv run --with pyelftools python tools/mwsearch.py ...") from error
    with open(path, "rb") as handle:
        elf = ELFFile(handle)
        if elf['e_machine'] != "EM_PPC" or elf.little_endian:
            raise ValueError("expected big-endian PowerPC ELF")
        symbols = elf.get_section_by_name(".symtab")
        if symbols is None:
            raise ValueError("object has no symbol table")
        functions = {symbol.name: symbol['st_size'] for symbol in symbols.iter_symbols()
                     if symbol['st_info']['type'] == "STT_FUNC" and isinstance(symbol['st_shndx'], int)
                     and symbol['st_size']}
        data = {section.name: section.data() for section in elf.iter_sections()
                if section.name in {".sdata2", ".rodata", ".data", ".sdata"}}
        return ObjectImage(functions, data)


@dataclasses.dataclass(frozen=True)
class Score:
    """objdiff's match percent for one function, plus its count of differing aligned rows."""
    percent: float
    differing: int
    total: int
    signature: str = dataclasses.field(default="", compare=False)

    def key(self):
        return (-round(self.percent, 4), self.differing)

    def matched(self):
        return self.percent >= 100 and self.differing == 0

    def __str__(self):
        return f"{self.percent:.3f}% ({self.differing}/{self.total} rows differ)"


OBJDIFF = ROOT / "build/tools/objdiff-cli"


def parse_objdiff(diff):
    """Score every target-side function in `objdiff-cli diff --format json` output.

    Left is the target (-1). Rows are objdiff's alignment, so one inserted or removed
    instruction costs one row, not every row after it; relocation targets are compared
    by name/address (objdiff's default), so a different literal or symbol is a diff.
    """
    right = diff.get("right", {}).get("symbols", [])
    result = {}
    for symbol in diff.get("left", {}).get("symbols", []):
        if symbol.get("kind") != "SYMBOL_FUNCTION":
            continue
        index = symbol.get("target_symbol")
        ours = right[index] if isinstance(index, int) and index < len(right) else {}
        left_rows, right_rows = symbol.get("instructions", []), ours.get("instructions", [])
        rows = max(len(left_rows), len(right_rows))
        differing = sum(1 for position in range(rows)
                        if (left_rows[position] if position < len(left_rows) else {}).get("diff_kind")
                        or (right_rows[position] if position < len(right_rows) else {}).get("diff_kind")
                        or position >= len(left_rows) or position >= len(right_rows))
        code = "\n".join(row.get("instruction", {}).get("formatted", "") for row in right_rows)
        result[symbol["name"]] = Score(float(symbol.get("match_percent", 0.0) if ours else 0.0), differing, rows,
                                       hashlib.sha256(code.encode()).hexdigest()[:16])
    return result


def objdiff_scores(target_path, object_path, func, timeout=30):
    result = run_process([str(OBJDIFF), "diff", "-1", str(target_path), "-2", str(object_path), func,
                          "-o", "-", "--format", "json"], timeout)
    if result.returncode:
        raise RuntimeError("objdiff diff failed: " + result.stderr.decode("utf-8", "replace")[-2000:])
    return parse_objdiff(json.loads(result.stdout))


def same_as_target(ours, target):
    """The target's bytes, allowing the extracted section's trailing alignment zeros (e.g. 76 vs 80 bytes)."""
    if ours is None or target is None:
        return False
    padding = target[len(ours):]
    return target[:len(ours)] == ours and len(padding) < 32 and not any(padding)


def acceptable(baseline_scores, baseline_image, candidate_scores, candidate_image, func, target_image=None):
    """Each data section must equal the input's or the target's; siblings must not lose objdiff percent."""
    target_data = target_image.data if target_image else {}
    for name in set(baseline_image.data) | set(candidate_image.data):
        ours, before = candidate_image.data.get(name), baseline_image.data.get(name)
        if ours != before and not same_as_target(ours, target_data.get(name)):
            return "data changed"
    if set(candidate_image.functions) != set(baseline_image.functions):
        return "function set changed"
    for name, score in baseline_scores.items():
        if name != func and round(candidate_scores[name].percent, 4) < round(score.percent, 4):
            return f"sibling regressed: {name}"
    return None


@dataclasses.dataclass(frozen=True)
class CompileCommand:
    argv: tuple
    original_source: str
    compiler: str
    extab_padding: str = ""

    def for_paths(self, source, output):
        argv = list(self.argv)
        argv[argv.index("-c") + 1] = str(source)
        argv[argv.index("-o") + 1] = str(output)
        return [argument for argument in argv if argument != "-MMD"]

    def preprocess(self, source):
        argv = list(self.argv)
        output_index = argv.index("-o")
        del argv[output_index:output_index + 2]
        compile_index = argv.index("-c")
        argv[compile_index:compile_index + 2] = ["-E", str(source)]
        return [argument for argument in argv if argument != "-MMD"]

    def flags(self):
        compiler_index = next(index for index, argument in enumerate(self.argv) if argument.endswith("mwcceppc.exe"))
        return self.argv[compiler_index + 1:self.argv.index("-c")]

    def defines(self):
        result = {}
        for flag in self.flags():
            if flag.startswith("-D"):
                name, separator, value = flag[2:].partition("=")
                result[name] = value if separator else "1"
        return result


def compile_command(unit, root=ROOT):
    resolution = perm_unit.resolve(unit, str(root))
    ninja_text = (root / "build.ninja").read_text()
    perm_unit.cflags(ninja_text, resolution['name'], resolution['mod'])
    ninja = os.environ.get("NINJA") or shutil.which("ninja") or str(Path.home() / "mgba-venv/bin/ninja")
    candidates = [f"build/G8MJ01/src/{resolution['name']}.o"]
    if resolution['mod']:
        candidates.append(f"build/G8MJ01/src/rel/{resolution['mod']}/{resolution['name']}.o")
    for target in candidates:
        result = subprocess.run([ninja, "-t", "commands", target], cwd=root, capture_output=True, text=True, timeout=30)
        for line in result.stdout.splitlines():
            argv = shlex.split(line.partition(" || ")[0])
            if "-c" not in argv or argv[argv.index("-c") + 1] != resolution['src']:
                continue
            compiler = next((argument for argument in argv if argument.endswith("mwcceppc.exe")), None)
            if not compiler:
                continue
            padding = ""
            remainder = shlex.split(line)
            if "--padding" in remainder:
                padding = remainder[remainder.index("--padding") + 1]
            return resolution, CompileCommand(tuple(argv), resolution['src'], compiler, padding)
    raise LookupError(f"{unit}: expanded Ninja MWCC command not found")


def run_process(argv, timeout):
    return subprocess.run(["timeout", "-k", "1", str(max(0.01, timeout)), *argv], cwd=ROOT,
                          capture_output=True, env={**os.environ, "RAYON_NUM_THREADS": "1"}, timeout=timeout + 3)


def compile_source(command, source, directory, timeout=30):
    input_path = Path(directory) / Path(command.original_source).name
    object_path = input_path.with_suffix(".o")
    input_path.write_text(source, encoding="utf-8", errors="surrogateescape")
    result = run_process(command.for_paths(input_path, object_path), timeout)
    if result.returncode or not object_path.exists():
        diagnostic = (result.stdout + result.stderr).decode("utf-8", "replace")[-2000:]
        detail = "; SIGSYS (sandbox disallows a wibo syscall)" if result.returncode in {159, -31} else ""
        raise RuntimeError(f"compile failed (exit {result.returncode}{detail}): " + diagnostic)
    if command.extab_padding:
        result = run_process([str(ROOT / "build/tools/dtk"), "extab", "clean", "--padding", command.extab_padding,
                              str(object_path), str(object_path)], timeout)
        if result.returncode:
            raise RuntimeError("extab cleanup failed")
    return object_path


def write_bundle(command, target_path, source, func, output):
    with tempfile.TemporaryDirectory(prefix="mwsearch-bundle-") as directory:
        asm_path = Path(directory) / "target.s"
        result = run_process([str(ROOT / "build/tools/dtk"), "elf", "disasm", str(target_path), str(asm_path)], 30)
        if result.returncode:
            raise RuntimeError("target disassembly failed")
        lines = asm_path.read_text().splitlines(keepends=True)
        start = next((index for index, line in enumerate(lines) if line.startswith(f".fn {func},")), None)
        if start is None:
            raise ValueError(f"{func} missing from target assembly")
        end = next(index for index in range(start, len(lines)) if lines[index].startswith(".endfn"))
        target_asm = "".join(lines[start:end + 1])
        input_path = Path(directory) / Path(command.original_source).name
        input_path.write_text(source, encoding="utf-8", errors="surrogateescape")
        result = run_process(command.preprocess(input_path), 30)
        if result.returncode:
            raise RuntimeError("MWCC preprocessing failed: " + result.stderr.decode("utf-8", "replace")[-2000:])
        preprocessed = result.stdout.decode("utf-8", "surrogateescape")
        tokens = tokenize(preprocessed)
        function = find_function(tokens, func)
        start, end = tokens[function.start].start, tokens[function.closing].end
        prototype = preprocessed[start:tokens[function.opening].start].rstrip() + ";"
        context = preprocessed[:start] + prototype + preprocessed[end:]
        function_source = preprocessed[start:end] + "\n"
    compiler_ids = {"GC/2.6": "mwcc_247_107"}
    version = str(Path(command.compiler).parent).removeprefix("build/compilers/")
    compiler_id = compiler_ids.get(version, version)
    flags = (f"compiler: {compiler_id}\nflags: {shlex.join(command.flags())}\n"
             f"command: {shlex.join(command.argv)}\n")
    for name, contents in {"target.s": target_asm, "ctx.c": context, "src.c": function_source, "flags.txt": flags}.items():
        (output / name).write_text(contents, encoding="utf-8", errors="surrogateescape")
    print(f"bundle: {output}", flush=True)


def as_json(scores):
    return {name: {"percent": score.percent, "differing": score.differing, "total": score.total}
            for name, score in scores.items()}


@dataclasses.dataclass(frozen=True)
class Result:
    description: str
    source: str
    scores: dict = dataclasses.field(default_factory=dict)
    error: str = ""
    stall: int = 0
    digest: str = ""

    def signature(self, func):
        """The function's generated code (objdiff's formatted rows), or the source when that is unknown."""
        score = self.scores.get(func)
        return (score.signature if score else "") or self.digest or normalized_hash(self.source)


def select_beam(main, buckets, func, beam_width, explore, expanded):
    """Next generation's parents: the best-or-tied `main` entries with distinct code, unexpanded code
    first, then up to `explore` plateau-crossing entries taken round-robin across parents.

    `buckets` maps each parent (ordered best first) to children that did not lose ground against it
    and generate code no frontier has expanded; one child per parent per round keeps each line of
    descent alive instead of letting the currently best parent fill every slot.
    """
    chosen, signatures = [], set()
    for entry in sorted(main, key=lambda item: (item.scores[func].key(), item.signature(func) in expanded,
                                                item.digest)):
        if len(chosen) < beam_width and entry.signature(func) not in signatures:
            chosen.append(entry)
            signatures.add(entry.signature(func))
    queues = [sorted(children, key=lambda item: (item.scores[func].key(), item.stall, item.digest))
              for children in buckets]
    explored = 0
    while explore and explored < explore and any(queues):
        for queue in queues:
            while queue and (queue[0].signature(func) in signatures or queue[0].signature(func) in expanded):
                queue.pop(0)
            if queue and explored < explore:
                entry = queue.pop(0)
                chosen.append(entry)
                signatures.add(entry.signature(func))
                explored += 1
    return chosen


def search(command, target_path, source, func, jobs, minutes, output, beam_width=4, explore=8, max_stall=2):
    started = time.monotonic()
    deadline = started + minutes * 60
    target = read_object(target_path)
    with tempfile.TemporaryDirectory(prefix="mwsearch-baseline-") as directory:
        object_path = compile_source(command, source, directory)
        baseline = read_object(object_path)
        baseline_scores = objdiff_scores(target_path, object_path, func)
    if func not in baseline.functions or func not in target.functions or func not in baseline_scores:
        raise ValueError(f"{func} missing from baseline or target object")
    best = Result("input", source, baseline_scores, digest=normalized_hash(source))
    stats = {"compiled": 0, "compile_failures": 0, "data_rejections": 0, "sibling_rejections": 0,
             "improvements": 0, "deduplicated": 0, "generations": 0, "explored": 0}
    seen = {normalized_hash(source)}
    beam = [best]
    expanded = {best.signature(func)}
    sibling_names = sorted(name for name in baseline_scores if name != func)
    print(f"baseline {func}: {baseline_scores[func]}", flush=True)
    print("siblings: " + ", ".join(f"{name}={baseline_scores[name].percent:.3f}%" for name in sibling_names), flush=True)
    (output / "input.c").write_text(source, encoding="utf-8", errors="surrogateescape")

    def evaluate(description, candidate_source, digest):
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            return Result(description, candidate_source, error="deadline", digest=digest)
        try:
            with tempfile.TemporaryDirectory(prefix="mwsearch-candidate-") as directory:
                object_path = compile_source(command, candidate_source, directory, min(30, remaining))
                image = read_object(object_path)
                candidate_scores = objdiff_scores(target_path, object_path, func)
                rejected = acceptable(baseline_scores, baseline, candidate_scores, image, func, target)
                return Result(description, candidate_source, candidate_scores, rejected or "", digest=digest)
        except (RuntimeError, ValueError, OSError, subprocess.SubprocessError) as error:
            return Result(description, candidate_source, error=str(error), digest=digest)

    def record(result):
        nonlocal best
        best = result
        stats['improvements'] += 1
        score = result.scores[func]
        prefix = f"improvement-{stats['improvements']:03d}-{score.percent:07.3f}"
        patch = "".join(difflib.unified_diff(source.splitlines(keepends=True), result.source.splitlines(keepends=True),
                                           fromfile="a/" + command.original_source, tofile="b/" + command.original_source))
        (output / (prefix + ".diff")).write_text(patch, encoding="utf-8", errors="surrogateescape")
        (output / "best.c").write_text(result.source, encoding="utf-8", errors="surrogateescape")
        (output / (prefix + ".json")).write_text(json.dumps({"mutation": result.description, "scores": as_json(result.scores),
                                                            "elapsed_seconds": time.monotonic() - started}, indent=2) + "\n")
        print(f"improvement {stats['improvements']}: {score} "
              f"{result.description}; {stats['compiled']} candidates; {time.monotonic() - started:.1f}s; {prefix}.diff", flush=True)

    if not best.scores[func].matched():
        with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as executor:
            while beam and time.monotonic() < deadline and not best.scores[func].matched():
                stats['generations'] += 1
                main = {}
                buckets = [[] for _ in beam]
                pending = {}
                candidates = iter((slot, item) for slot, parent in enumerate(beam)
                                  for item in mutations(parent.source, func, command.defines()))
                exhausted = False
                while (not exhausted or pending) and time.monotonic() < deadline:
                    while not exhausted and len(pending) < jobs:
                        try:
                            slot, (description, candidate_source) = next(candidates)
                        except StopIteration:
                            exhausted = True
                            break
                        digest = normalized_hash(candidate_source)
                        if digest in seen:
                            stats['deduplicated'] += 1
                            continue
                        seen.add(digest)
                        pending[executor.submit(evaluate, description, candidate_source, digest)] = slot
                    if not pending:
                        break
                    completed, _ = concurrent.futures.wait(pending, timeout=max(0.01, deadline - time.monotonic()),
                                                            return_when=concurrent.futures.FIRST_COMPLETED)
                    for future in completed:
                        slot = pending.pop(future)
                        result = future.result()
                        stats['compiled'] += 1
                        if result.error:
                            if result.error == "data changed":
                                stats['data_rejections'] += 1
                            elif result.error.startswith("sibling regressed") or result.error == "function set changed":
                                stats['sibling_rejections'] += 1
                            else:
                                stats['compile_failures'] += 1
                            continue
                        parent = beam[slot]
                        key, parent_key = result.scores[func].key(), parent.scores[func].key()
                        result = dataclasses.replace(result, stall=0 if key < parent_key else parent.stall + 1)
                        if key < best.scores[func].key():
                            record(result)
                        if key <= best.scores[func].key():
                            signature = result.signature(func)
                            if signature not in main or result.digest < main[signature].digest:
                                main[signature] = result
                            ranked = sorted(main.values(), key=lambda item: (item.scores[func].key(), item.signature(func) in expanded,
                                                                            item.digest))
                            main = {item.signature(func): item for item in ranked[:beam_width * 2]}
                        if (key <= parent_key and result.stall <= max_stall and result.signature(func) != parent.signature(func)
                                and result.signature(func) not in expanded):
                            bucket = buckets[slot]
                            bucket.append(result)
                            bucket.sort(key=lambda item: (item.scores[func].key(), item.stall, item.digest))
                            del bucket[max(1, explore):]
                    if best.scores[func].matched():
                        break
                for future in pending:
                    future.cancel()
                order = sorted(range(len(beam)), key=lambda index: beam[index].scores[func].key())
                main_entries = [entry for entry in main.values() if entry.scores[func].key() <= best.scores[func].key()]
                beam = select_beam(main_entries, [buckets[index] for index in order], func, beam_width, explore, expanded)
                main_digests = {entry.digest for entry in main_entries}
                stats['explored'] += sum(1 for entry in beam if entry.digest not in main_digests)
                expanded |= {entry.signature(func) for entry in beam}
    summary = {"function": func, "baseline": as_json(baseline_scores), "best": as_json(best.scores), "stats": stats,
               "elapsed_seconds": time.monotonic() - started, "jobs": jobs, "minutes": minutes,
               "stop_reason": "match" if best.scores[func].matched() else "deadline" if time.monotonic() >= deadline else "exhausted",
               "score": "objdiff-cli diff match_percent (aligned rows; relocations by name/address)",
               "data_gate": "each data section equals the input's or the target's",
               "beam": beam_width, "explore": explore, "max_stall": max_stall}
    (output / "results.json").write_text(json.dumps(summary, indent=2) + "\n")
    print(f"finished: {best.scores[func]}; {stats}; {summary['stop_reason']}", flush=True)
    return summary


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--unit", required=True, help="perm_unit name, e.g. event/evt_shop or MOD/path")
    parser.add_argument("--func", required=True)
    parser.add_argument("--src", type=Path, help="alternate whole translation unit (never modified)")
    parser.add_argument("-j", type=int, default=8, help="worker count, 1..12 (default 8)")
    parser.add_argument("--minutes", type=float, default=15)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--beam", type=int, default=4, help="equal-or-better frontier width (default 4)")
    parser.add_argument("--explore", type=int, default=8,
                        help="extra parents per generation that kept their parent's score but changed its code (default 8)")
    parser.add_argument("--stall", type=int, default=2,
                        help="non-improving steps an exploring line may take in a row (default 2)")
    parser.add_argument("--bundle", action="store_true", help="write scratch bundle instead of searching")
    args = parser.parse_args(argv)
    if not 1 <= args.j <= 12:
        parser.error("-j must be between 1 and 12")
    if not math.isfinite(args.minutes) or args.minutes <= 0 or args.beam < 1 or args.explore < 0 or args.stall < 0:
        parser.error("--minutes must be finite and positive; --beam must be positive; --explore/--stall nonnegative")
    try:
        resolution, command = compile_command(args.unit)
        source_path = (args.src or ROOT / resolution['src']).resolve()
        source = source_path.read_text(encoding="utf-8", errors="surrogateescape")
        find_function(active_tokens(source, command.defines()), args.func)
        output = args.out.resolve()
        forbidden = [ROOT / directory for directory in ("src", "rels", ".git", "build")]
        if any(output == directory or directory in output.parents for directory in forbidden):
            parser.error("--out must not be inside src/, rels/, .git/, or build/")
        output.mkdir(parents=True, exist_ok=True)
        if any(output.iterdir()):
            parser.error("--out must be empty (previous results are never overwritten)")
        target_path = ROOT / resolution['target']
        if args.bundle:
            write_bundle(command, target_path, source, args.func, output)
        else:
            search(command, target_path, source, args.func, args.j, args.minutes, output, args.beam, args.explore, args.stall)
    except (ValueError, LookupError, RuntimeError, OSError, subprocess.SubprocessError) as error:
        parser.exit(1, f"mwsearch: {error}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
