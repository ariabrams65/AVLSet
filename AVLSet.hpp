// AVLSet.hpp
//
// ICS 46 Spring 2022
// Project #4: Set the Controls for the Heart of the Sun
//
// An AVLSet is an implementation of a Set that is an AVL tree, which uses
// the algorithms we discussed in lecture to maintain balance every time a
// new element is added to the set.  The balancing is actually optional,
// with a bool parameter able to be passed to the constructor to explicitly
// turn the balancing on or off (on is default).  If the balancing is off,
// the AVL tree acts like a binary search tree (e.g., it will become
// degenerate if elements are added in ascending order).
//
// You are not permitted to use the containers in the C++ Standard Library
// (such as std::set, std::map, or std::vector) to store the information
// in your data structure.  Instead, you'll need to implement your AVL tree
// using your own dynamically-allocated nodes, with pointers connecting them,
// and with your own balancing algorithms used.

#ifndef AVLSET_HPP
#define AVLSET_HPP

#include <functional>
#include "Set.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>


namespace
{
    enum Rotation {LL, LR, RL, RR};
}


template <typename ElementType>
class AVLSet : public Set<ElementType>
{
public:
    // A VisitFunction is a function that takes a reference to a const
    // ElementType and returns no value.
    using VisitFunction = std::function<void(const ElementType&)>;

public:
    // Initializes an AVLSet to be empty, with or without balancing.
    explicit AVLSet(bool shouldBalance = true);

    // Cleans up the AVLSet so that it leaks no memory.
    ~AVLSet() noexcept override;

    // Initializes a new AVLSet to be a copy of an existing one.
    AVLSet(const AVLSet& s);

    // Initializes a new AVLSet whose contents are moved from an
    // expiring one.
    AVLSet(AVLSet&& s) noexcept;

    // Assigns an existing AVLSet into another.
    AVLSet& operator=(const AVLSet& s);

    // Assigns an expiring AVLSet into another.
    AVLSet& operator=(AVLSet&& s) noexcept;


    // isImplemented() should be modified to return true if you've
    // decided to implement an AVLSet, false otherwise.
    bool isImplemented() const noexcept override;


    // add() adds an element to the set.  If the element is already in the set,
    // this function has no effect.  This function always runs in O(log n) time
    // when there are n elements in the AVL tree.
    void add(const ElementType& element) override;


    // contains() returns true if the given element is already in the set,
    // false otherwise.  This function always runs in O(log n) time when
    // there are n elements in the AVL tree.
    bool contains(const ElementType& element) const override;


    // size() returns the number of elements in the set.
    unsigned int size() const noexcept override;


    // height() returns the height of the AVL tree.  Note that, by definition,
    // the height of an empty tree is -1.
    int height() const noexcept;


    // preorder() calls the given "visit" function for each of the elements
    // in the set, in the order determined by a preorder traversal of the AVL
    // tree.
    void preorder(VisitFunction visit) const;


    // inorder() calls the given "visit" function for each of the elements
    // in the set, in the order determined by an inorder traversal of the AVL
    // tree.
    void inorder(VisitFunction visit) const;


    // postorder() calls the given "visit" function for each of the elements
    // in the set, in the order determined by a postorder traversal of the AVL
    // tree.
    void postorder(VisitFunction visit) const;


private:
    struct Node
    {
        Node* left;
        Node* right;
        ElementType value;
        int height;
    };

    Node* _root;
    int _sz;
    bool _shouldBalance;

    void updateDepth(int depth);

    void preorderR(VisitFunction visit, Node* t) const;

    void postorderR(VisitFunction visit, Node* t) const;

    void inorderR(VisitFunction visit, Node* t) const;

    void deleteTree(Node* t) noexcept;

    Node* copyTree(Node* t);

    Node* addR(Node* t, const ElementType& element, bool& exists);

    int getHeight(Node* t) const;

    Rotation getNeededRotation(Node* t, const ElementType& element);

    Node* rotate(Node* t, Rotation r);

    Node* rotLL(Node* t);
    
    Node* rotLR(Node* t);

    Node* rotRL(Node* t);

    Node* rotRR(Node* t);
};


