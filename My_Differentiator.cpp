#include "My_Differentiator.h"
#include "assert.h"
#include "ctype.h"
#include <iostream>
int erno = 0;

namespace My_Diff
{

int Tree::Write_tree_rec(Node *node, FILE* out)
{
    // assert out of function
    assert(out != NULL);

    if (node == nullptr) return 0;

    Tree::Write_tree_rec(node->left, out);

    if (node->type == OP)
        fprintf(out, "%c", (char)node->value);

    else
    if(node->type == CONST)
        fprintf(out, "%lg", node->value);

    else
        fprintf(out, "%c", (char)node->value);

    Tree::Write_tree_rec(node->right, out);

    return 0;
}

int Tree::Read(FILE* input)
{
    assert(input != NULL);

    fseek(input, 0, SEEK_END);
    size_t buf_size = ftell(input);
    rewind(input);

    char* buf = new char[buf_size + 1];
    memset(buf, '\0', buf_size);

    fread(buf, sizeof(buf[0]), buf_size + 1, input);

    Rec_descent tree;

    root = tree.get_G(buf);

    delete[] buf;

    return 0;
}

Node* Copy(Node* node)
{
    if (node == nullptr) return nullptr;

    Node* newNode = new Node;


    newNode->type  = node->type;
    newNode->value = node->value;

    newNode->left = Copy(node->left);
    newNode->right = Copy(node->right);

    return newNode;
}

Node* CreateNode(TYPE type, double value, Node* left, Node* right)
{
    Node* newNode = new Node;

    newNode->type  = type;
    newNode->value = value;
    newNode->left  = left;
    newNode->right = right;

    return newNode;
}

Node* Tree::diff(Node *node)
{
#define L Copy(node->left)
#define R Copy(node->right)

#define dL diff(node->left)
#define dR diff(node->right)

#define PLUS(a, b) CreateNode(OP, add, a, b)

#define  SUB(a, b) CreateNode(OP, sub, a, b)

#define  MUL(a, b) CreateNode(OP, mul, a, b)

#define  DIV(a, b) CreateNode(OP, div, a, b)

#define  POW(a, b) CreateNode(OP, pow, a, b)

#define  COS CreateNode(OP, cos, L, nullptr)

#define  SIN CreateNode(OP, sin, L, nullptr)

    switch (node->type)
    {
        case CONST:
            return CreateNode(CONST, 0, nullptr, nullptr);

        case VAR:
            return CreateNode(CONST, 1, nullptr, nullptr);

        case OP:
            switch ((int)node->value)
            {
                case add:
                    return PLUS(dL, dR);

                case mul:
                    return PLUS(MUL(dL, R), MUL(L,dR));

                case sub:
                    return SUB(dL, dR);

                case div:
                    return DIV(SUB(MUL(dL, R), MUL(L, dR)), MUL(R, R));

                case sin:
                    return MUL(COS, dL);

                case cos:
                {
                    Node* minus_one = new Node;
                    minus_one->type = CONST;
                    minus_one->value = -1;
                    return MUL((SIN, dL), minus_one);
                }

                case pow:
                {
                    Node* one = new Node;
                    one->type = CONST;
                    one->value = 1;
                    return MUL(MUL(R, POW(L, SUB(R, one))), dL);
                }

                default:
                    erno = 2;
                    return nullptr;
            }

        default:
            erno = 1;
            return nullptr;
    }

}

bool is_leaf(Node* ptr)
{
    if (ptr->left == nullptr && ptr->right == nullptr)
        return true;
    else
        return false;
}

int Delete_rec(Node* ptr)
{
    if (ptr == nullptr) return 0;

    if (ptr->left != nullptr)
        Delete_rec(ptr->left);

    if (ptr->right != ptr->left)
        if (ptr->right != nullptr)
            Delete_rec(ptr->right);

    delete[] ptr;
    return 0;
}


Tree::~Tree()
{
    if (root != nullptr)
        Delete_rec(root);

    root = nullptr;
}

int Tree::Write_DOT(FILE* out)
{
        assert(out != NULL);
        assert(root!=nullptr);

#define fillcolor "#FFEBCD"
        fprintf(out,"digraph G {\nnode[shape=ellipse fontsize =15 style=%cfilled%c fillcolor=%c" fillcolor "%c]\n",
                '"', '"', '"', '"');

        int num = 0;
        Dotwrite_elems(root, &num, out);

        fprintf(out, "}");
        return 0;
}

int Dotwrite_elems(Node* elem, int* num, FILE* out)
{
        assert(elem !=nullptr);

        int cur_num = *num;
        if (elem->type == CONST)
            fprintf(out, "node%d[label=%c%lg%c];\n", cur_num, '"', elem->value, '"');
        else
            fprintf(out, "node%d[label=%c%c%c];\n", cur_num, '"', (char)elem->value, '"');

        if (elem->left != nullptr)
        {
            ++*num;
            fprintf(out, "node%d->node%d;\n", cur_num, *num);

            Dotwrite_elems(elem->left, num, out);
        }

        if (elem->right != nullptr)
        {
            ++*num;
            fprintf(out, "node%d->node%d;\n", cur_num, *num);
            Dotwrite_elems(elem->right, num, out);
        }

        return 0;
}
// mult on zero
// plus zero
// mult on 1
// mult on -1

bool Check(Node* node, int num,  bool* Left)
{
#define check(a) node->a->type == CONST && node->a->value == num

    assert(node != nullptr);

   if (check(left))
   {
       *Left = true;
       return true;
   }

   if (check(right))
   {
       *Left = false;
       return true;
   }

   return false;
}
 int Instead(Node* node1, Node* node2)
{
     assert(node1 != nullptr && node2 != nullptr);

     node1->type = node2->type;
     node1->value = node2->value;
     node1->left = node2->left;
     node1->right = node2->right;

     delete[] node2;
     return 0;
}

int ConstS(Node* node, double num)
{
    assert(node != nullptr);

    node->type = CONST;
    node->value = num;

    Delete_rec(node->right);
    Delete_rec(node->left);

    node->left = node->right = nullptr;

    return 0;
}

int simple_mul(Node* node)
{
    bool Left = false;

    if (node->left->type == CONST && node->right->type == CONST)
    {
        double num = node->left->value * node->right->value;
        ConstS(node, num);

        return 0;
    }

    if (Check(node, 0, &Left))
    {
        ConstS(node, 0);
        return 0;
    }

    if (Check(node, 1, &Left))
    {
        if (Left)
        {
            delete[] node->left;
            Instead(node, node->right);
        }
        else
        {
            delete[] node->right;
            Instead(node, node->left);
        }
    }

    return 0;
}

int simple_add(Node* node)
{
    bool Left = false;

    if (node->left->type == CONST && node->right->type == CONST)
    {
        double num = node->left->value + node->right->value;
        ConstS(node, num);

        return 0;
    }

    if (Check(node, 0, &Left))
    {
        if (Left)
        {
            delete[] node->left;
            Instead(node, node->right);
        }
        else
        {
            delete[] node->right;
            Instead(node, node->left);
        }

        return 0;
    }

    return 0;
}

int Simplify(Node* node)
{
    if (node == nullptr || is_leaf(node)) return 0;


    Simplify(node->left);
    Simplify(node->right);

    switch((int)node->value)
    {
        case '*':
            if (simple_mul(node)) return 0;
            break;

        case '+':
            if (simple_add(node)) return 0;
            break;
    }

    return 0;
}

}