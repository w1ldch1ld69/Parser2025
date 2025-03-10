#include "parserh.h"
#include "lexerh.h"

double
checkVariable(const char *name, variable *vars, size_cdz size) {
    for (int i = 0; i < sizeof(constants) / sizeof(variable); ++i) {
        if (!strcmp(name, constants[i].name)) {
            return constants[i].value;
        }
    }

    for (size_cdz i = 0; i < size; ++i) {
        if (!strcmp(name, vars[i].name)) {
            return vars[i].value;
        }
    }

    fprintf(stderr, "Unknown variable\n");
    exit(1);
}

double 
checkFunction(const char *name, size_cdz count, double *args, function *funcs, size_cdz size) {
    for (size_t i = 0; i < sizeof(systemFuncs) / sizeof(function); ++i) {
        if (strcmp(name, systemFuncs[i].name) == 0) {
            if (systemFuncs[i].argCntr != count) {
                fprintf(stderr, "Invalid number of arguments\n");
                exit(1);
            }
            return systemFuncs[i].f(args);
        }
    }

    for (size_cdz i = 0; i < size; ++i) {
        if (strcmp(name, funcs[i].name) == 0) {
            if (funcs[i].argCntr != count) {
                fprintf(stderr, "Invalid number of arguments\n");
                exit(1);
            }
            return funcs[i].f(args);
        }
    }

    fprintf(stderr, "Unknown function\n");
    exit(1);
}

double 
eval(Node *root, variable *vars, size_cdz varsize, function *funcs, size_cdz funcsize) {
    switch (root->type) {
        case BINARY:
            double left = eval(root->binary.left, vars, varsize, funcs, funcsize);
            double right = eval(root->binary.right, vars, varsize, funcs, funcsize);
            switch (root->binary.type) {
                case SUM:
                    return left + right;
                case SUB:
                    return left - right;
                case MUL:
                    return left * right;
                case DIV:
                    return left / right;
                case POW:
                    double args[] = {left, right};
                    return pow(args);
            }
        case UNARY:
            double operand = eval(root->unary.operand, vars, varsize, funcs, funcsize);
            if (root->unary.type == NEG) {
                return -operand;
            }
            return operand;
        case NUM:
            return root->number;
        case VAR:
            return checkVariable(root->var, vars, varsize);
        case GROUP:
            return eval(root->group, vars, varsize, funcs, funcsize);
        case FUNC:
            double args[root->func.count];
            for (size_t i = 0; i < root->func.count; ++i) {
                args[i] = eval(root->func.arguments[i], vars, varsize, funcs, funcsize);
            }
            return checkFunction(root->func.name, root->func.count, args, funcs, funcsize);
    }
}

void
printTree(Node *root) {
    switch (root->type) {
        case BINARY:
            printf("(");
            printTree(root->binary.left);
            printf(" %s ", root->binary.oper);
            printTree(root->binary.right);
            printf(")");
            break;
        case UNARY:
            printf("%s(", root->unary.oper);
            printTree(root->unary.operand);
            printf(")");
            break;
        case NUM:
            printf("%g", root->number);
            break;
        case VAR:
            printf("%s", root->var);
            break;
        case FUNC:
            printf("%s(", root->func.name);
            for (size_t i = 0; i < root->func.count; ++i) {
                printTree(root->func.arguments[i]);
                if (i != root->func.count - 1) {
                    printf(", ");
                }
            }
            printf(")");
            break;
        case GROUP:
            printf("(");
            printTree(root->group);
            printf(")");
            break;
    }
}

