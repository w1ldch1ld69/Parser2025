#pragma once

#include "tokens.h"

typedef enum {
    BINARY,
    UNARY,
    NUM,
    VAR,
    FUNC,
    GROUP
} NodeType;

typedef enum {
    SUM, SUB, MUL, DIV, POW, POS, NEG
} OperationType;

typedef struct Node {
    NodeType type;

    union {

        struct {
            OperationType type;
            const char *oper;
            struct Node *left, *right;
        } binary;

        struct {
            OperationType type;
            const char *oper;
            struct Node *operand;
        } unary;

        double number;

        const char *var;

        struct {
            const char *name;
            size_cdz count;
            struct Node **arguments;
        } func;

        struct Node *group;

    };

} Node;

Node*
createNode(NodeType type) {
    Node *tmp = (Node *) malloc(sizeof(Node));
    tmp -> type = type;
    return tmp;
}

Node*
createBinary(TokenType type, const char *oper, Node *left, Node *right) {
    Node *bin = createNode(BINARY);
    switch (type) {
        case TOKEN_PLUS:
            bin -> binary.type = SUM; break;
        case TOKEN_MINUS:
            bin -> binary.type = SUB; break;
        case TOKEN_STAR:
            bin -> binary.type = MUL; break;
        case TOKEN_SLASH:
            bin -> binary.type = DIV; break;
        case TOKEN_CARET: 
            bin -> binary.type = POW; break;
        default:;
    }
    bin -> binary.oper = oper;
    bin -> binary.left = left;
    bin -> binary.right = right; 
    return bin;
}

Node*
createUnary(TokenType type, const char *oper, Node *operand) {
    Node *un = createNode(UNARY);
    switch (type) {
        case TOKEN_PLUS:
            un -> unary.type = POS; break;
        case TOKEN_MINUS: 
            un -> unary.type = NEG; break;
        default:;
    }
    un -> unary.oper = oper;
    un -> unary.operand = operand;
    return un;
}

Node*
createNumber(const char *number) {
    Node *num = createNode(NUM);
    num -> number = atof(number); 
    return num;
}

Node*
createWord(const char *name) {
    Node *var = createNode(VAR);
    var -> var = name;
    return var;
}

Node*
createFunction(const char *name, size_cdz argCount, Node **arguments) {
    Node *function = createNode(FUNC);
    function -> func.name = name;
    function -> func.count = argCount;
    function -> func.arguments = arguments;
    return function;
}