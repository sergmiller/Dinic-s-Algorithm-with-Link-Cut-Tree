//
//  linkcuttree.h
//  Dinic's Algorithm with Link-Cut-Tree
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
    friend class LinkCutBlockFlowFinder;
private:
    std::vector <Node> nodes;
    
    Node* _cutout(Node* vertex);
    Node* _leftest(Node* vertex);
    Node* _expose(Node* vertex);
    Node* _cleanUp(Node* vertex);
    Node* _liftUpToRoot(Node* vertex);   //it's splay current vertex
    Node* _findLeftestMin(size_t minValue, Node* vertex);
public:
    Node* lastExposed;
    LinkCutTree(size_t _size);
    ~LinkCutTree();
    
    void clearTrees();
    
    void removeWeightInPath(size_t weight, size_t ind);
    void link(size_t indRoot, size_t indVert);
    void cut(size_t indVert, size_t indParent);
    void setWeight(size_t indVert, size_t weight);
    
    size_t getEdgeWeight(size_t indVert);
    Node* prevInPath(size_t ind);
    Node* getMinEdge(size_t ind);
    Node* findRoot(size_t ind);
};


#endif /* linkcuttree_h */