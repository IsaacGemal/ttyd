#include "parse.h"
#include <string.h>
#include <stdio.h>

//.bss
ParseWork parse;

const char str_8041A6B0[4] = "%d";
const char str_8041A6B4[4] = "%f";
void parseInit(void* ptr, u32 size) {
	parse.ptr = ptr;
	parse.field_0x54 = 0;
	parse.field_0x4 = 0;
	parse.size = size;
}





// Copies the tag starting at src[pos] into dst, dropping whitespace, up to 127 characters or the closing '>'.
static inline void parseTagName(char* dst, s8* src, s32 pos, s32 end) {
	s32 len = 0;

	for (; pos < end; pos++) {
		if (src[pos] != ' ' && src[pos] != '\t' && src[pos] != '\n' && src[pos] != '\r') {
			dst[len] = src[pos];
			len++;
		}
		if (len >= 0x7f || src[pos] == '>') {
			break;
		}
	}
	dst[len] = 0;
}

BOOL parsePush(const char* tag) {
	static char buf[128];
	// field_0x4 and size head two parallel 10-entry position arrays indexed by nesting depth.
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
					parseTagName(buf, base, i, end);
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
	// Resume the parent level at the end of the child tag that was just parsed.
	((s32*)&parse.field_0x4)[parse.field_0x54] = ((s32*)&parse.field_0x30)[parse.field_0x54];
}





BOOL parseGet1Next(s32 type, void* data) {
	s8* src;
	s8* src2;
	s8* q;
	s8 c2;
	s32 result = 1;
	s8* dst;
	s32* p;
	s32 tokenEnd;
	s32 end;
	s8 c;
	s8 ch;
	s8* base;
	s32 k;
	s32 len;
	s32 i;
	s32 tokenStart;
	s32 state;


	switch (type) {
	case 3:
		// Three-component (x, y, z) value: parse each part as a float.
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
			c = base[i];

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
					c2 = base[i];

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
			// String: copy the quoted text, dropping the surrounding quotes.
			k = tokenStart;
			dst = data;
			src = base + tokenStart;
			len = 0;

			while (k < end) {
				if (*src == '"') {
					q = dst;

					k++;
					src2 = base + k;
					src++;
					while (k < end) {
						ch = *src2;
						if (ch == '"') {
							break;
						}
						*q = ch;
						q++;
						dst++;
						len++;
						k++;
						src++;
						src2++;
					}
				}
				k++;
				src++;
			}
			((s8*)data)[len] = 0;
			p[0] = k;
			break;
		}
		case 1:
			// Integer: scanf with "%d".
			sscanf((const char*)&base[tokenStart], str_8041A6B0, data);
			p[0] = tokenEnd;
			break;
		case 2:
			// Float: scanf with "%f".
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
