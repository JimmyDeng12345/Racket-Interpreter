


#ifndef INTERPRETER_PARSEFR_H
#define INTERPRETER_PARSEFR_H
#include <stdio.h>
#include "parsesexp.h"

enum ASTType {FUN = 0, APP, BIN, VAR, NUMBER, BAD};

struct AST;
struct Fun;
struct App;
struct Bin;
struct Num;
struct Var;

struct AST {
    enum ASTType type;
    union {
        struct Fun *func;
        struct App *application;
        struct Bin *bin_op;
        struct Var *v;
        struct Num *n;
    };
};

struct Fun {
    char var[MAXLENGTH];
    struct AST body;
};

struct App {
    struct AST fn;
    struct AST arg;
};

struct Bin {
    char op;
    struct AST arg1, arg2;
};

struct Num {
    int val;
};

struct Var {
    char name[MAXLENGTH];
};

// Produces application Faux Racket AST structure
struct AST parseFR(FILE *in);

// Prints application Faux Racket AST structure
void printFR(struct AST expr);

// Frees application Faux Racket AST structure
void freeFR(struct AST expr);

#endif //INTERPRETER_PARSEFR_H
