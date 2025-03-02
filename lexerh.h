#pragma once

#include "tokens.h"
#include <ctype.h>

/*

<expr> = <term> | <term> '+'/'-' <expr>
<term> = <factor> | <factor> '*'/'/' <term>
<factor> = <unary> | <unary> '^' <factor>
<unary> = <base> | '+'/'-' <unary>
<base> = <number> | <var> | <group> / <func>

<group> = '(' <expr> ')' | <func>
<func> = <var> '('{<expr>,}*')'

*/

const char based[] = "+-*/^,()";

const char *operators[] = {
    "+", "-", "*", "/", "^", ",", "(", ")"
};

Token 
extractWord(const char *input, size_cdz *index) {
    size_cdz size = 0;
    while (isalpha(input[*index + size])) ++size;

    char *word = strndup(&input[*index], size);
    *index += size;
    Token token = {TOKEN_ID, word};
    return token;
}

Token 
extractNumber(const char *input, size_cdz *index) {
    size_cdz size = 0;
    while (isdigit(input[*index + size])) ++size;

    if (input[*index + size] == '.') {
        ++size;
        while (isdigit(input[*index + size])) ++size;
    }


    char *number = strndup(&input[*index], size);
    *index += size;
    Token token = {TOKEN_NUM, number};
    return token;
}

Token
extractOper(const char *input, size_cdz *index) {
    for (size_cdz i = 0; operators[i] != NULL; ++i) {
        size_cdz len = strlen(operators[i]);
        if (!strncmp(&input[*index], operators[i], len)) {
            TokenType type = (TokenType) i;
            char *op = strndup(&input[*index], len);
            *index += len;
            Token token = {type, op};
            return token;
        }
    }
}

Token 
extract(const char *input, size_cdz *index) {
    while (input[*index] == ' ') ++*index;

    if (isalpha(input[*index])) {
        return extractWord(input, index);
    }

    if (isdigit(input[*index])) {
        return extractNumber(input, index);
    }

    if (strchr(based, input[*index])) {
        return extractOper(input, index);
    }

    fprintf(stderr, "Unknown symbol: '%c'\n", input[*index]);
    exit(1);
}

Token*
tokenize(const char *input) {
    size_cdz index = 0, i = 0, n = DEFAULT_TARRAY_SIZE;
    Token *tokens = (Token *) malloc(n * sizeof(Token));
    while (input[index] != '\0') {
        if (i == n - 1) {
            n += DEFAULT_TARRAY_SIZE;
            tokens = (Token *) realloc(tokens, n * sizeof(Token));
        }
        tokens[i++] = extract(input, &index);
    }
    Token token = {TOKEN_END, ""};
    tokens[i] = token;
    return tokens;
}