#include "exp.h"

int
main() {
    const char input[]= "-3 + -x + 23";
    Token *tokensss = tokenize(input);
    size_cdz i = 0;
    while (tokensss[i].type != TOKEN_END) {
        printf("Token %d: id = %d, value = %s\n", i + 1, tokensss[i].type, tokensss[i].value);
        ++i;
    }
    variable myVars[] = {"x", 8};
    Node *line = parse(tokensss);
    printf("%.2f\n", eval(line, myVars, 1, NULL, 0));
}