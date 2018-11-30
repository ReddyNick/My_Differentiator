#include <iostream>
#include "My_Differentiator.h"
#include "assert.h"

//todo sin, cos, etc.
//todo degree (^), sqrt
//todo ln, log, exp
//todo parent, side are necessary?
//todo check g != 0 when d/dt(f/g)




int main()
{
    My_Diff::Tree tree;

    FILE* input = fopen("Polynomial.txt", "r");
    FILE* res = fopen("result.txt", "w");

    tree.Read(input);

    My_Diff::Tree derivation;


    derivation.root = tree.diff(tree.root);

    derivation.Write_tree(res);


    FILE* dot = fopen("dot/tree.gv","w");

//    tree.Write_DOT(dot);
    derivation.Write_DOT(dot);

    fclose(dot);



    fclose(res);
    fclose(input);
    return 0;
}