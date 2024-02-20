#ifndef KPATCH_SPARSE_H
#define KPATCH_SPARSE_H

#include "expression.h"
#include "parse.h"
#include "scope.h"
#include "symbol.h"

enum scopes {
	SCOPE_NONE = 0,
	SCOPE_FILE = 1,
	SCOPE_GLOBAL = 2,
};

struct analysis_card {
	struct symbol *base;
	enum scopes scope;
	enum type type:8;
	enum namespace namespace:9;
	unsigned long modifiers;
};

#endif
