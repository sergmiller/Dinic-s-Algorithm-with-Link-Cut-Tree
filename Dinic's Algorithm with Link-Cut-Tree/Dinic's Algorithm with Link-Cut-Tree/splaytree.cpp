//
//  splaytree.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 10.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include "splaytree.h"
#include <cstdio>
#include <vector>

using std::min;

Node::Node(size_t key, size_t edgeWeight) {
    this->key = key;
    sizeOfSubtree = 1;
    leftChild = nullptr;
    rightChild = nullptr;
    parent = nullptr;
    link = nullptr;
    subtreeMinWeight = edgeWeight;
    removedWeightValue = 0;
    sizeOfSubtree = 1;
    treePtr = nullptr;
    
    this->edgeWeight = edgeWeight;
}

void Node::recursiveDelete() {
    if(leftChild) {
        leftChild->recursiveDelete();
    }
    
    if(rightChild) {
        rightChild->recursiveDelete();
    }
    
    delete this;
}

void removeWeight(size_t value, Node* vertex) {
    if(vertex) {
        vertex->removedWeightValue += value;
    }
}

void Node::push() {
    edgeWeight -= removedWeightValue;
    removeWeight(removedWeightValue, leftChild);
    removeWeight(removedWeightValue, rightChild);
    removedWeightValue = 0;
    updateTreeSize(this);
}

SplayTree::SplayTree(Node* root) {
    this->root = root;
    if(root) {
        root->treePtr = this;
    }
}

SplayTree::~SplayTree() {
    if(root) {
        root->recursiveDelete();
    }
}

void SplayTree::setParent(Node* vertex, Node* parent) {
    if(vertex) {
        vertex->parent = parent;
    }
}

size_t getSize(Node* vertex) {
    if(vertex) {
        return vertex->sizeOfSubtree;
    }
    return 0;
}

size_t getMin(Node* vertex) {
    if(vertex) {
        return vertex->subtreeMinWeight - vertex->removedWeightValue;
    }
    return INF;
}

void updateTreeSize(Node* vertex) {
    if(vertex) {
        vertex->sizeOfSubtree = getSize(vertex->leftChild) + getSize(vertex->rightChild) + 1;
        vertex->subtreeMinWeight = min(min(getMin(vertex->leftChild), getMin(vertex->rightChild)), vertex->edgeWeight);
    }
}

void SplayTree::keepParent(Node* vertex) {
    setParent(vertex->leftChild, vertex);
    setParent(vertex->rightChild, vertex);
    updateTreeSize(vertex);
    //vertex->push();
}

void SplayTree::rotate(Node* parent, Node* vertex) {
    Node* grandParent = parent->parent;
    
    if(grandParent) {
        grandParent->push();
    }
    
    parent->push();
    vertex->push();
    
    if(grandParent) {
        if(grandParent->leftChild == parent) {
            grandParent->leftChild = vertex;
        } else {
            grandParent->rightChild = vertex;
        }
    }
    
    if(parent->leftChild == vertex) {
        parent->leftChild = vertex->rightChild;
        vertex->rightChild = parent;
    } else {
        parent->rightChild = vertex->leftChild;
        vertex->leftChild = parent;
    }
    
    keepParent(parent);
    keepParent(vertex);
    
    //updateTreeSize(vertex);
    //updateTreeSize(grandParent);
    
    setParent (vertex, grandParent);
}

void SplayTree::splay(Node* vertex){
    if(!vertex->parent) {
        root = vertex;
        root->treePtr = this;
        return;
    }
    
    Node* parent = vertex->parent;
    Node* grandParent = parent->parent;
    
    if(!grandParent) {
        rotate(parent, vertex);
        root = vertex;
        root->treePtr = this;
        return;
    }
    
    //grandParent->push();
    //parent->push();
    //vertex->push();
    
    bool zigZigFlag = ((grandParent->leftChild == parent) == (parent->leftChild == vertex));
    
    if(zigZigFlag) {
        rotate(grandParent, parent);
        rotate(parent, vertex);
    } else {
        rotate(parent, vertex);
        rotate(grandParent, vertex);
    }
    
    splay(vertex);
    return;
}

Node* SplayTree::find(size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position >= treeSize) {
        return NULL;
        // throw std::out_of_range("out of range in SplayTree::find\n");
    }
    
    return find(position, root);
}

Node* SplayTree::find(size_t position, Node* vertex) {
    vertex->push();
    size_t indexLeft = (vertex->leftChild ? vertex->leftChild->sizeOfSubtree : 0);
    
    if(position == indexLeft) {
        splay(vertex);
        return vertex;
    }
    
    if(position < indexLeft) {
        return find(position, vertex->leftChild);
    }
    
    return find(position - indexLeft - 1, vertex->rightChild);
}


SplayTree* SplayTree::split(size_t position){
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        return NULL;
        //    throw std::out_of_range("out of range in SplayTree::split\n");
    }
    
    if(position == treeSize) {
        return new SplayTree(nullptr);
    }
    
    Node* newRoot = find(position, root);
    
    SplayTree* rightTree = new SplayTree(newRoot);
    
    root = newRoot->leftChild;
    newRoot->leftChild = nullptr;
    setParent(root, nullptr);
    
    if(rightTree->root) {
        rightTree->root->treePtr = rightTree;
        rightTree->root->push();
    }
    
    if(root) {
        root->push();
    }
    
    // updateTreeSize(root);
    //updateTreeSize(rightTree->root);
    
    return rightTree;
}

void SplayTree::insert(int key, size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        return;
        //    throw std::out_of_range("out of range in SplayTree::insert\n");
    }
    
    SplayTree* rightTree = split(position);
    Node* newRoot = new Node(key);
    newRoot->treePtr = this;
    newRoot->leftChild = this->root;
    newRoot->rightChild = rightTree->root;
    root = newRoot;
    keepParent(root);
    
    rightTree->root = nullptr;
    
    delete rightTree;
    rightTree = nullptr;
}

void SplayTree::merge(SplayTree* addedTree) {
    if(!addedTree || !addedTree->root)
    {
        return;
    }
    
    if(!root) {
        root = addedTree->root;
        addedTree->root = nullptr;
        delete addedTree;
        addedTree = nullptr;
        return;
    }
    
    find(root->sizeOfSubtree - 1);
    addedTree->find(0);
    
    root->push();
    
    root->rightChild = addedTree->root;
    addedTree->root = nullptr;
    delete addedTree;
    addedTree = nullptr;
    keepParent(root);
}

void SplayTree::remove(size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position >= treeSize) {
        return;
        //throw std::out_of_range("out of range in SplayTree::remove\n");
    }
    
    find(position);
    SplayTree* leftTree = new SplayTree(root->leftChild);
    SplayTree* rightTree = new SplayTree(root->rightChild);
    
    setParent(leftTree->root, nullptr);
    setParent(rightTree->root, nullptr);
    leftTree->merge(rightTree);
    
    delete root;
    root = leftTree->root;
    leftTree->root = nullptr;
    
    if(root) {
        root->treePtr = this;
    }
    
    updateTreeSize(root);
    
    delete leftTree;
    delete rightTree;
}
