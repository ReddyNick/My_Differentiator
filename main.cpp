#include <iostream>
#include "My_Differentiator.h"
#include "assert.h"

//todo
// sqrt
// ln, log, exp
// parent, side are necessary?
// check g != 0 when d/dt(f/g)
// one symbol variable ONLY
// simplifer
// NO derivative of complex fucntion
// NO minus in begining
// no simple subst, division

int main()
{
    My_Diff::Tree tree;

    FILE* input = fopen("Polynomial.txt", "r");
    FILE* res = fopen("result.txt", "w");

    tree.Read(input);

    My_Diff::Tree derivation;


    derivation.root = tree.diff(tree.root);
    My_Diff::Simplify(derivation.root);

//    derivation.Write_tree(res);


    FILE* dot = fopen("dot/tree.gv","w");

//    tree.Write_DOT(dot);
    derivation.Write_DOT(dot);

    fclose(dot);



    fclose(res);
    fclose(input);
    return 0;
}