#pragma once
#include <iostream>

using namespace std;
class values_here
{
public:
    int data = 0;
    values_here() {}
    values_here(int data) : data(data)
    {
    }

    bool operator==(const values_here& other)
    {
        return (data == other.data);
    }
    bool operator>(const values_here& other)
    {
        return (data > other.data);
    }
    bool operator<(const values_here& other)
    {
        return (data < other.data);
    }
    bool operator<=(const values_here& other)
    {
        return (data <= other.data);
    }
    bool operator>=(const values_here& other)
    {
        return (data >= other.data);
    }
    bool operator!=(const values_here& other)
    {
        return (data >= other.data);
    }
    int operator=(const values_here& other)
    {
        return other.data;
    }
    friend std::ostream& operator<<(std::ostream& os, const values_here& other)
    {
        os << other.data << endl;
        return os;
    }
};

template <typename T>
class node
{
public:
    T val;
    node<T>* left;
    node<T>* right;
    int height;
    node() : height(0), left(nullptr), right(nullptr) {}
    node(T val) : height(0), left(nullptr), right(nullptr), val(val) {}
};

template <typename T>
class AVL
{
    node<T>* root;

public:
    AVL() : root(nullptr) {}

    void insert(T val)
    {
        root = inserting(val, root);
    }

    int computeHeight(node<T>* n)
    {
        if (n == nullptr)
        {
            return -1;
        }

        int leftHeight = computeHeight(n->left);
        int rightHeight = computeHeight(n->right);

        n->height = std::max(leftHeight, rightHeight) + 1;

        return n->height;
    }
    node<T>* inserting(T val, node<T>* start)
    {
        if (start == nullptr)
        {
            // cout << "Inserting" << val << endl;
            node<T>* create = new node<T>(val);
            return create;
        }
        else if (val > start->val)
        {
            // cout << "Passing through :" << start->val << endl;
            start->right = inserting(val, start->right);
        }
        else if (val < start->val)
        {
            // cout << "Passing through :" << start->val << endl;
            start->left = inserting(val, start->left);
        }

        computeHeight(start);
        int balance = Balanced(start);
        if (balance > 1)
        {
            if (val > start->left->val)
            {
                start->left = leftrotate(start->left);
            }

            return rightrotate(start);
        }

        if (balance < -1)
        {
            if (val < start->right->val)
            {

                start->right = rightrotate(start->right);
            }
            return leftrotate(start);
        }
        return start;
    }

    node<T>* rightrotate(node<T>* y)
    {
        node<T>* x = y->left;
        node<T>* T2 = x->right;
        y->left = T2;
        x->right = y;
        x->height = (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right)) + 1;
        y->height = (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right)) + 1;
        return x;
    }

    node<T>* leftrotate(node<T>* x)
    {
        node<T>* y = x->right;
        node<T>* T2 = y->left;
        x->right = T2;
        y->left = x;
        x->height = (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right)) + 1;
        y->height = (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right)) + 1;
        return y;
    }
    int getHeight(node<T>* node)
    {
        if (node == nullptr)
        {
            return -1;
        }
        return node->height;
    }

    void preorder(node<T>* curr, int lev, int k)
    {
        if (curr == nullptr)
        {
            return;
        }
        if (k == lev)
        {
            cout << "   ==   " << curr->val << endl;
            return;
        }
        if (curr->left != nullptr)
        {
            // cout << "\nleft->";
            preorder(curr->left, lev, k + 1);
        }
        if (curr->right != nullptr)
        {
            // cout << "\nright->";
            preorder(curr->right, lev, k + 1);
        }
    }

    node<T>* deleteNode(T val)
    {
        if (root->val != val)
        {
            node<T>* curr = root;
            node<T>* prev = root;
            bool found = false;
            while (!(curr == nullptr))
            {
                if (curr->val == val)
                {
                    found = true;
                    break;
                }
                else if (curr->val > val)
                {
                    prev = curr;
                    curr = curr->left;
                }
                else if (curr->val < val)
                {
                    prev = curr;
                    curr = curr->right;
                }
            }
            if (found)
            {
                cout << "found: " << found << endl;
                if (prev->val > curr->val)
                {
                    prev->left = nullptr;
                }
                else if (prev->val < curr->val)
                {
                    prev->right = nullptr;
                }
                node<T>* lefty = curr->left;
                node<T>* righty = curr->right;
                if (lefty != nullptr)
                    help_del(lefty);
                if (righty != nullptr)
                    help_del(righty);
            }
            else
            {
                return nullptr;
            }
        }
        else if (root->val == val)
        {

            if (root->left == nullptr && root->right != nullptr)
            {
                node<T>* top = root->right;
                delete root;
                root = top;
            }
            else if (root->right == nullptr && root->left != nullptr)
            {
                node<T>* top = root->left;
                delete root;
                root = top;
            }
            else if (root->left == nullptr && root->right == nullptr)
            {
                delete root;
                root = nullptr;
            }
            else if (root->left != nullptr && root->right != nullptr)
            {

                node<T>* righty = root->right;
                node<T>* top = root->left;
                delete root;
                root = top;
                if (righty != nullptr)
                    help_del(righty);
            }
        }
    }
    void help_del(node<T>* curr)
    {
        insert(curr->val);
        if (curr->left != nullptr)
        {
            help_del(curr->left);
        }
        if (curr->right != nullptr)
        {
            help_del(curr->right);
        }
    }

    void print()
    {
        if (root == nullptr)
        {
            return;
        }

        int height = root->height;
        for (int i = 0; i <= height; i++)
        {
            cout << "==>level: " << i << endl;
            preorder(root, i, 0);
        }
    }
    int Balanced(node<T>* start)
    {
        if (start == nullptr)
        {
            return 0;
        }
        return getHeight(start->left) - getHeight(start->right);
    }
    void is_balanced()
    {

        cout << "Balanced factor is: " << Balanced(root) << endl;
    }
};

