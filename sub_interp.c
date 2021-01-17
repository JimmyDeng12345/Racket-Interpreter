
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsefr.h"

struct AST allSteps(struct AST ast);
struct AST subst(int val, char* fp,struct AST fb);
int main() {
    struct AST expr = parseFR(stdin);
    printFR(expr);
    struct AST result = allSteps(expr);
    printFR(result);
    printf("\n");
    freeFR(result);
    return 0;
}



struct AST oneStep(struct AST ast){
    if(ast.type == BIN){
        if(ast.bin_op->arg1.type == NUMBER && ast.bin_op->arg2.type == NUMBER){
            struct AST result;
            result.type = NUMBER;
            int a1 = ast.bin_op->arg1.n->val;
            int a2 = ast.bin_op->arg2.n->val;
            char op = ast.bin_op->op;
            int ans;
            if(op == '+'){
                ans = a1 + a2;
            }else if (op == '*') {
                ans = a1 * a2;
            }else{
                ans = 3;
            }
            struct Num *numStruct = malloc(sizeof(struct Num));
            numStruct->val = ans;
            result.n = numStruct;
            freeFR(ast);
            return result;
        }else if(ast.bin_op->arg1.type == NUMBER) {
            ast.bin_op->arg2 = oneStep(ast.bin_op->arg2);
            return ast;
        }else{
            ast.bin_op->arg1 = oneStep(ast.bin_op->arg1);
            return ast;
        }
    }else if(ast.type == APP){
        if(ast.application->fn.type == FUN && ast.application->arg.type == NUMBER){
            int val = ast.application->arg.n->val;
            char* fp = strdup(ast.application->fn.func->var);
            struct AST fb = ast.application->fn.func->body;
            free(ast.application->fn.func);
            freeFR(ast.application->arg);
            free(ast.application);
            return subst(val,fp,fb);
        }
        else if(ast.application->fn.type == FUN){
            ast.application->arg =  oneStep(ast.application->arg);
            return ast;
        }else{
            ast.application->fn = oneStep(ast.application->fn);
            return ast;
        }
    }
}


struct AST allSteps(struct AST ast){
    if(ast.type==NUMBER || ast.type==FUN){
        return ast;
    }else{
        return allSteps(oneStep(ast));
    }
}

struct AST subst(int val, char* fp,struct AST fb){
    if(fb.type==NUMBER){
        free(fp);
        return fb;
    }else if(fb.type==VAR){
        if(0==strcmp(fp,fb.v->name)){
            freeFR(fb);
            free(fp);
            struct AST result;
            result.type = NUMBER;
            struct Num * numStruct = malloc(sizeof(struct Num));
            numStruct->val = val;
            result.n = numStruct;
            return result;
        }else{
            free(fp);
            return fb;
        }
    }else if(fb.type==BIN) {
        struct AST arg1 = fb.bin_op->arg1;
        struct AST arg2 = fb.bin_op->arg2;
        char* temp = strdup(fp);
        char* temp2 = strdup(fp);
        fb.bin_op->arg1 = subst(val, temp, arg1);
        fb.bin_op->arg2 = subst(val, temp2, arg2);
        free(fp);
        return fb;
    }else if(fb.type==APP){
        struct AST fn = fb.application->fn;
        struct AST arg = fb.application->arg;
        char* temp = strdup(fp);
        char* temp2 = strdup(fp);
        fb.application->fn = subst(val, temp, fn);
        fb.application->arg = subst(val, temp2, arg);
        free(fp);
        return fb;
    }else if(fb.type==FUN){
        if(0==strcmp(fp,fb.func->var)){
            free(fp);
            return fb;
        }else{
            char* temp = strdup(fp);
            fb.func->body=subst(val, temp, fb.func->body);
            free(fp);
            return fb;
        }
    }else{
        //BAD
        free(fp);
        return fb;
    }
}

