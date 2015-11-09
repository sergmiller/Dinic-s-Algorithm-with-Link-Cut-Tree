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



LinkCutTree::~LinkCutTree() {
}

void LinkCutTree::link(Node* treeRoot, Node* vertex) {
    treeRoot->link = vertex;
    expose(treeRoot);
}

void LinkCutTree::cut(Node* vertex, Node* parent) {
    expose(parent);
    vertex->link = nullptr;
    //expose(vertex);
}

Node* LinkCutTree::findRoot(Node* vertex) {
    if(vertex != lastExposed) {
        expose(vertex);
    }
    return leftest(supportRoot(vertex));
}

Node* LinkCutTree::cleanUp(Node* vertex) {
    Node* root;
    if(vertex->parent) {
        root = cleanUp(vertex->parent);
    } else {
        root = vertex;
    }
    
    vertex->push();
    return root;
}

inline Node* LinkCutTree::supportRoot(Node* vertex) {
    if(!vertex) {
        return nullptr;
    }
    
    if(!vertex->parent) {
        return vertex;
    }
    
    Node* root = cleanUp(vertex);
    root->treePtr->splay(vertex);
    return vertex;
}

Node* LinkCutTree::leftest(Node* root) {
    return root->treePtr->find(0);
}

Node* LinkCutTree::cutout(Node* vertex) {
    supportRoot(vertex);
    SplayTree* left = vertex->treePtr;
    SplayTree* right = left->split(getSize(vertex->leftChild) + 1);
    if(right->getRoot()) {
        right->find(0)->link = vertex;
    }
    return vertex;
}

//inline Node* LinkCutTree::expose(Node* vertex) {
//    Node* memVert = vertex;
//    Node* next;
//    vertex = leftest(cutout(vertex));
//    while(vertex->link != nullptr) {
//        next = cutout(vertex->link);
//        vertex->link = nullptr;
//        (next->treePtr)->merge(vertex->treePtr);//<---!!!
//        vertex = leftest(supportRoot(vertex));
//    }
//    return supportRoot(memVert);
//}

Node* LinkCutTree::expose(Node* vertex) {
    lastExposed = vertex;
    Node* next;
    vertex = leftest(supportRoot(cutout(vertex)));
    while(vertex->link != nullptr) {
        next = cutout(vertex->link);
        vertex->link = nullptr;
        (supportRoot(next)->treePtr)->merge(supportRoot(vertex)->treePtr);
        vertex = leftest(supportRoot(vertex));
    }
    return vertex;
}

Node* LinkCutTree::getMinEdge(Node* vertex) {
    expose(vertex);
    //expose(findRoot(vertex));
    supportRoot(vertex);
    //vertex->push();
    size_t minValue = getMin(vertex);
    return findLeftestMin(minValue, vertex);
}

Node* LinkCutTree::findLeftestMin(size_t minValue, Node* vertex) {
    vertex->push();
    if(getMin(vertex->leftChild) == minValue) {
        return findLeftestMin(minValue, vertex->leftChild);
    }
    
    if(vertex->edgeWeight == minValue) {
        //expose(vertex);
        return supportRoot(vertex);
    }
    
    return findLeftestMin(minValue, vertex->rightChild);
}

void LinkCutTree::setWeight(Node* vertex, size_t weight) {
    supportRoot(vertex);
    vertex->edgeWeight = weight;
    updateTreeSize(vertex);
    //expose(vertex);
}

void LinkCutTree::removeWeightInPath(size_t added, Node* vertex) {
    expose(vertex);
    //expose(findRoot(vertex));
    supportRoot(vertex);
    removeWeight(added, vertex);
}

size_t LinkCutTree::getEdgeWeight(Node* vertex) {
    supportRoot(vertex);
    vertex->push();
    size_t edgeWeight = vertex->edgeWeight;
    //expose(vertex);
    return edgeWeight;
}
