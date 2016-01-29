//
//  splaytree.h
//  Dinic's Algorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 10.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#ifndef splaytree_h
#define splaytree_h

#include <cstdio>
#include <vector>

using std::min;
using std::max;

class Node;
class SplayTree;

const size_t INF = 1e16;

class Node{
    friend class SplayTree;
    friend class LinkCutTree;
public:
    static void removeWeight(size_t value, Node* vertex);
    static void updateNodeParams(Node* vertex);
    static void recursiveDelete(Node* vertex);
    static void push(Node* vertex);
    static void reverse(Node* vertex);
    
    static size_t getSize(Node* vertex);
    static size_t getMin(Node* vertex);
    static size_t getMax(Node* vertex);
    static size_t getSum(Node* vertex);
    static size_t getKey(Node* vertex);
    static size_t getWeight(Node* vertex);
    static size_t getSubtreeWeight(Node* vertex);
    
    Node(size_t key, size_t weight = 0);
private:
    size_t key;
    size_t edgeWeight;
    size_t subtreeWeight;
    size_t sizeOfSubtree;
    size_t subtreeMaxWeight;
    size_t subtreeMinWeight;
    size_t removedWeightValue;
    
    bool reverseFlag;
    
    Node* leftChild;
    Node* rightChild;
    Node* parent;
    Node* link;
    
    SplayTree* treePtr;
};

class SplayTree
{
    friend class LinkCutTree;
private:
    Node* _find(size_t position, Node* vertex);
    void insert(int key, int position);
    void remove(int position);
    void _keepParent(Node* vertex);
    void _setParent(Node* parent, Node* vertex);
    void _rotate(Node* parent, Node* vertex);
    void _merge(SplayTree* addedTree); //added tree is right merged tree
    SplayTree* _split(size_t position); //returned tree is tight splited tree
    
    Node* _root;
public:
    SplayTree(Node* root);
    ~SplayTree();
    
    Node* find(size_t position);
    
    static SplayTree* merge(SplayTree* leftTree, SplayTree* rightTree);
    static std::pair<SplayTree*, SplayTree*> split(SplayTree* tree,size_t position);
    
    void splay(Node* vertex);
    
    Node* getRoot() { return _root; };
};


#endif /* splaytree_h */