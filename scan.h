

#ifndef INTERPRETER_SCAN_H
#define INTERPRETER_SCAN_H

#include <stdio.h>

enum TokenType {LPAREN=0, RPAREN, ID, NUM, OP, DONE, ERROR};

enum {MAXLENGTH = 101};

struct Token {
    enum TokenType tokenType;
    char lexeme[MAXLENGTH];
};

struct Token scan(FILE *in);

void printToken(struct Token t);


#endif //INTERPRETER_SCAN_H
