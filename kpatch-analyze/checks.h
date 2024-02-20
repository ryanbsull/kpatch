#ifndef CHECKS_H
#define CHECKS_H

#include <stdio.h>
#include <string.h>
#include "expression.h"
#include "parse.h"
#include "scope.h"
#include "symbol.h"
#include "kpatch-sparse.h"

static void show_changed_symbol(struct position pos, const char *name)
{
	if (name == NULL)
		name = "<noname>";
	printf("%s:%d:%d change at '%s':\n", stream_name(pos.stream),
	        pos.line, pos.pos, name);
}

int check_notrace(struct position pos, const char *name, struct analysis_card *results)
{
	if (results->namespace == NS_SYMBOL && results->type == SYM_FN &&
	    results->modifiers & MOD_NOTRACE) {
		show_changed_symbol(pos, name);
		printf("REPORT: patch changes code in a function with a 'notrace' modifier.\n"
		       "Livepatch cannot apply changes to those functions. A workaround is needed.\n\n");
		return 1;
	}
	return 0;
}

int check_init(struct position pos, const char *name, struct analysis_card *results)
{
	if (results->namespace == NS_SYMBOL && results->type == SYM_FN &&
	    results->modifiers & MOD_INIT) {
		show_changed_symbol(pos, name);
		printf("REPORT: patch changes code in a function with a 'init' modifier.\n"
		       "Livepatch cannot apply changes to those functions. A workaround is needed.\n\n");
		return 1;
	}
	return 0;
}

int check_global_changes(struct position pos, const char *name, struct analysis_card *results)
{
	if (results->namespace != NS_SYMBOL || (results->namespace == NS_SYMBOL &&
	    results->type != SYM_FN)) {
		show_changed_symbol(pos, name);
		printf("REPORT: patch changes data structures or definitions outside functions which\n"
		       "cannot be patched by Livepatch. A workaround like shadow variables is needed.\n\n");
		return 1;
	}
	return 0;
}

int check_sibling_call(struct position pos, const char *name, struct analysis_card *results)
{
	struct symbol *edited_func;
	char edited_func_rettype[200];
	// check to see if function is being edited
	if (results->namespace == NS_SYMBOL && results->type == SYM_FN) {
		// find return type of edited function
		edited_func = results->base;
		while(edited_func->type != SYM_BASETYPE && edited_func->ctype.base_type != NULL)
			edited_func = edited_func->ctype.base_type;
		strcpy(edited_func_rettype, builtin_typename(edited_func));
		printf("edited_func RETURN TYPE: %s\n", edited_func_rettype);
		/*
		 * Check return statement(s) of function: (for each return statement)
		 *    1. return function(...)
		 *       - make sure function matches return type, if so then warn for possible sibling call error
		 *    2. return var
		 *       - check if var is storing function o/p, if so then repeat #1 check
		 *    3. return 1 (scalar / set value)
		 *       - do nothing
		 * If either #1 or #2 == true then exit loop and throw error
		 */
		printf("TODO: trace from return statement back to func call and check that return type\n\n"); // check to see if there is 
		show_changed_symbol(pos, name);
		return 1;
	}
	return 0;
}

#endif
