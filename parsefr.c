#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parsefr.h"

struct Fun *makeFun(char *name, struct AST body) {
    struct Fun *result = malloc(sizeof(struct Fun));
    strncpy(result->var, name, MAXLENGTH-1);
    result->var[MAXLENGTH-1] = 0;
    result->body = body;
    return result;
}

struct Bin *makeBin(char op, struct AST arg1, struct AST arg2) {
    struct Bin *result = malloc(sizeof(struct Bin));
    result->op = op;
    result->arg1 = arg1;
    result->arg2 = arg2;
    return result;
}

struct App *makeApp(struct AST fn, struct AST arg) {
    struct App *result = malloc(sizeof(struct Bin));
    result->fn = fn;
    result->arg = arg;
    return result;
}

struct Var *makeVar(char *name) {
    struct Var *result = malloc(sizeof(struct Var));
    strncpy(result->name, name, MAXLENGTH-1);
    result->name[MAXLENGTH-1] = 0;
    return result;
}

struct Num *makeNum(int num) {
    struct Num *result = malloc(sizeof(struct Num));
    result->val = num;
    return result;
}

int isKeyword (char *word) {
    return !strcmp(word, "lambda") || !strcmp(word, "let");
}

static int length (struct SExpListNode *n) {
    if (!n) return 0;
    return 1 + length(n->rest);
}

struct AST doParse(struct SExp sexp) {
    struct AST result;
    if (sexp.type == BADSEXP) {
        result.type = BAD;
        return result;
    }
    if (sexp.type == ATOM) {
        struct Token t = sexp.atom;
        if (t.tokenType == ID) {
            if (isKeyword(t.lexeme)) {
                result.type = BAD;
                return result;
            }
            result.type = VAR;
            result.v = makeVar(t.lexeme);
            return result;
        }
        else if (t.tokenType == NUM) {
            result.type = NUMBER;
            int val;
            sscanf(t.lexeme, "%d", &val);
            result.n = makeNum(val);
            return result;
        }
        else {
            result.type = BAD;
            return result;
        }
    }
    if (sexp.type == LIST) {
        int n = length(sexp.list);
        if (n == 0) {
            result.type = BAD;
            return result;
        }
        struct SExp first = sexp.list->first;
        if (first.type == ATOM) {
            struct Token t = first.atom;
            if (t.tokenType == ID) {
                if (!strcmp(t.lexeme, "lambda")) {
                    if (n != 3) {
                        fprintf(stderr, "Ill-formed 'fun' expression\n");
                        result.type = BAD;
                        return result;
                    }
                    struct SExp second = sexp.list->rest->first;
                    if (second.type == ATOM) {
                        fprintf(stderr, "Function must have param in parens\n");
                        result.type = BAD;
                        return result;
                    }
                    if (length(second.list) != 1) {
                        fprintf(stderr, "Function must have exactly one param\n");
                        result.type = BAD;
                        return result;
                    }
                    struct SExp param = second.list->first;
                    if (param.type != ATOM) {
                        fprintf(stderr, "Use of list as function param\n");
                        result.type = BAD;
                        return result;
                    }
                    if (isKeyword(param.atom.lexeme)) {
                        fprintf(stderr,
                                "Use of keyword %s as function param\n", param.atom.lexeme);
                        result.type = BAD;
                        return result;
                    }
                    struct AST body = doParse(sexp.list->rest->rest->first);
                    if (body.type == BAD) {
                        result.type = BAD;
                        return result;
                    }
                    result.type = FUN;
                    result.func = makeFun(param.atom.lexeme, body);
                    return result;
                }
                else if (!strcmp(t.lexeme, "let")) {
                    if (n != 3) {
                        fprintf(stderr, "Ill-formed 'let' expression\n");
                        result.type = BAD;
                        return result;
                    }
                    struct SExp second = sexp.list->rest->first;
                    if (second.type == ATOM) {
                        fprintf(stderr, "'let' requires (name value) binding\n");
                        result.type = BAD;
                        return result;
                    }
                    struct SExpListNode *binding = second.list;
                    if (length(binding) == 1) { // Allow for double parens
                        struct SExp inner = binding->first;
                        if (inner.type != LIST) {
                            fprintf(stderr, "'let' requires (name value) binding\n");
                            result.type = BAD;
                            return result;
                        }
                        binding = inner.list;
                    }
                    if (length(binding) != 2) {
                        fprintf(stderr, "'let' requires (name value) binding\n");
                        result.type = BAD;
                        return result;
                    }
                    struct SExp var = binding->first;
                    if (var.type != ATOM) {
                        fprintf(stderr, "'let' must bind an id\n");
                        result.type = BAD;
                        return result;
                    }
                    struct Token t = var.atom;
                    if (isKeyword(t.lexeme)) {
                        fprintf(stderr, "'let' cannot bind keyword %s\n", t.lexeme);
                        result.type = BAD;
                        return result;
                    }
                    struct AST val = doParse(binding->rest->first);
                    if (val.type == BAD) {
                        fprintf(stderr, "Error in bound value of 'let' expression\n");
                        result.type = BAD;
                        return result;
                    }

                    struct AST body = doParse(sexp.list->rest->rest->first);
                    if (body.type == BAD) {
                        freeFR(val);
                        fprintf(stderr, "Error in body of 'let' expression\n");
                        result.type = BAD;
                        return result;
                    }

                    struct AST fun;
                    fun.type = FUN;
                    fun.func = makeFun(t.lexeme, body);

                    result.type = APP;
                    result.application = makeApp(fun, val);
                    return result;
                }
                else {
                    if (n != 2) {
                        fprintf(stderr, "Lambda function application takes exactly one arg\n");
                        result.type = BAD;
                        return result;
                    }
                    struct AST fn = doParse(sexp.list->first);
                    if (fn.type == BAD) {
                        fprintf(stderr, "Error in function of lambda function application\n");
                        result.type = BAD;
                        return result;
                    }
                    struct AST arg = doParse(sexp.list->rest->first);
                    if (arg.type == BAD) {
                        freeFR(fn);
                        fprintf(stderr, "Error in argument of lambda function application\n");
                        result.type = BAD;
                        return result;
                    }
                    result.type = APP;
                    result.application = makeApp(fn, arg);
                    return result;
                }
            }
            else if (t.tokenType == NUM) {
                fprintf(stderr, "Cannot hava application number in function position\n");
                result.type = BAD;
                return result;
            }
            else if (t.tokenType == OP) {
                if (n != 3) {
                    fprintf(stderr, "Binary operation takes exactly two args\n");
                    result.type = BAD;
                    return result;
                }
                char op = t.lexeme[0];
                struct AST arg1 = doParse(sexp.list->rest->first);
                if (arg1.type == BAD) {
                    fprintf(stderr, "Error in argument 1 of binary operation\n");
                    result.type = BAD;
                    return result;
                }
                struct AST arg2 = doParse(sexp.list->rest->rest->first);
                if (arg2.type == BAD) {
                    freeFR(arg1);
                    fprintf(stderr, "Error in argument 2 of binary operation\n");
                    result.type = BAD;
                    return result;
                }
                result.type = BIN;
                result.bin_op = makeBin(op, arg1, arg2);
                return result;
            }
        }
        else { // first.type == LIST
            if (n != 2) {
                fprintf(stderr, "Lambda function application takes exactly one arg\n");
                result.type = BAD;
                return result;
            }
            struct AST fn = doParse(sexp.list->first);
            if (fn.type == BAD) {
                fprintf(stderr, "Error in function of function application\n");
                result.type = BAD;
                return result;
            }
            struct AST arg = doParse(sexp.list->rest->first);
            if (arg.type == BAD) {
                freeFR(fn);
                fprintf(stderr, "Error in argument of function application\n");
                result.type = BAD;
                return result;
            }
            result.type = APP;
            result.application = makeApp(fn, arg);
            return result;
        }
    }
}

