#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "parsesexp.h"

static struct SExpListNode BadList;

struct SExpListNode *cons(struct SExp first, struct SExpListNode *rest) {
    struct SExpListNode *result = malloc(sizeof(struct SExpListNode));
    result->first = first;
    result->rest = rest;
    return result;
}

void printSExpList(struct SExpListNode *list);

void printSExp(struct SExp s) {
    if (s.type == BADSEXP) printf("Bad S-expression");
    else if (s.type == ATOM) printToken(s.atom);
    else {
        printf("(");
        printSExpList(s.list);
        printf(")");
    }
}

void printSExpList(struct SExpListNode *list) {
    if (list) {
        printSExp(list->first);
        if (list->rest) printf(" ");
        printSExpList(list->rest);
    }
}

void freeSExpList(struct SExpListNode *list);

void freeSExp (struct SExp s) {
    if (s.type == LIST) {
        freeSExpList(s.list);
    }
}

void freeSExpList(struct SExpListNode *list) {
    if (list) {
        freeSExp(list->first);
        freeSExpList(list->rest);
        free(list);
    }
}

enum TokenType match (enum TokenType left) {
    if (left == LPAREN) return RPAREN;
    else return ERROR;
}

struct SExpListNode *parseSExpList(FILE *in, enum TokenType open) {
    struct Token t = scan(in);
    if (t.tokenType == DONE || t.tokenType == ERROR) {
        return &BadList;
    }
    if (t.tokenType == RPAREN) {
        return (t.tokenType == match(open)) ? NULL : &BadList;
    }
    struct SExp item;
    if (t.tokenType == LPAREN) {
        struct SExpListNode *first = parseSExpList(in, t.tokenType);
        if (first == &BadList) {
            return &BadList;
        }
        item.type = LIST;
        item.list = first;
    }
    if (t.tokenType == ID || t.tokenType == NUM || t.tokenType == OP) {
        item.type = ATOM;
        item.atom = t;
    }
    struct SExpListNode *rest = parseSExpList(in, open);
    return (rest == &BadList) ?  &BadList : cons(item, rest);
}

struct SExp parseSExp(FILE *in) {
    struct Token t = scan(in);
    struct SExp result;
    if (t.tokenType == RPAREN || t.tokenType == DONE
        || t.tokenType == ERROR) {
        result.type = BADSEXP;
        return result;
    }
    if (t.tokenType == ID || t.tokenType == NUM || t.tokenType == OP) {
        result.type = ATOM;
        result.atom = t;
        return result;
    }
    if (t.tokenType == LPAREN) {
        result.type = LIST;
        result.list = parseSExpList(in, t.tokenType);
        if (result.list == &BadList) {
            result.type = BADSEXP;
        }
        return result;
    }
}
