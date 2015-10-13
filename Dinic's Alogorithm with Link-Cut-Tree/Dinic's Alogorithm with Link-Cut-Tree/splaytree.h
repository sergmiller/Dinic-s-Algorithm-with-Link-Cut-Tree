//
//  splaytree.h
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 10.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#ifndef splaytree_h
#define splaytree_h

#include <cstdio>

class Node;
class SplayTree;

size_t size(Node* vertex);
size_t weight(Node* vertex);

class Node{
public:
    Node(int key, size_t weight = 0);
    
    int key;
    size_t edgeWeight;
    size_t subtreeWeight;
    bool reverseFlag;
    Node* leftChild;
    Node* rightChild;
    Node* parent;
    Node* link;
    size_t sizeOfSubtree;
    SplayTree* treePtr;
    
    void reverse();
    void reverse(Node* vertex);
    void push();
    void recursiveDelete();
};

class SplayTree
{
private:
    Node* find(size_t position, Node* vertex);
    void updateTreeSize(Node* vertex);
    void keepParent(Node* vertex);
    void setParent(Node* parent, Node* vertex);
    void rotate(Node* parent, Node* vertex);
    Node* root;
public:
    SplayTree(Node* root);
    ~SplayTree();
    
    void insert(int key, size_t position);
    void remove(size_t position);
    void merge(SplayTree* addedTree); //added tree is right merged tree
    SplayTree* split(size_t position); //returned tree is tight splited tree
    Node* find(size_t position);
    
    void splay(Node* vertex);
    
    Node* getRoot() { return root; };
};

#endif /* splaytree_h */