Node*
derivative(Node *root, char *derArg) {
    switch(root -> type) {
        case BINARY:
            Node *left = derivative(root -> binary.left, derArg);
            Node *right = derivative(root -> binary.right, derArg);
            Node *der;
            switch (root -> binary.type) {
                case SUM:
                    der = createBinary(TOKEN_PLUS, "+", left, right);
                    return der;
                case SUB:
                    der = createBinary(TOKEN_MINUS, "-", left, right);
                    return der;
                case MUL:
                    Node *llMul = root -> binary.left;
                    Node *lrMul = right;
                    Node *rlMul = left;
                    Node *rrMul = root -> binary.right;
                    der = createBinary(TOKEN_PLUS, "+", createBinary(TOKEN_STAR, "*", llMul, lrMul), createBinary(TOKEN_STAR, "*", rlMul, rrMul));
                    return der;
                case DIV:
                    Node *llDiv = root -> binary.left;
                    Node *lrDiv = right;
                    Node *rlDiv = left;
                    Node *rrDiv = root -> binary.right;
                    der = createBinary(TOKEN_SLASH, "/", createBinary(TOKEN_MINUS, "-", createBinary(TOKEN_STAR, "*", llDiv, lrDiv), createBinary(TOKEN_STAR, "*", rlDiv, rrDiv)), createBinary(TOKEN_CARET, "^", rrDiv, createNumber("2")));
                    return der;
                case POW:
                    if (root -> binary.left -> type == NUM) {
                        if (root -> binary.right -> type == NUM) {
                            der = createNumber("0");
                            return der;
                        } else {
                            Node **argss = malloc(1 * sizeof(Node *));
                            argss[0] = root -> binary.left;
                            der = createBinary(TOKEN_STAR, "*", createBinary(TOKEN_STAR, "*", right, createFunction("log", 1, argss)), root);
                            return der;
                        }
                    } else {
                        if (root -> binary.right -> type == NUM) {
                            char number[32];
                            sprintf(number, "%g", root -> binary.right -> number - 1);
                            der = createBinary(TOKEN_STAR, "*", root -> binary.right, createBinary(TOKEN_STAR, "*", left, createBinary(TOKEN_CARET, "^", root -> binary.left, createNumber(number))));
                            return der;
                        }
                        else {
                            Node **args = malloc(1 * sizeof(Node *));
                            args[0] = root -> binary.left;
                            der = createBinary(TOKEN_CARET, "^", createNumber("e"), createBinary(TOKEN_STAR, "*", root -> binary.right, createFunction("log", 1, args)));
                            return der;
                        }
                    }
            }   
        case UNARY:
            Node *operand = derivative(root -> unary.operand, derArg);
            switch (operand -> type) {
                case NUM:
                    return createNumber("0");
                case VAR:
                    if (!strcmp(operand -> var, derArg)) {
                        return createNumber("1");
                    } else {
                        return createNumber("0");
                    }
                case FUNC:
                    char *name = operand -> func.name;
                    for (int i = 0; i < sizeof(derFuncs)/sizeof(ders); ++i) {
                        if (!strcmp(name, derFuncs[i].func)) {
                            if (!strcmp(operand -> func.arguments[0] -> var, derArg)) {
                                return createFunction(derFuncs[i].der, derFuncs[i].argCount, operand -> func.arguments);
                            } else {
                                return createNumber("0");
                            }
                        }
                    }
                    fprintf(stderr, "OSHIBKA\n");
                    exit(1);
                case BINARY:
                    return createUnary(operand -> type, operand -> unary.oper, operand);
                case UNARY:
                    return createUnary(operand -> type, operand -> unary.oper, operand);
            }
            der = createUnary(operand -> type, operand -> unary.oper, operand);
            return der;
        case NUM:
            der = createNumber("0");
            return der;
        case VAR:
            if (!strcmp(root -> var, derArg)) {
                der = createNumber("1");
                return der;
            } else {
                der = createNumber("0");
                return der;
            }
        case FUNC:
            char *name = root -> func.name;
            for (int i = 0; i < sizeof(derFuncs)/sizeof(ders); ++i) {
                if (!strcmp(name, derFuncs[i].func)) {
                    if (!strcmp(root -> func.arguments[0] -> var, derArg)) {
                        return createFunction(derFuncs[i].der, derFuncs[i].argCount, root -> func.arguments);
                    } else {
                        return createNumber("0");
                    }
                }
            }
            fprintf(stderr, "OSHIBKA\n");
            exit(1);
        case GROUP:
            return derivative(root -> group, derArg);
    }
}

