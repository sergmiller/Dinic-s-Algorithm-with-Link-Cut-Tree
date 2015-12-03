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


Node::Node(size_t key, size_t edgeWeight):key(key), sizeOfSubtree(1), leftChild(nullptr), rightChild(nullptr), parent(nullptr), link(nullptr), subtreeMinWeight(edgeWeight), removedWeightValue(0), treePtr(nullptr), edgeWeight(edgeWeight) {
}

void Node::recursiveDelete(Node* vertex) {
    if(vertex) {
        Node::recursiveDelete(vertex->leftChild);
        Node::recursiveDelete(vertex->rightChild);
        delete vertex;
    }
}

void Node::removeWeight(size_t value, Node* vertex) {
    if(vertex) {
        vertex->removedWeightValue += value;
    }
}

void Node::push(Node* vertex) {
    if(vertex) {
        vertex->edgeWeight -= vertex->removedWeightValue;
        Node::removeWeight(vertex->removedWeightValue, vertex->leftChild);
        Node::removeWeight(vertex->removedWeightValue, vertex->rightChild);
        vertex->removedWeightValue = 0;
        Node::updateNodeParams(vertex);
    }
}

SplayTree::SplayTree(Node* root) {
    _root = root;
    if(root) {
        root->treePtr = this;
    }
}

SplayTree::~SplayTree() {
    Node::recursiveDelete(_root);
}

void SplayTree::_setParent(Node* vertex, Node* parent) {
    if(vertex) {
        vertex->parent = parent;
    }
}

inline size_t Node::getSize(Node* vertex) {
    return (vertex ? vertex->sizeOfSubtree : 0);
}

inline size_t Node::getMin(Node* vertex) {
    return (vertex ? vertex->subtreeMinWeight - vertex->removedWeightValue : INF);
}

size_t Node::getKey(Node* vertex) {
    return (vertex ? vertex->key : INF);
}

size_t Node::getWeight(Node* vertex) {
    return (vertex ? vertex->edgeWeight : INF);
}



void Node::updateNodeParams(Node* vertex) {
    if(vertex) {
        vertex->sizeOfSubtree = getSize(vertex->leftChild) + getSize(vertex->rightChild) + 1;
        vertex->subtreeMinWeight = min(min(getMin(vertex->leftChild), getMin(vertex->rightChild)), vertex->edgeWeight);
    }
}

void SplayTree::_keepParent(Node* vertex) {
    _setParent(vertex->leftChild, vertex);
    _setParent(vertex->rightChild, vertex);
    Node::updateNodeParams(vertex);
}

void SplayTree::_rotate(Node* parent, Node* vertex) {
    Node* grandParent = parent->parent;
    
    Node::push(grandParent);
    Node::push(parent);
    Node::push(vertex);
    
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
    
    _keepParent(parent);
    _keepParent(vertex);
    
    _setParent (vertex, grandParent);
}

void SplayTree::splay(Node* vertex){
    while(true) {
        if(!vertex->parent) {
            _root = vertex;
            _root->treePtr = this;
            return;
        }
        
        Node* parent = vertex->parent;
        Node* grandParent = parent->parent;
        
        if(!grandParent) {
            _rotate(parent, vertex);
            _root = vertex;
            _root->treePtr = this;
            return;
        }
        
        bool zigZigFlag = ((grandParent->leftChild == parent) == (parent->leftChild == vertex));
        
        if(zigZigFlag) {
            _rotate(grandParent, parent);
            _rotate(parent, vertex);
        } else {
            _rotate(parent, vertex);
            _rotate(grandParent, vertex);
        }
    }
}

Node* SplayTree::find(size_t position) {
    size_t treeSize = Node::getSize(_root);
    
    if(position >= treeSize) {
        return NULL;
        // throw std::out_of_range("out of range in SplayTree::find\n");
    }
    
    return _find(position, _root);
}

Node* SplayTree::_find(size_t position, Node* vertex) {
    Node::push(vertex);
    
    size_t indexLeft = Node::getSize(vertex->leftChild);
    
    if(position == indexLeft) {
        splay(vertex);
        return vertex;
    }
    
    if(position < indexLeft) {
        return _find(position, vertex->leftChild);
    }
    
    return _find(position - indexLeft - 1, vertex->rightChild);
}

std::pair<SplayTree*, SplayTree*> SplayTree::split(SplayTree* tree,size_t position) {
    SplayTree* leftTree = nullptr;
    SplayTree* rightTree = nullptr;
    if(tree) {
        rightTree = tree->_split(position);
        leftTree = tree;
    }
    return std::make_pair(leftTree, rightTree);
}


SplayTree* SplayTree::_split(size_t position){
    size_t treeSize = (_root ? _root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        return NULL;
        //    throw std::out_of_range("out of range in SplayTree::split\n");
    }
    
    if(position == treeSize) {
        return new SplayTree(nullptr);
    }
    
    Node* newRoot = _find(position, _root);
    
    SplayTree* rightTree = new SplayTree(newRoot);
    
    _root = newRoot->leftChild;
    newRoot->leftChild = nullptr;
    _setParent(_root, nullptr);
    
    if(rightTree->_root) {
        rightTree->_root->treePtr = rightTree;
    }
    
    Node::push(rightTree->_root);
    Node::push(_root);
    
    return rightTree;
}

SplayTree* SplayTree::merge(SplayTree* leftTree, SplayTree* rightTree) {
    if(!leftTree) {
        return rightTree;
    }
    
    leftTree->_merge(rightTree);
    
    return leftTree;
}

void SplayTree::_merge(SplayTree* addedTree) {
    if(!addedTree->_root)
    {
        delete addedTree;
        addedTree = nullptr;
    }
    
    
    if(!_root) {
        _root = addedTree->_root;
        addedTree->_root = nullptr;
        delete addedTree;
        addedTree = nullptr;
        return;
    }
    
    find(_root->sizeOfSubtree - 1);
    addedTree->find(0);
    
    Node::push(_root);
    
    _root->rightChild = addedTree->_root;
    addedTree->_root = nullptr;
    delete addedTree;
    addedTree = nullptr;
    _keepParent(_root);
}
