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