template <typename ElementType>
AVLSet<ElementType>::AVLSet(bool shouldBalance)
    : _root{nullptr}, _sz{0}, _shouldBalance{shouldBalance}
{
}


template <typename ElementType>
void AVLSet<ElementType>::deleteTree(Node* t) noexcept
{
    if (t == nullptr)
    {
        return;
    }
    if (t->left != nullptr)
    {
        deleteTree(t->left); 
    }
    if (t->right != nullptr)
    {
        deleteTree(t->right); 
    }
    delete t;
}


template <typename ElementType>
AVLSet<ElementType>::~AVLSet() noexcept
{
    deleteTree(_root); 
}


template <typename ElementType>
typename AVLSet<ElementType>::Node* AVLSet<ElementType>::copyTree(Node* t)
{
    if (t == nullptr)
    {
        return nullptr;
    }

    Node* copy = new Node{nullptr, nullptr, t->value, t->height};
    copy->left = copyTree(t->left);
    copy->right = copyTree(t->right);

    return copy;
}


template <typename ElementType>
AVLSet<ElementType>::AVLSet(const AVLSet& s)
    :_sz{s._sz}, _shouldBalance{s._shouldBalance}
{
    _root = copyTree(s._root); 
}


template <typename ElementType>
AVLSet<ElementType>::AVLSet(AVLSet&& s) noexcept
    :_root{nullptr}, _sz{0}, _shouldBalance{false}
{
    std::swap(_root, s._root);
    std::swap(_sz, s._sz);
    std::swap(_shouldBalance, s._shouldBalance);
}


template <typename ElementType>
AVLSet<ElementType>& AVLSet<ElementType>::operator=(const AVLSet& s)
{
    deleteTree(_root);
    _sz = s._sz;
    _shouldBalance = s._shouldBalance;
    _root = copyTree(s._root);

    return *this;
}


template <typename ElementType>
AVLSet<ElementType>& AVLSet<ElementType>::operator=(AVLSet&& s) noexcept
{
    std::swap(_root, s._root);
    std::swap(_sz, s._sz);
    std::swap(_shouldBalance, s._shouldBalance);

    return *this;
}


template <typename ElementType>
bool AVLSet<ElementType>::isImplemented() const noexcept
{
    return true;
}


template <typename ElementType>
int AVLSet<ElementType>::getHeight(Node* t) const
{
    if (t == nullptr)
    {
        return -1;
    }
    return t->height;
}


template <typename ElementType>
Rotation AVLSet<ElementType>::getNeededRotation(Node* t, const ElementType& element)
{
    if (element < t->value)
    {
        if (element < t->left->value)
        {
            return Rotation::LL;
        }
        return Rotation::LR;
    }
    if (element < t->right->value)
    {
        return Rotation::RL;
    }
    return Rotation::RR;
}


template <typename ElementType>
typename AVLSet<ElementType>::Node* AVLSet<ElementType>::rotLL(Node* t)
{
    Node* a = t->left;
    Node* t2 = a->right;

    t->left = t2; 
    a->right = t;

    t->height = 1 + std::max(getHeight(t2) , getHeight(t->right));     
    a->height = 1 + std::max(getHeight(a->left), getHeight(t));

    return a; 
}


template <typename ElementType>
typename AVLSet<ElementType>::Node* AVLSet<ElementType>::rotLR(Node* t)
{
    Node* a = t->left; 
    Node* b = a->right;
    Node* t2 = b->left;
    Node* t3 = b->right;

    a->right = t2;
    t->left = t3;
    b->left = a;
    b->right = t;

    a->height = 1 + std::max(getHeight(a->left), getHeight(t2));
    t->height = 1 + std::max(getHeight(t3), getHeight(t->right));
    b->height = 1 + std::max(getHeight(a), getHeight(t));

    return b;
}


template <typename ElementType>
typename AVLSet<ElementType>::Node* AVLSet<ElementType>::rotRL(Node* t)
{
    Node* c = t->right;
    Node* b = c->left;
    Node* t2 = b->left;
    Node* t3 = b->right;

    t->right = t2;
    c->left = t3;
    b->left = t;
    b->right = c;

    t->height = 1 + std::max(getHeight(t->left), getHeight(t2));
    c->height = 1 + std::max(getHeight(t3), getHeight(c->right));
    b->height = 1 + std::max(getHeight(t), getHeight(c));

    return b;
}


