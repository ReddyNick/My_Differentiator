#ifndef MY_DIFFERENTIATOR_MY_DIFFERENTIATOR_H
#define MY_DIFFERENTIATOR_MY_DIFFERENTIATOR_H
#include <vector>
#include "stdio.h"
#include "string.h"

const int STR_MAXSIZE = 20;
namespace My_Diff
{

enum TYPE
{
    OP = 0, VAR, CONST, Other
};

enum Operations
{
    add = '+', sub = '-', mul = '*', div = '/',
    sin = 's', cos = 'c', pow = '^',
};

struct Node
{
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;

    std::vector<Node*> nodes;

    
    // if left, side == true
    // if right, side == false
    bool side = false;

    TYPE type = Other;
    double value = 0;
};

class Tree
{

public:


    Node* root = nullptr;
    size_t size = 0;

    // infix type of tree
    int Read(FILE* input);
    Node* Make_tree(char*& ptr);

    // infix write without any brackets
    int Write_tree_rec(Node* root, FILE* out);

    int Write_tree(FILE *out)
    {
        return Write_tree_rec(root, out);
    }

    Node* diff(Node* node);

    int Write_DOT(FILE* out);

    ~Tree();
};

int Simplify(Node* node);

int Delete_rec(Node* ptr);

Node* CreateNode(TYPE type, double value, Node* left, Node* right);

bool is_leaf(Node* ptr);

int Dotwrite_elems(Node* elem, int* num, FILE* out);

class Rec_descent
{
    char* begin = nullptr;
    char* ptr = nullptr;
    int size = 0;

    Node* root = nullptr;

    Node* get_E();
    Node* get_T();
    Node* get_Deg();
    Node* get_Trigo();
    Node* get_P();
    double get_N();
    int skip_space();

public:

    Node* get_G(char* buf);

};


}


#endif //MY_DIFFERENTIATOR_MY_DIFFERENTIATOR_H
