#include <stdio.h>
#include <ctype.h>

#include "scan.h"

struct Token scan (FILE *in) {
    struct Token token;
    char c = getc(in);
    while (isspace(c)) {
        c = getc(in);
    }

    if (c == EOF) {
        token.tokenType = DONE;
        return token;
    }
    if (c == '(') {
        token.tokenType = LPAREN;
        sprintf(token.lexeme, "(");
        return token;
    }
    if (c == ')') {
        token.tokenType = RPAREN;
        sprintf(token.lexeme, ")");
        return token;
    }
    if (c == '+' || c == '-' || c == '*' || c == '/') {
        token.tokenType = OP;
        sprintf(token.lexeme, "%c", c);
        return token;
    }
    if (isdigit(c) ) {
        token.tokenType = NUM;
        int tokenLength = 0;
        token.lexeme[tokenLength++] = c;
        token.lexeme[tokenLength] = 0;
        c = getc(in);
        while (isdigit(c)) {
            token.lexeme[tokenLength++] = c;
            c = getc(in);
            if (tokenLength == MAXLENGTH - 1) break;
        }
        token.lexeme[tokenLength] = 0;
        ungetc(c, in);
        return token;
    }
    if (isalpha(c)) {
        token.tokenType = ID;
        int tokenLength = 0;
        while (isalpha(c) || isdigit(c)) {
            token.lexeme[tokenLength++] = c;
            c = getc(in);
            if (tokenLength == MAXLENGTH - 1) break;
        }
        token.lexeme[tokenLength] = 0;
        ungetc(c, in);
        return token;
    }
    token.tokenType = ERROR;
    return token;
}

void printToken(struct Token t) {
    switch (t.tokenType) {
        case LPAREN:
            printf("LPAREN(");
            break;
        case RPAREN:
            printf("RPAREN(");
            break;
        case NUM:
            printf("NUM(");
            break;
        case ID:
            printf("ID(");
            break;
        case OP:
            printf("OP(");
            break;
        case DONE:
            printf("DONE");
            return;
        case ERROR:
            printf("ERROR");
            return;
    }
    printf("%s)", t.lexeme);
}