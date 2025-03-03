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

void                                                // Got the print function (made by copilot)
printTree(Node *root) {
    printHelp(root);
    printf("\n");
}

void
printHelp(Node *root) {
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

Node*                                                                // Started derivative function, have some troubles with count of arguments in my "built-in" functions
derivative(Node *root, char *derArg) {
    switch(root -> type) {
        case BINARY:
            Node *left = derivative(root -> binary.left, derArg);
            Node *right = derivative(root -> binary.right, derArg);
            switch (root -> binary.type) {
                case SUM:
                    Node *der = createBinary(TOKEN_PLUS, "+", left, right);
                    return der;
                case SUB:
                    Node *der = createBinary(TOKEN_MINUS, "-", left, right);
                    return der;
                case MUL:
                    Node *ll = root -> binary.left;
                    Node *lr = right;
                    Node *rl = left;
                    Node *rr = root -> binary.right;
                    Node *der = createBinary(TOKEN_PLUS, "+", createBinary(TOKEN_STAR, "*", ll, lr), createBinary(TOKEN_STAR, "*", rl, rr));
                    return der;
                case DIV:
                    Node *ll = root -> binary.left;
                    Node *lr = right;
                    Node *rl = left;
                    Node *rr = root -> binary.right;
                    Node *der = createBinary(TOKEN_SLASH, "/", createBinary(TOKEN_MINUS, "-", createBinary(TOKEN_STAR, "*", ll, lr), createBinary(TOKEN_STAR, "*", rl, rr)), createBinary(TOKEN_CARET, "^", rr, createNumber("2")));
                    return der;
                case POW:
                    if (root -> binary.left -> type == NUM) {
                        if (root -> binary.right -> type == NUM) {
                            Node *der = createNumber("0");
                            return der;
                        } else {
                            Node *der = createBinary(TOKEN_STAR, "*", createBinary(TOKEN_STAR, "*", right, createFunction("log", 1, root -> binary.left)), root);
                            return der;
                        }
                    } else {
                        if (root -> binary.right -> type == NUM) {
                            char number[32];
                            sprintf(number, "%g", root -> binary.right -> number - 1);
                            Node *der = createBinary(TOKEN_STAR, "*", root -> binary.right, createBinary(TOKEN_STAR, "*", left, createBinary(TOKEN_CARET, "^", root -> binary.left, createNumber(number))));
                            return der;
                        }
                        else {
                            Node *der = createBinary(TOKEN_CARET, "^", createNumber("e"), createBinary(TOKEN_STAR, "*", root -> binary.right, createFunction("log", 1, root -> binary.left)));
                            return der;
                        }
                    }
            }   
        case UNARY:
            Node *operand = derivative(root -> unary.operand, derArg);
            Node *der = createUnary(operand -> type, operand -> unary.oper, operand);
            return der;
        case NUM:
            Node *der = createNumber("0");
            return der;
        case VAR:
            if (!strcmp(root -> var, derArg)) {
                Node *der = createNumber("1");
                return der;
            } else {
                Node *der = createNumber("0");
                return der;
            }
        case FUNC:
            char *name = root -> func.name;
            for (int i = 0; i < sizeof(derFuncs)/sizeof(ders); ++i) {
                if (!strcmp(name, derFuncs[i].func)) {
                    return createFunction(derFuncs[i].der, derFuncs[i].argCount, root -> func.arguments);
                }
            }
            fprintf(stderr, "OSHIBKA\n");
            exit(1);
        case GROUP:
            return derivative(root -> group, derArg);
    }
}

// char*
// derivative(const char *input, char derArg) {
    // char *der = (char) malloc(64);
    // size_cdz index = 0;
    // return helpDerivative(input, derArg, der, &index);
// }
// 
// char*
// helpDerivative(const char *input, char derArg, char *der, size_cdz *index) {
// 
// }
