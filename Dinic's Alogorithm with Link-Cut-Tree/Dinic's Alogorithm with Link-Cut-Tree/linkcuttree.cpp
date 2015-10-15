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

Node* LinkCutTree::lca(Node* vertex1, Node* vertex2) {
    if(vertex1 == vertex2) {
        return vertex1;
    }
    Node* candidate1 = getLca(vertex1, vertex2);
    Node* candidate2 = getLca(vertex2, vertex1);
    if(candidate1 == candidate2) {
        return candidate1;
    } else {
        if(candidate2 == vertex2 || candidate2 == vertex1) {
            return candidate2;
        } else {
            return candidate1;
        }
    }
}

Node* LinkCutTree::getLca(Node* vertex1, Node* vertex2) {
    if(!vertex1 || !vertex2) {
        return nullptr;
    }
    
    expose(vertex1);
    expose(vertex2);
    return leftest(supportRoot(vertex1))->link;
}

size_t LinkCutTree::getDist(Node* vertex1, Node* vertex2) {
    size_t dist = 0;
    
    if(vertex1 == vertex2) {
        return dist;
    }
    
    Node* lcaVert = lca(vertex1, vertex2);
    
    if(!lcaVert) {
        return SIZE_T_MAX;
    }
    
    if(lcaVert == vertex2) {
        std::swap(vertex1, vertex2);
    }
    
    expose(vertex2);
    expose(vertex1);
    expose(lcaVert);
    
    dist += sumWeight(supportRoot(vertex2));
    
    if(vertex1 != lcaVert) {
        dist += sumWeight(supportRoot(vertex1));
    }
    
    return dist;
}

Node* LinkCutTree::getMinEdge(Node* vertex) {
    expose(findRoot(vertex));
    supportRoot(vertex);
    vertex->push();
    size_t minValue = minWeight(vertex);
    return findLeftestMin(minValue, vertex);
}

Node* LinkCutTree::findLeftestMin(size_t minValue, Node* vertex) {
    vertex->push();
    if(minWeight(vertex->leftChild) == minValue) {
        return findLeftestMin(minValue, vertex->leftChild);
    }
    
    if(minWeight(vertex) == minValue) {
        return vertex;
    }
    
    return findLeftestMin(minValue, vertex->rightChild);
}

void LinkCutTree::setWeight(Node* vertex, size_t weight) {
    supportRoot(vertex);
    vertex->edgeWeight = weight;
    vertex->treePtr->updateTreeSize(vertex);
    expose(vertex);
}

void LinkCutTree::removeWeightInPath(size_t added, Node* vertex) {
    expose(findRoot(vertex));
    supportRoot(vertex);
    vertex->removedWeightValue += added;
}

size_t LinkCutTree::getEdgeWeight(Node* vertex) {
    supportRoot(vertex);
    size_t edgeWeight = vertex->edgeWeight;
    expose(vertex);
    return edgeWeight;
}
