#include "My_Differentiator.h"
#include "assert.h"
#include <iostream>
namespace My_Diff
{

Node* Rec_descent::get_G(char* buf)
{
    ptr = begin = buf;

    root = get_E();

    int place = ptr - buf;
    if (*ptr != '\n')
        printf("Syntactic mistake! in %d place. Found %c, expected end of line\n", place + 1, *ptr );

    return root;
}

Node* Rec_descent::get_E()
{
    skip_space();

    Node* node = get_T();

    while (*ptr == '+' || *ptr == '-')
    {
        char op = *ptr;
        ptr++; skip_space();

        if (op == '+')
        {
            Node* node2 = get_T();
            node = CreateNode(OP, add, node, node2);
        }
        else
        {
            Node* node2= get_T();
            node = CreateNode(OP, sub, node, node2);
        }
    }
        return node;
}

Node* Rec_descent::get_T()
{
    skip_space();
    Node* node = get_Deg();

    while (*ptr == '*' || *ptr == '/')
    {
        char op = *ptr;
        ptr++; skip_space();

        if (op == '*' )
        {
            Node* node2 = get_Deg();
            node = CreateNode(OP, mul, node, node2);
        }
        else
        {
            Node* node2 = get_Deg();
            node = CreateNode(OP, div, node, node2);
        }
    }

    return node;
}

Node* Rec_descent::get_Deg()
{
    skip_space();
    Node* node = get_Trigo();


    while (*ptr == '^')
    {
        ptr++; skip_space();

        Node* node2 = get_Trigo();

        node = CreateNode(OP, '^', node, node2);
    }

    return node;
}

Node *Rec_descent::get_Trigo()
{
    skip_space();

    char trig[4] = "";

    for (int i = 0; i < 3 && *(ptr + i) != '\0'; i++)
        trig[i] = *(ptr + i);

    Node* node = nullptr;
    if (strcmp(trig, "sin") == 0)
    {
      ptr+=3;
      assert(*ptr == '(');
      ptr++; skip_space();

      node = get_E();

      assert(*ptr == ')');
      ptr++; skip_space();

      node = CreateNode(OP, 's', nullptr, node);
    }
    else
    if (strcmp(trig, "cos") == 0)
    {
        ptr+=3;
        assert(*ptr == '(');
        ptr++; skip_space();

        node = get_E();

        assert(*ptr == ')');
        ptr++; skip_space();

        node = CreateNode(OP, 'c', nullptr, node);
    }
    else
        node = get_P();

    return node;
}

    Node* Rec_descent::get_P()
{
    skip_space();

    if (*ptr == '(')
    {
        ptr++; skip_space();

        Node* node = get_E();

        if (*ptr != ')')
        {
            printf("Syntactic mistake! in %d place. Found %c, expected ')'\n", ptr - begin, *ptr );
            assert(*ptr == ')');
        }

        ptr++; skip_space(); // for ')'

        return node;
    }

    char* oldptr = ptr;

    Node* node = new Node;

    if ('0' <= *ptr && *ptr <= '9' )
    {
        node->value = get_N();
        node->type = CONST;
    }
    else
    {
        node->value = *ptr;
        ptr++; skip_space();
        node->type = VAR;
    }

    if (ptr == oldptr)
    {
        printf("Syntactic mistake! Empty number.\n");
        assert(ptr != oldptr);
    }

    return node;
}

double Rec_descent::get_N()
{
    double val = 0;

    while ('0' <= *ptr && *ptr <= '9' )
    {
        val = val * 10 + *ptr  - '0';
        ptr++;  skip_space();
    }

    return val;
}

int Rec_descent::skip_space()
{
    while (*ptr == ' ')
        ptr++;

    return 0;
}

} // My_Diff