#include "parse.h"
#include <string.h>
#include <stdio.h>

//.bss
ParseWork parse;

const char str_8041A6B0[] = "%d";
const char str_8041A6B4[] = "%f";
void parseInit(void* ptr, u32 size) {
	parse.ptr = ptr;
	parse.field_0x54 = 0;
	parse.field_0x4 = 0;
	parse.size = size;
}





// NONMATCHING (99.71%): the copy loop's strength-reduced buffer pointer and index use swapped scratch
// registers (mine buf=r4/j=r6, original buf=r6/j=r4); everything else matches. Tested without success:
// u8/s8 buffer, explicit `*(buf+len)`, an explicit `char* bp = buf`, a separate index local, reordered
// declarations, and the opt_lifetimes/opt_common_subs/optimize_for_size pragmas (all leave the r4/r6 swap).
BOOL parsePush(const char* tag) {
	static char buf[128];
	s32 i = ((s32*)&parse.field_0x4)[parse.field_0x54];
	s32 end = ((s32*)&parse.size)[parse.field_0x54];
	s8* base = parse.ptr;
	s32 state;
	s32 contentStart;
	s32 closeEnd;
	s32 depth;
	s32 top;

	depth = 0;
	state = 0;
	for (; i < end; i++) {
		if (base[i] == '<') {
			if (base[i + 1] == '/') {
				depth--;
				if (state == 1 && depth == 0) {
					for (; i < end; i++) {
						if (base[i] == '>') {
							i++;
							break;
						}
					}
					closeEnd = i;
					state = 2;
					break;
				}
			}
			else {
				if (state == 0 && depth == 0) {
					s32 len = 0;
					s32 j;

					for (j = i; j < end; j++) {
						s8 c = base[j];

						if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
							buf[len] = c;
							len++;
						}
						if (len >= 0x7f || base[j] == '>') {
							break;
						}
					}
					buf[len] = 0;
					if (strcmp(buf, tag) == 0) {
						for (; i < end; i++) {
							if (base[i] == '>') {
								break;
							}
						}
						state = 1;
						contentStart = i + 1;
					}
				}
				depth++;
			}
		}
	}

	if (state != 2) {
		return FALSE;
	}
	top = ++parse.field_0x54;
	((s32*)&parse.field_0x4)[top] = contentStart;
	((s32*)&parse.size)[top] = closeEnd;
	return TRUE;
}





// .text:0x320 | 0x80083D8C | size: 0x18
void parsePop(void) {
	parse.field_0x54--;
}





// .text:0x2F8 | 0x80083D64 | size: 0x28
void parsePopNext(void) {
	parse.field_0x54--;
	((s32*)&parse.field_0x4)[parse.field_0x54] = ((s32*)&parse.field_0x30)[parse.field_0x54];
}





// control flow and every instruction shape match; only volatile register names differ.
// The original puts state=r5, tokenStart=r6, the parse base temp (=&parse)=r7, base=r4, i=r10, end=r0; this
// version gets base=r4, i=r10, end=r0 but state=r8, tokenStart=r9, &parse=r5, which cascades into the
// token-scan scratch (r6/r7 vs the original r7/r8/r9) and case 0's i/src/q/src2. Moving the `state`
// declaration to the front (the declaration order here) keeps base/i/end right and swaps state/tokenStart
// between r8/r9; no other order of the six default locals (240 permutations tried) beats this. Making
// `state` born at entry (`s32 state = 0;` at the declaration) does give state=r5 and &parse=r7, and the scan
// scratch then lands in r6/r7 with i=r10, but the compiler hoists the `li r5,0` to the entry and moves base
// to r6 and i/end to r4 (96.40%). Every default-block assignment order, each opt_* / optimize_for_size /
// scheduling pragma, and the named-`idx`/comma/default-scope forms leave the same allocator choice: the
// colouring wants the `&parse` temp to take r5 before `state`. A permuter job.
// NONMATCHING (97.32%): only the volatile register assignment above still differs.
BOOL parseGet1Next(s32 type, void* data) {
	s32 state;
	s32 tokenStart;
	s32 result = 1;
	s32* p;
	s32 i;
	s32 end;
	s8* base;
	s32 tokenEnd;

	switch (type) {
	case 3:
		result *= parseGet1Next(2, data);
		result *= parseGet1Next(2, (s8*)data + 4);
		result *= parseGet1Next(2, (s8*)data + 8);
		return result;
	default: {
		p = &((s32*)&parse.field_0x4)[parse.field_0x54];
		i = p[0];
		end = ((s32*)&parse.size)[parse.field_0x54];
		base = parse.ptr;
		state = 0;

		while (i < end) {
			s8 c = base[i];

			if (c == '/' && base[i + 1] == '/') {
				for (; i < end; i++) {
					if (base[i] == '/n') {
						break;
					}
				}
			}
			else if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
				tokenStart = i;
				state = 1;
				i++;
				for (; i < end; i++) {
					s8 c2 = base[i];

					if (c2 == ' ' || (u8)(c2 - 9) <= 1 || c2 == '\r' ||
					    (c2 == '/' && base[i + 1] == '/')) {
						tokenEnd = i;
						state = 2;
						break;
					}
				}
				break;
			}
			i++;
		}

		if (state <= 1) {
			return FALSE;
		}

		switch (type) {
		case 0: {
			s8* src = base + tokenStart;
			s8* dst = data;
			s32 len = 0;

			i = tokenStart;
			while (i < end) {
				if (*src == '"') {
					s8* q = dst;
					s8* src2;

					i++;
					src2 = base + i;
					src++;
					for (; i < end; i++) {
						if (*src2 == '"') {
							break;
						}
						*q = *src2;
						q++;
						dst++;
						len++;
						src++;
						src2++;
					}
				}
				i++;
				src++;
			}
			((s8*)data)[len] = 0;
			p[0] = i;
			break;
		}
		case 1:
			sscanf((const char*)&base[tokenStart], str_8041A6B0, data);
			p[0] = tokenEnd;
			break;
		case 2:
			sscanf((const char*)&base[tokenStart], str_8041A6B4, data);
			p[0] = tokenEnd;
			break;
		default:
			break;
		}
		return TRUE;
	}
	}
}





// .text:0x0 | 0x80083A6C | size: 0x68
BOOL parseTagGet1(const char* tag, s32 type, void* data) {
	if (parsePush(tag)) {
		BOOL result = parseGet1Next(type, data);

		parse.field_0x54--;
		return result;
	}
	return FALSE;
}
