#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "size_cdz.h"

typedef enum {
    TOKEN_PLUS, // +
    TOKEN_MINUS, // -
    TOKEN_STAR, // *
    TOKEN_SLASH, // /
    TOKEN_CARET, // ^
    TOKEN_COMMA, // ,
    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )


    TOKEN_NUM,
    TOKEN_ID,
    TOKEN_END
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;