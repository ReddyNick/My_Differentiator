#include "My_Differentiator.h"
#include "assert.h"
#include "ctype.h"

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

    char* ptr = buf;

    root = Make_tree(ptr);

    delete[] buf;

    return 0;
}

Node* Tree::Make_tree(char* &ptr)
{

#define Move ptr += move;

    assert(ptr != nullptr);

    char c = '\0';
    int move = 0;
    double number = 0;

    // first reading
    sscanf(ptr, " %c%n", &c, &move);
    Move;

    if (c != '(')
    {
        printf("invalid format!\n");
        assert(0);
        return nullptr;
    }

    Node* newNode = new Node;
    size++;

    //second reading
    sscanf(ptr, " %c%n", &c, &move);

    if (c == '(')
    {
        newNode->left = Make_tree(ptr);

        sscanf(ptr, " (%c)%n", &c, &move);
        Move;

        newNode->type = OP;
        newNode->value = c;

        newNode->right = Make_tree(ptr);
    }

    // if a leaf
    else
    {
        if (isdigit(c))
        {
            sscanf(ptr, " %lg%n", &number, &move);
            Move;

            newNode->type = CONST;
            newNode->value = number;
        }
        // if a variable
        else
        {
            sscanf(ptr, " %c%n", &c, &move);
            Move;

            newNode->type = VAR;
            newNode->value = c;
        }
    }

    sscanf(ptr, " )%n", &move);
    Move;

    return newNode;
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
#define YEScolor "red"
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
}