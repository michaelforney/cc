#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "cc.h"

static struct token pending;

void
ppinit(void)
{
	next();
}

static void
keyword(struct token *tok)
{
	static const struct {
		const char *name;
		int value;
	} keywords[] = {
		{"_Alignas",       T_ALIGNAS},
		{"_Alignof",       T_ALIGNOF},
		{"_Atomic",        T_ATOMIC},
		{"_Bool",          T_BOOL},
		{"_Complex",       T_COMPLEX},
		{"_Generic",       T_GENERIC},
		{"_Imaginary",     T_IMAGINARY},
		{"_Noreturn",      T_NORETURN},
		{"_Static_assert", T_STATIC_ASSERT},
		{"_Thread_local",  T_THREAD_LOCAL},
		{"__alignof__",    T_ALIGNOF},
		{"__asm",          T__ASM__},
		{"__asm__",        T__ASM__},
		{"__attribute__",  T__ATTRIBUTE__},
		{"__inline",       TINLINE},
		{"__inline__",     TINLINE},
		{"__signed",       TSIGNED},
		{"__signed__",     TSIGNED},
		{"__thread",       T_THREAD_LOCAL},
		{"__typeof",       T__TYPEOF__},
		{"__typeof__",     T__TYPEOF__},
		{"__volatile__",   TVOLATILE},
		{"auto",           TAUTO},
		{"break",          TBREAK},
		{"case",           TCASE},
		{"char",           TCHAR},
		{"const",          TCONST},
		{"continue",       TCONTINUE},
		{"default",        TDEFAULT},
		{"do",             TDO},
		{"double",         TDOUBLE},
		{"else",           TELSE},
		{"enum",           TENUM},
		{"extern",         TEXTERN},
		{"float",          TFLOAT},
		{"for",            TFOR},
		{"goto",           TGOTO},
		{"if",             TIF},
		{"inline",         TINLINE},
		{"int",            TINT},
		{"long",           TLONG},
		{"register",       TREGISTER},
		{"restrict",       TRESTRICT},
		{"return",         TRETURN},
		{"short",          TSHORT},
		{"signed",         TSIGNED},
		{"sizeof",         TSIZEOF},
		{"static",         TSTATIC},
		{"struct",         TSTRUCT},
		{"switch",         TSWITCH},
		{"typedef",        TTYPEDEF},
		{"union",          TUNION},
		{"unsigned",       TUNSIGNED},
		{"void",           TVOID},
		{"volatile",       TVOLATILE},
		{"while",          TWHILE},
	};
	size_t low = 0, high = LEN(keywords), mid;
	int cmp;

	while (low < high) {
		mid = (low + high) / 2;
		cmp = strcmp(tok->lit, keywords[mid].name);
		if (cmp == 0) {
			free(tok->lit);
			tok->kind = keywords[mid].value;
			tok->lit = NULL;
			break;
		}
		if (cmp < 0)
			high = mid;
		else
			low = mid + 1;
	}
}

static void
nextinto(struct token *t)
{
	do scan(t);
	while (t->kind == TNEWLINE);
	if (t->kind == TIDENT)
		keyword(t);
}

void
next(void)
{
	if (pending.kind) {
		tok = pending;
		pending.kind = TNONE;
	} else {
		nextinto(&tok);
	}
}

bool
peek(int kind)
{
	if (!pending.kind)
		nextinto(&pending);
	if (pending.kind != kind)
		return false;
	pending.kind = TNONE;
	nextinto(&tok);
	return true;
}

char *
expect(enum tokenkind kind, const char *msg)
{
	char *lit, want[64], got[64];

	if (tok.kind != kind) {
		tokendesc(want, sizeof(want), kind, NULL);
		tokendesc(got, sizeof(got), tok.kind, tok.lit);
		error(&tok.loc, "expected %s %s, saw %s", want, msg, got);
	}
	lit = tok.lit;
	next();

	return lit;
}

bool
consume(int kind)
{
	if (tok.kind != kind)
		return false;
	next();
	return true;
}
