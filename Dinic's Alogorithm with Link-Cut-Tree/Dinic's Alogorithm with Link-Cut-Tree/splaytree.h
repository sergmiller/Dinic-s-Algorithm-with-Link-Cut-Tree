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

class Node{
public:
    Node(int key);
    
    int key;
    int edgeWeight;
    Node* leftChild;
    Node* rightChild;
    Node* parent;
    size_t sizeOfSubtree;
    
    void recursiveDelete();
};

class SplayTree
{
private:
    Node* find(size_t position, Node* vertex);
    void updateTreeSize(Node* vertex);
    Node* root;
public:
    SplayTree(Node* root);
    ~SplayTree();
    
    void insert(int key, size_t position);
    void remove(size_t position);
    /*added tree is right merged tree*/
    void merge(SplayTree* addedTree);
    /*returned tree is tight splited tree*/
    SplayTree* split(size_t position);
    Node* find(size_t position);
    
    void splay(Node* vertex);
    
    void keepParent(Node* vertex);
    void setParent(Node* parent, Node* vertex);
    
    void rotate(Node* parent, Node* vertex);
    
    Node* getRoot() { return root; };
};

#endif /* splaytree_h */
