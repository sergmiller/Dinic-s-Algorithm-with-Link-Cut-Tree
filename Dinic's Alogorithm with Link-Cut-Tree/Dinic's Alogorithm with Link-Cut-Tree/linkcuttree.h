//
//  linkcuttree.h
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 12.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#ifndef linkcuttree_h
#define linkcuttree_h

#include <stdio.h>
#include <vector>
#include "splaytree.h"
#include <map>

class LinkCutTree {
private:
    //std::vector <SplayTree>* trees;
    //std::map <Node*, size_t> numberOfTree;
    SplayTree* getTree(Node* vertex);
    Node* cutout(Node* vertex);
    Node* cleanUp(Node* vertex);
    Node* getLca(Node* vertex1, Node* vertex2);
    Node* findLeftestMin(size_t minValue, Node* vertex);
public:
    Node* lastEx;
    // LinkCutTree(std::vector<SplayTree>* trees);
    ~LinkCutTree();
    void makeTree(Node* vertex);
    //void revert(Node* vertex);
    void removeWeightInPath(size_t weight, Node* vertex);
    void cut(Node* vertex, Node* parent);
    void link(Node* treeRoot, Node* vertex);
    void linkEdge(Node* vertex1, Node* vertex2);
    void cutEdge(Node* vertex1, Node* vertex2);
    void setWeight(Node* vertex, size_t weight);
    size_t getEdgeWeight(Node* vertex);
    size_t getDist(Node* vertex1, Node* vertex2);
    Node* lca(Node* vertex1, Node* vertex2);
    Node* supportRoot(Node* vertex);   //it's splay current vertex
    Node* getMinEdge(Node* vertex);
    Node* findRoot(Node* vertex);
    Node* leftest(Node* root);
    size_t dist(Node* vertex1, Node* vertex2);
    size_t depth(Node* vertex);
    
    Node* expose(Node* vertex);
};

#endif /* linkcuttree_h */