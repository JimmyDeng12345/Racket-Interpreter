#ifndef INTERPRETER_PARSESEXP_H
#define INTERPRETER_PARSESEXP_H

#include <stdio.h>
#include "scan.h"

enum SExpType {ATOM = 0, LIST, BADSEXP};

struct SExp {
    enum SExpType type;
    union {
        struct Token atom;
        struct SExpListNode *list;
    };
};

struct SExpListNode {
    struct SExp first;
    struct SExpListNode *rest;
};

struct SExp parseSExp(FILE *in);

void printSExp(struct SExp s);
void freeSExp (struct SExp s);

#endif //INTERPRETER_PARSESEXP_H