Node*
simplify(Node *root) {
    switch (root -> type) {
        case BINARY:
            double num;
            Node *left = simplify(root -> binary.left);
            Node *right = simplify(root -> binary.right);
            Node *ladno;
            double leftNum, rightNum;
            int flagL = 0, flagR = 0;
            char str [10];
            switch (root -> binary.type) {
                case SUM:
                    if (left -> type == NUM) {
                        leftNum = left -> number;
                        flagL = 1;
                    } else if (left -> type == UNARY && left -> unary.operand -> type == NUM) {
                        leftNum = (strcmp(left -> unary.oper, "+")) ? -left -> unary.operand -> number : left -> unary.operand -> number;
                        flagL = 1;
                    }
                    if (right -> type == NUM) {
                        rightNum = right -> number;
                        flagR = 1;
                    } else if (right -> type == UNARY && right -> unary.operand -> type == NUM) {
                        rightNum = (strcmp(right -> unary.oper, "+")) ? -right -> unary.operand -> number : right -> unary.operand -> number;
                        flagR = 1;
                    }
                    if (flagL && flagR) {
                        num = leftNum + rightNum;
                        sprintf(str, "%g", abss(&num));
                        ladno = createNumber(str);
                        return (num < 0) ? simplify(createUnary(TOKEN_MINUS, "-", ladno)) : simplify(ladno);
                    }
                    if (flagL) {
                        if (leftNum == 0) return right; 
                    }
                    if (flagR) {
                        if (rightNum == 0) return left;
                    }
                    if (right -> type == UNARY) {
                        if (!strcmp(right -> unary.oper, "-")) {
                            return simplify(createBinary(TOKEN_MINUS, "-", left, right -> unary.operand));
                        }
                    }
                    return createBinary(root -> binary.type, root -> binary.oper, left, right);
                case SUB:
                    if (left -> type == NUM) {
                        leftNum = left -> number;
                        flagL = 1;
                    } else if (left -> type == UNARY && left -> unary.operand -> type == NUM) {
                        leftNum = (strcmp(left -> unary.oper, "+")) ? -left -> unary.operand -> number : left -> unary.operand -> number;
                        flagL = 1;
                    }
                    if (right -> type == NUM) {
                        rightNum = right -> number;
                        flagR = 1;
                    } else if (right -> type == UNARY && right -> unary.operand -> type == NUM) {
                        rightNum = (strcmp(right -> unary.oper, "+")) ? -right -> unary.operand -> number : right -> unary.operand -> number;
                        flagR = 1;
                    }
                    if (flagL && flagR) {
                        num = leftNum - rightNum;
                        sprintf(str, "%g", abss(&num));
                        ladno = createNumber(str);
                        return (num < 0) ? simplify(createUnary(TOKEN_MINUS, "-", ladno)) : simplify(ladno);
                    }
                    if (flagL) {
                        if (leftNum == 0) return createUnary(TOKEN_MINUS, "-", right); 
                    }
                    if (flagR) {
                        if (rightNum == 0) return left;
                    }
                    if (right -> type == UNARY) {
                        if (!strcmp(right -> unary.oper, "-")) {
                            return simplify(createBinary(TOKEN_PLUS, "+", left, right -> unary.operand));
                        }
                    }
                    return createBinary(root -> binary.type, root -> binary.oper, left, right);
                case MUL:
                    if (left -> type == NUM) {
                        leftNum = left -> number;
                        flagL = 1;
                    } else if (left -> type == UNARY && left -> unary.operand -> type == NUM) {
                        leftNum = (strcmp(left -> unary.oper, "+")) ? -left -> unary.operand -> number : left -> unary.operand -> number;
                        flagL = 1;
                    }
                    if (right -> type == NUM) {
                        rightNum = right -> number;
                        flagR = 1;
                    } else if (right -> type == UNARY && right -> unary.operand -> type == NUM) {
                        rightNum = (strcmp(right -> unary.oper, "+")) ? -right -> unary.operand -> number : right -> unary.operand -> number;
                        flagR = 1;
                    }
                    if (flagL && flagR) {
                        num = leftNum * rightNum;
                        sprintf(str, "%g", abss(&num));
                        ladno = createNumber(str);
                        return (num < 0) ? simplify(createUnary(TOKEN_MINUS, "-", ladno)) : simplify(ladno);
                    }
                    if (flagL) {
                        if (leftNum == 0) return createNumber("0");
                        if (leftNum == 1) return right; 
                    }
                    if (flagR) {
                        if (rightNum == 0) return createNumber("0");
                        if (rightNum == 1) return left;
                    }
                    return createBinary(root -> binary.type, root -> binary.oper, left, right);
                case DIV:
                    if (left -> type == NUM) {
                        leftNum = left -> number;
                        flagL = 1;
                    } else if (left -> type == UNARY && left -> unary.operand -> type == NUM) {
                        leftNum = (strcmp(left -> unary.oper, "+")) ? -left -> unary.operand -> number : left -> unary.operand -> number;
                        flagL = 1;
                    }
                    if (right -> type == NUM) {
                        rightNum = right -> number;
                        flagR = 1;
                    } else if (right -> type == UNARY && right -> unary.operand -> type == NUM) {
                        rightNum = (strcmp(right -> unary.oper, "+")) ? -right -> unary.operand -> number : right -> unary.operand -> number;
                        flagR = 1;
                    }
                    if (flagL && flagR) {
                        num = leftNum / rightNum;
                        sprintf(str, "%g", abss(&num));
                        ladno = createNumber(str);
                        return (num < 0) ? simplify(createUnary(TOKEN_MINUS, "-", ladno)) : simplify(ladno);
                    }
                    if (flagL) {
                        if (leftNum == 0) return createNumber("0"); 
                    }
                    if (flagR) {
                        if (rightNum == 1) return left;
                    }
                    return createBinary(root -> binary.type, root -> binary.oper, left, right);
                case POW:
                    if (right -> type == NUM) {
                        if (right -> number == 0) {
                            return simplify(createNumber("1"));
                        }
                    }
                    if (right -> type == NUM) {
                        if (right -> number == 1) {
                            return left;
                        }
                    }
                    if (left -> type == NUM) {
                        if (left -> number == 0) {
                            return simplify(createNumber("0"));
                        }
                    }
                    if (left -> type == NUM) {
                        if (left -> number == 1) {
                            return simplify(createNumber("1"));
                        }
                    }
                    return createBinary(root -> binary.type, root -> binary.oper, left, right);
            }
        case UNARY:
            return root;
        case NUM:
            return root;
        case VAR:
            return root;
        case FUNC:
            for (size_t i = 0; i < root -> func.count; ++i) {
                root -> func.arguments[i] = simplify(root -> func.arguments[i]);
            }
            return root;
        case GROUP:
            Node *group = simplify(root -> group);
            return group;

    }
}
