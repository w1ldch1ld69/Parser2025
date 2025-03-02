#pragma once

#include "ToParse.h"

Node *parse(const Token *);
Node *parseSum(const Token *, size_cdz *);
Node *parseMul(const Token *, size_cdz *);
Node *parsePow(const Token *, size_cdz *);
Node *parseUnary(const Token *, size_cdz *);
Node *parseBase(const Token *, size_cdz *);
Node *parseNum(const Token *, size_cdz *);
Node *parseVar(const Token *, size_cdz *);
Node *parseGroup(const Token *, size_cdz *);
Node *parseFunc(const char *, const Token *, size_cdz *);



Node* 
parse(const Token *tokens) {
    size_cdz index = 0;
    return parseSum(tokens, &index);
}

Node*
parseSum(const Token *tokens, size_cdz *index) {
    Node *left = parseMul(tokens, index);
    while (tokens[*index].type == TOKEN_PLUS || tokens[*index].type == TOKEN_MINUS) {
        Token op = tokens[*index];
        ++*index;
        Node *right = parseMul(tokens, index);
        left = createBinary(op.type, op.value, left, right);
    }
    return left;
}

Node*
parseMul(const Token *tokens, size_cdz *index) {
    Node *left = parsePow(tokens, index);
    while (tokens[*index].type == TOKEN_STAR || tokens[*index].type == TOKEN_SLASH) {
        Token op = tokens[*index];
        ++*index;
        Node *right = parsePow(tokens, index);
        left = createBinary(op.type, op.value, left, right);
    }
    return left;
}

Node*
parsePow(const Token *tokens, size_cdz *index) {
    Node *left = parseUnary(tokens, index);
    if (tokens[*index].type == TOKEN_CARET) {
        Token op = tokens[*index];
        ++*index;
        Node *right = parsePow(tokens, index);
        left = createBinary(op.type, op.value, left, right);
    }
    return left;
}

Node*
parseUnary(const Token *tokens, size_cdz *index) {
    if (tokens[*index].type == TOKEN_PLUS || tokens[*index].type == TOKEN_MINUS) {
        Token op = tokens[*index];
        ++*index;
        Node *operand = parseUnary(tokens, index);
        return createUnary(op.type, op.value, operand);
    }
    return parseBase(tokens, index);
}

Node*
parseBase(const Token *tokens, size_cdz *index) {
    if (tokens[*index].type == TOKEN_NUM)
        return parseNum(tokens, index);

    if (tokens[*index].type == TOKEN_LPAREN)
        return parseGroup(tokens, index);

    if (tokens[*index].type == TOKEN_ID) {
        const char *name = tokens[*index].value;
        if (tokens[++*index].type == TOKEN_LPAREN) {
            ++*index;
            return parseFunc(name, tokens, index);
        }
        --*index;
        return parseVar(tokens, index);
    }

    fprintf(stderr, "Invalid token: %s\n", tokens[*index].value);
    exit(1);
}

Node*
parseNum(const Token *tokens, size_cdz *index) {
    const char *number = tokens[(*index)++].value;
    return createNumber(number);
}

Node*
parseGroup(const Token *tokens, size_cdz *index) { 
    ++*index;
    Node *group = parseSum(tokens, index);
    if (tokens[*index].type == TOKEN_RPAREN) {
        ++*index;
        return group;
    }
    else {
        fprintf(stderr, "Unexpected token %d, expected %d\n", tokens[*index].type, TOKEN_RPAREN);
        exit(1);
    }
}

Node*
parseVar(const Token *tokens, size_cdz *index) {
    const char *word = tokens[(*index)++].value;
    return createWord(word);
}

Node*
parseFunc(const char *name, const Token *tokens, size_cdz *index) {
    size_cdz n = DEFAULT_ARG_SIZE, i = 0;
    Node **args = (Node **) calloc(n, sizeof(Node *));
    if (tokens[*index].type != TOKEN_RPAREN) {
        do {
            if (i == n - 1) {
                n += DEFAULT_ARG_SIZE;
                args = (Node **) realloc(args, sizeof(Node *) * n);
            }
            args[i] = parseSum(tokens, index);
            ++i;
        } while (tokens[*index].type == TOKEN_COMMA);
        args[i] = NULL;
    }
    if (tokens[*index].type == TOKEN_RPAREN) 
        ++*index;
    else {
        fprintf(stderr, "Unexpected token %d, expected %d\n", tokens[*index].type, TOKEN_RPAREN);
        exit(1);
    }
    return createFunction(name, i, args);
}