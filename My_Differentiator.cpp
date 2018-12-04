#include "My_Differentiator.h"
#include "assert.h"
#include "ctype.h"
#include <iostream>
#include "math.h"
int erno = 0;

namespace My_Diff
{
//from lower priority down to higher - no brackets
//from higher down to lower - brackets.

int get_priority(Node* node)
{
    switch((int)node->value)
    {
        case add:
            return p_add;
        case sub:
            return p_sub;
        case mul:
            return p_mul;
        case div:
            return p_div;
        case pow:
            return p_pow;
        case sin:
            return p_sin;
        case cos:
            return p_cos;
        default:
            printf("wrong priority!\n");
            abort();
    }
}

int LaTex(FILE *out, Node* node1, Node* node2)
{

    fprintf(out, "\\documentclass[12pt,a4paper]{scrartcl}\n"
                 "\\usepackage[utf8]{inputenc}\n"
                 "\\usepackage[english,russian]{babel}\n"
                 "\\usepackage{indentfirst}\n"
                 "\\usepackage{misccorr}\n"
                 "\\usepackage{graphicx}\n"
                 "\\usepackage{amsmath}\n"
                 "\\begin{document}\n");

    fprintf(out, "Найдем производную \n\n $f(x) = ");

    Write_tree_rec(node1, 0, out); fprintf(out, "$\n\n");

    fprintf(out,"элементарно и ачевидно, что\n\n $\\frac{df(x)}{dx} = ");

    Write_tree_rec(node2, 0, out); fprintf(out, "$\n\n");

    fprintf(out, "\n\n"
                 "\\end{document}");

    return 0;
}

    int Tree::Write_tree(FILE *out)
{
    fprintf(out, "\\documentclass[12pt,a4paper]{scrartcl}\n"
                 "\\usepackage[utf8]{inputenc}\n"
                 "\\usepackage[english,russian]{babel}\n"
                 "\\usepackage{indentfirst}\n"
                 "\\usepackage{misccorr}\n"
                 "\\usepackage{graphicx}\n"
                 "\\usepackage{amsmath}\n"
                 "\\begin{document}\n"
                 "$$\n");


    Write_tree_rec(root, 0, out);

    fprintf(out, "\n$$\n"
                 "\\end{document}");

    return 0;
}

int Write_tree_rec(Node *node, int prev_proir, FILE* out)
{
    // assert out of function
    assert(out != NULL);

    if (node == nullptr) return 0;

    if (node->type != OP)
    {
        if(node->type == CONST)
            fprintf(out, "%lg", node->value);
        else
            fprintf(out, "%c", (char)node->value);
    }
    else
    {
        int prior = get_priority(node);

        if (prev_proir > prior)
            fprintf(out, "(");


        if ((int)node->value == '/')
        {
            fprintf(out, "\\frac{");
            prior = 0;

            Write_tree_rec(node->left, prior, out);

            fprintf(out, "}{");

            Write_tree_rec(node->right, prior, out);

            fprintf(out, "}");
        }
        else
        {
            Write_tree_rec(node->left, prior, out);

            switch ((int) node->value)
            {
                case mul:
                    fprintf(out, "\\cdot ");
                    break;
                case sin:
                    fprintf(out, "sin");

                    fprintf(out, "(");
                    Write_tree_rec(node->right, 0, out);
                    fprintf(out, ")");

                    return 0;

                case cos:
                    fprintf(out, "cos");

                    fprintf(out, "(");
                    Write_tree_rec(node->right, 0, out);
                    fprintf(out, ")");

                    return 0;

                case add:
                case sub:
                case pow:
                    fprintf(out,"%c",(char)node->value);
                    break;

                default:
                    fprintf(out, "error writing!\n");
                    assert(0);
            }

            Write_tree_rec(node->right, prior, out);
        }

        if (prev_proir > prior)
            fprintf(out, ")");
    }

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

#define  COS CreateNode(OP, cos, nullptr, R)

#define  SIN CreateNode(OP, sin, nullptr, R)

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
                    return MUL(COS, dR);

                case cos:
                {
                    Node* minus_one = new Node;
                    minus_one->type = CONST;
                    minus_one->value = -1;
                    return MUL(MUL(SIN, dR), minus_one);
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

int ConstS(Node* node, Operations oper)
{
    assert(node != nullptr);

    if (node->left->type == CONST && node->right->type == CONST)
    {

        double num = 0;
        switch (oper)
        {
            case mul:
                num = node->left->value * node->right->value;
                break;
            case add:
                num = node->left->value + node->right->value;
                break;
            case sub:
                num = node->left->value - node->right->value;
                break;
            case div:
                num = node->left->value / node->right->value;
                break;
        }

        node->type = CONST;
        node->value = num;

        Delete_rec(node->right);
        Delete_rec(node->left);

        node->left = node->right = nullptr;

        return 1;
    }

    return 0;
}
int Add_zero_mul_one(Node* node, int num)
{
    bool Left = false;

    if (Check(node, num, &Left))
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

        return 1;
    }

    return 0;
}

int mul_div_zero(Node* node)
{
    int num = 0;
    node->type = CONST;
    node->value = num;

    Delete_rec(node->right);
    Delete_rec(node->left);

    node->left = node->right = nullptr;

    return 0;
}


int Simplify(Node* node)
{
    if (node == nullptr || is_leaf(node)) return 0;


    Simplify(node->left);
    Simplify(node->right);

    switch((int)node->value)
    {
        case mul:
            if (ConstS(node, mul));

            else
            if (Add_zero_mul_one(node, 1));

            else
            {
                bool Left = false;

                if (Check(node, 0, &Left))
                    mul_div_zero(node);
            }
            break;

        case add:
            if (ConstS(node, add));
            else
                Add_zero_mul_one(node, 0);
            break;

        case sub:
            if (ConstS(node, sub));

            else
            if (node->right->type == CONST && node->right->value == 0)
            {
                delete[] node->right;
                Instead(node, node->left);
            }
            break;

        case div:
        {
            bool Left = false;

            if (ConstS(node, div));

            else
            if (Check(node,0, &Left) && Left)
                mul_div_zero(node);
        }
        break;
    }

    return 0;
}

}