template <typename ElementType>
typename AVLSet<ElementType>::Node* AVLSet<ElementType>::rotRR(Node* t)
{
    Node* b = t->right;
    Node* t2 = b->left; 

    t->right = t2;
    b->left = t;

    t->height = 1 + std::max(getHeight(t->left), getHeight(t2));
    b->height = 1 + std::max(getHeight(t), getHeight(b->right));

    return b;
}


template <typename ElementType>
typename AVLSet<ElementType>::Node* AVLSet<ElementType>::rotate(Node* t, Rotation r)
{
    if (r == Rotation::LL)
    {
        return rotLL(t);
    } else if (r == Rotation::LR)
    {
        return rotLR(t);
    } else if (r == Rotation::RL)
    {
        return rotRL(t);
    } else
    {
        return rotRR(t);
    }
}


template <typename ElementType>
typename AVLSet<ElementType>::Node* AVLSet<ElementType>::addR(Node* t, const ElementType& element,
    bool& exists)
{
    if(t == nullptr)
    {
        return new Node{nullptr, nullptr, element, 0};
    }
    if (t->value == element)
    {
        exists = true;
        return t;
    }
    if (element < t->value)
    {
        t->left = addR(t->left, element, exists);
    } else
    {
        t->right = addR(t->right, element, exists);
    }
    if (!exists)
    {
        int newHeight = 1 + std::max(getHeight(t->left), getHeight(t->right));
        if (newHeight > t->height)
        {
            t->height += 1; 
        }

        if (abs(getHeight(t->left) - getHeight(t->right)) > 1 && _shouldBalance)
        {
            Rotation r = getNeededRotation(t, element); 
            t = rotate(t, r);
        }
    }
    
    return t;
}

template <typename ElementType>
void AVLSet<ElementType>::add(const ElementType& element)
{
    bool exists = false;
    _root = addR(_root, element, exists); 
    
    if (!exists)
    {
        ++_sz;
    }
}


template <typename ElementType>
bool AVLSet<ElementType>::contains(const ElementType& element) const
{
    Node* cur = _root; 
    while (cur != nullptr)
    {
        if (cur->value == element)
        {
            return true;
        }
        if (element > cur->value)
        {
            cur = cur->right;
        } else
        {
            cur = cur->left;
        }
    }
    return false;
}


template <typename ElementType>
unsigned int AVLSet<ElementType>::size() const noexcept
{
    return _sz;
}


template <typename ElementType>
int AVLSet<ElementType>::height() const noexcept
{
    return getHeight(_root);
}


template <typename ElementType>
void AVLSet<ElementType>::preorderR(VisitFunction visit, Node* t) const
{
    visit(t->value);

    if (t->left != nullptr)
    {
        preorderR(visit, t->left);
    }
    if (t->right != nullptr)
    {
        preorderR(visit, t->right);
    }
}


template <typename ElementType>
void AVLSet<ElementType>::preorder(VisitFunction visit) const
{
    preorderR(visit, _root);
}


template <typename ElementType>
void AVLSet<ElementType>::inorderR(VisitFunction visit, Node* t) const
{
    if (t != nullptr)
    {
        inorderR(visit, t->left);
        visit(t->value);
        inorderR(visit, t->right);
    }
}


template <typename ElementType>
void AVLSet<ElementType>::inorder(VisitFunction visit) const
{ 
    inorderR(visit, _root);
}


template <typename ElementType>
void AVLSet<ElementType>::postorderR(VisitFunction visit, Node* t) const
{
    if (t->left != nullptr)
    {
        postorderR(visit, t->left);
    }
    if (t->right != nullptr)
    {
        postorderR(visit, t->right);
    }
    visit(t->value);
}


template <typename ElementType>
void AVLSet<ElementType>::postorder(VisitFunction visit) const
{
    postorderR(visit, _root);
}


#endif