struct AST parseFR(FILE *in) {
    struct SExp sexp = parseSExp(in);
    struct AST result =  doParse(sexp);
    freeSExp(sexp);
    return result;
}

void printFR(struct AST expr) {
    switch(expr.type) {
        case VAR:
            printf("Var(%s)", expr.v->name);
            return;
        case FUN:
            printf("Lambda(%s,", expr.func->var);
            printFR(expr.func->body);
            printf(")");
            return;
        case APP:
            printf("App(");
            printFR(expr.application->fn);
            printf(",");
            printFR(expr.application->arg);
            printf(")");
            return;
        case BIN:
            printf("Bin(%c,", expr.bin_op->op);
            printFR(expr.bin_op->arg1);
            printf(",");
            printFR(expr.bin_op->arg2);
            printf(")");
            return;
        case NUMBER:
            printf("Num(%d)", expr.n->val);
            return;
        case BAD:
            printf("Invalid Faux Racket Expression");
    }
}

void freeFR(struct AST expr) {
    switch(expr.type) {
        case VAR:
            free(expr.v);
            return;
        case FUN:
            freeFR(expr.func->body);
            free(expr.func);
            return;
        case APP:
            freeFR(expr.application->fn);
            freeFR(expr.application->arg);
            free(expr.application);
            return;
        case BIN:
            freeFR(expr.bin_op->arg1);
            freeFR(expr.bin_op->arg2);
            free(expr.bin_op);
            return;
        case NUMBER:
            free(expr.n);
    }
}
