//
//  linkcuttree.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 12.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include "linkcuttree.h"
#include <fstream>
#include <iostream>


LinkCutTree::LinkCutTree(size_t sizeVert){
    nodes.resize(sizeVert, Node(0));
    for(size_t i = 0;i < nodes.size(); ++i) {
        new SplayTree(&nodes[i]);
        nodes[i].key = i;
    }
}

LinkCutTree::~LinkCutTree() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        if(nodes[i].parent == nullptr) {
            SplayTree* buff = nodes[i].treePtr;
            //assert(buff->_root = &nodes[i]);
            buff->_root = nullptr;
            delete buff;
        }
    }
}

void LinkCutTree::clearTrees() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        if(nodes[i].parent != nullptr) {
            nodes[i] = Node(i,0);
            nodes[i].treePtr = new SplayTree(&nodes[i]);
        } else {
            SplayTree* buff = nodes[i].treePtr;
            nodes[i] =  Node(i,0);
            nodes[i].treePtr = buff;
            //buff->_root = &nodes[i];
        }
    }
}

void LinkCutTree::link(size_t indRoot, size_t indVert) {
    Node* vertex = &nodes[indVert];
    Node* treeRoot = &nodes[indRoot];
    treeRoot->link = vertex;
    _expose(treeRoot);
}

void LinkCutTree::cut(size_t indVert, size_t indParent) {
    Node* vertex = &nodes[indVert];
    Node* parent = &nodes[indParent];
    _expose(parent);
    vertex->link = nullptr;
}

Node* LinkCutTree::findRoot(size_t ind) {
    Node* vertex = &nodes[ind];
    if(vertex != lastExposed) {
        _expose(vertex);
    }
    return _leftest(_liftUpToRoot(vertex));
}

Node* LinkCutTree::_cleanUp(Node* vertex) {
    Node* root;
    
    if(vertex->parent) {
        root = _cleanUp(vertex->parent);
    } else {
        root = vertex;
    }
    
    Node::push(vertex);
    
    return root;
}

inline Node* LinkCutTree::_liftUpToRoot(Node* vertex) {
    if(!vertex) {
        return nullptr;
    }
    
    if(!vertex->parent) {
        return vertex;
    }
    
    Node* root = _cleanUp(vertex);
    root->treePtr->splay(vertex);
    return vertex;
}

Node* LinkCutTree::_leftest(Node* root) {
    return root->treePtr->find(0);
}

Node* LinkCutTree::_cutout(Node* vertex) {
    _liftUpToRoot(vertex);
    std::pair<SplayTree*, SplayTree*> splitedTrees = SplayTree::split(vertex->treePtr, Node::getSize(vertex->leftChild) + 1);
    SplayTree* right = splitedTrees.second;
    if(right->getRoot()) {
        right->find(0)->link = vertex;
    } else {
        delete right;
    }
    return vertex;
}

Node* LinkCutTree::_expose(Node* vertex) {
    lastExposed = vertex;
    Node* next;
    vertex = _leftest(_liftUpToRoot(_cutout(vertex)));
    while(vertex->link != nullptr) {
        next = _cutout(vertex->link);
        vertex->link = nullptr;
        SplayTree::merge(_liftUpToRoot(next)->treePtr, _liftUpToRoot(vertex)->treePtr);
        vertex = _leftest(_liftUpToRoot(vertex));
    }
    return vertex;
}

Node* LinkCutTree::getMinEdge(size_t ind) {
    Node* vertex = &nodes[ind];
    _liftUpToRoot(vertex);
    size_t minValue = Node::getMin(vertex);
    return _findLeftestMin(minValue, vertex);
}

Node* LinkCutTree::_findLeftestMin(size_t minValue, Node* vertex) {
    Node::push(vertex);
    
    if(Node::getMin(vertex->leftChild) == minValue) {
        return _findLeftestMin(minValue, vertex->leftChild);
    }
    
    if(vertex->edgeWeight == minValue) {
        return vertex;
    }
    
    return _findLeftestMin(minValue, vertex->rightChild);
}

void LinkCutTree::setWeight(size_t indVert, size_t weight) {
    Node* vertex = &nodes[indVert];
    _liftUpToRoot(vertex);
    vertex->edgeWeight = weight;
    Node::updateNodeParams(vertex);
}

void LinkCutTree::removeWeightInPath(size_t added, size_t indVert) {
    Node::removeWeight(added, &nodes[indVert]);
}

size_t LinkCutTree::getEdgeWeight(size_t indVert) {
    Node* vertex = &nodes[indVert];
    _liftUpToRoot(vertex);
    Node::push(vertex);
    size_t edgeWeight = vertex->edgeWeight;
    return edgeWeight;
}

Node* LinkCutTree::prevInPath(size_t ind) {
    Node* source = &nodes[ind];
    _expose(findRoot(ind));
    return _leftest(_liftUpToRoot(source));
}