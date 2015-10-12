//
//  linkcuttree.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 12.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include "linkcuttree.h"
/*
LinkCutTree::LinkCutTree() {
    trees = new std::vector <SplayTree>;
}

LinkCutTree::LinkCutTree(std::vector<SplayTree>* trees) {
    this->trees = trees;
}*/

LinkCutTree::~LinkCutTree() {
   /* for(int i  = 0;i < trees->size(); ++i) {
        (*trees)[i].~SplayTree();
    }*/
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
    expose(vertex);
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

Node* LinkCutTree::supportRoot(Node* vertex) {
    if(!vertex) {
        return nullptr;
    }
    
    Node* root = cleanUp(vertex);
    
    SplayTree* currentTree = root->treePtr;
    currentTree->splay(vertex);
    return vertex;
}

Node* LinkCutTree::leftest(Node* root) {
    return root->treePtr->find(0);
}

size_t LinkCutTree::depth(Node* vertex) {
    expose(vertex);
    return supportRoot(vertex)->sizeOfSubtree - 1;
}

Node* LinkCutTree::cutout(Node* vertex) {
    supportRoot(vertex);
    SplayTree* left = vertex->treePtr;
    SplayTree* right = left->split(size(vertex->leftChild) + 1);
    if(right->getRoot()) {
        right->find(0)->link = vertex;
    }
    return vertex;
}

Node* LinkCutTree::expose(Node* vertex) {
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

void LinkCutTree::revert(Node* vertex) {
    Node* root = expose(vertex);
    supportRoot(root);
    root->reverse();
}

void LinkCutTree::linkEdge(Node* vertex1, Node* vertex2) {
    revert(vertex1);
    link(vertex1, vertex2);
}

void LinkCutTree::cutEdge(Node* vertex1, Node* vertex2) {
    revert(vertex2);
    cut(vertex1, vertex2);
}

size_t LinkCutTree::dist(Node* vertex1, Node* vertex2) {
    revert(vertex2);
    return depth(vertex1);
}