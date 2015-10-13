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

Node::Node(int key, size_t weight) {
    this->key = key;
    this->sizeOfSubtree = 1;
    this->leftChild = nullptr;
    this->rightChild = nullptr;
    this->parent = nullptr;
    this->treePtr = nullptr;
    this->link = nullptr;
    this->reverseFlag = false;
    this->edgeWeight = weight;
    this->subtreeWeight = weight;
}
/*
 Node::Node() {
 this->key = 0;
 this->sizeOfSubtree = 1;
 this->leftChild = nullptr;
 this->rightChild = nullptr;
 this->parent = nullptr;
 this->treePtr = nullptr;
 this->reverseFlag = false;
 }*/

void Node::recursiveDelete() {
    if(leftChild) {
        leftChild->recursiveDelete();
    }
    
    if(rightChild) {
        rightChild->recursiveDelete();
    }
    
    this->~Node();
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

void Node::push() {
    if(reverseFlag) { //guarantee that this != nullptr
        std::swap(leftChild, rightChild);
        reverseFlag = false;
        
        reverse(leftChild);
        reverse(rightChild);
    }
}

void Node::reverse() {
    reverse(this);
}

void Node::reverse(Node* vertex) {
    if(vertex) {
        vertex->reverseFlag ^= true;
    }
}

void SplayTree::setParent(Node* vertex, Node* parent) {
    if(vertex) {
        vertex->parent = parent;
    }
}

void SplayTree::updateTreeSize(Node* vertex) {
    if(vertex) {
        vertex->sizeOfSubtree = size(vertex->leftChild) + size(vertex->rightChild) + 1;
        vertex->subtreeWeight = weight(vertex->leftChild) + weight(vertex->rightChild) + vertex->edgeWeight;
    }
}

void SplayTree::keepParent(Node* vertex) {
    setParent(vertex->leftChild, vertex);
    setParent(vertex->rightChild, vertex);
}

void SplayTree::rotate(Node* parent, Node* vertex) {
    Node* grandParent = parent->parent;
    
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
    
    keepParent(vertex);
    keepParent(parent);
    
    updateTreeSize(parent);
    updateTreeSize(vertex);
    updateTreeSize(grandParent);
    
    setParent (vertex, grandParent);
}

void SplayTree::splay(Node* vertex){
    if(!vertex->parent) {
        root = vertex;
        vertex->treePtr = this;
        return;
    }
    
    Node* parent = vertex->parent;
    Node* grandParent = parent->parent;
    
    if(!grandParent) {
        rotate(parent, vertex);
        root = vertex;
        //setParent(vertex, nullptr);
        vertex->treePtr = this;
        return;
    }
    
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
        throw std::out_of_range("out of range in SplayTree::find\n");
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
        throw std::out_of_range("out of range in SplayTree::split\n");
    }
    
    if(position == treeSize) {
        return new SplayTree(nullptr);
    }
    
    Node* newRoot = find(position, root);
    
    SplayTree* rightTree = new SplayTree(newRoot);
    
    root = newRoot->leftChild;
    newRoot->leftChild = nullptr;
    setParent(root, nullptr);
    
    updateTreeSize(root);
    updateTreeSize(rightTree->root);
    
    return rightTree;
}

void SplayTree::insert(int key, size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        throw std::out_of_range("out of range in SplayTree::insert\n");
    }
    
    SplayTree* rightTree = split(position);
    Node* newRoot = new Node(key);
    newRoot->leftChild = this->root;
    newRoot->rightChild = rightTree->root;
    root = newRoot;
    keepParent(root);
    
    updateTreeSize(root);
    
    rightTree->root = nullptr;
    
    rightTree->~SplayTree();
}

void SplayTree::merge(SplayTree* addedTree) {
    if(!addedTree || !addedTree->root)
    {
        return;
    }
    
    if(!root) {
        root = addedTree->root;
        addedTree->root = nullptr;
        return;
    }
    
    find(this->root->sizeOfSubtree - 1);
    addedTree->find(0);
    
    this->root->rightChild = addedTree->root;
    addedTree->root = nullptr;
    keepParent(this->root);
    updateTreeSize(this->root);
}

void SplayTree::remove(size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position >= treeSize) {
        throw std::out_of_range("out of range in SplayTree::remove\n");
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
    
    updateTreeSize(root);
    
    leftTree->~SplayTree();
    rightTree->~SplayTree();
}

size_t size(Node* vertex) {
    return (vertex ? vertex->sizeOfSubtree : 0);
}

size_t weight(Node* vertex) {
    return (vertex ? vertex->subtreeWeight : 0);
}
