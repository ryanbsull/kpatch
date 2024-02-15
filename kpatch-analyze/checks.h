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

static struct symbol *get_func_base(struct symbol *sym) {
	struct symbol *ret;
	ret = sym;
	while (ret->type != SYM_BASETYPE && ret->ctype.base_type != NULL)
		ret = ret->ctype.base_type;
	return ret;
}

static struct symbol *get_tail_call(struct statement *stmt) {
	struct symbol *ret;
	if (stmt->expression->ctype && stmt->expression->type == EXPR_CALL)
		ret = stmt->expression->ctype;
	else
		ret = NULL;

	return ret;
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
	struct symbol *edited_func, *tail_call;
  //unsigned long tail_call_mod;
	char edited_func_rettype[200], tail_call_rettype[200];
	// check to see if function is being edited
	if (results->namespace == NS_SYMBOL && results->type == SYM_FN) {
		// find return type of edited function
		edited_func = get_func_base(results->base);
		tail_call = get_tail_call(results->ret_stmt);
		if (tail_call == NULL) {
			return 0;
		}
		strcpy(edited_func_rettype, builtin_typename(edited_func));
		strcpy(tail_call_rettype, builtin_typename(tail_call));
		//printf("edited_func RETURN TYPE: %s\n", edited_func_rettype);
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
		if (strcmp(edited_func_rettype, tail_call_rettype) == 0 && edited_func->label_modifiers == tail_call->label_modifiers) {
			show_changed_symbol(pos, name);
			printf("REPORT: patch changes a function featuring a tail call, this could trigger\n"
			       "sibling call optimizations that could interfere with building a kpatch be sure\n"
						 "to add __attribute__((fno-optimize-sibling-calls)) to the function definition.\n\n");
			return 1;
		}
	}
	return 0;
}

#endif
