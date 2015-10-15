//
//  main.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 10.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include <iostream>
#include "splaytree.h"
#include "linkcuttree.h"
#include "dinicmaxflow.h"
#include <vector>

void splayTest();
void linkCutTest();
void fullTest();
void solve();

int main() {
   // splayTest();
   // linkCutTest();
    solve();
    return 0;
}
/*
 10
 1 2 3 4 5 6 7 8 9 10
 3 6
 */

void splayTest() {
    int n,x,l,r;
    std::cin >> n;
    SplayTree tree(nullptr);
    for(int i = 0;i < n;++i) {
        std::cin >> x;
        tree.insert(x,i);
    }
    std::cin >> l >> r;
    //  tree.remove(2);
    
    //std::cout << std::endl;
    
    //tree.remove(poz);
    for(int i = 0;i < n;++i) {
        std::cout << tree.find(i)->key << " ";
    }
    
    std::cout << std::endl;
    
    SplayTree* rightTree = tree.split(r);
    SplayTree* midTree = tree.split(l);
    tree.merge(midTree);
    tree.merge(rightTree);
    
    for(int i = 0;i < n;++i) {
        std::cout << tree.find(i)->key << " ";
    }
    
    std::cout << std::endl;
}

void linkCutTest() {
    size_t weights[10] = {0, 5, 10, 2, 4, 2, 2, 2, 0, 0};
    std::vector <Node> nodes(10, *(new Node(0)));
    std::vector <SplayTree> trees;
    for(int i = 0;i < 10; ++i) {
        nodes[i].key = i;
        nodes[i].edgeWeight = weights[i];
        trees.push_back(*(new SplayTree(&nodes[i])));
    }
    LinkCutTree* tree = new LinkCutTree();
    
    tree->link(&nodes[1], &nodes[2]);
    tree->link(&nodes[3], &nodes[2]);
    tree->link(&nodes[4], &nodes[1]);
    tree->link(&nodes[5], &nodes[4]);
    tree->link(&nodes[6], &nodes[4]);
    tree->link(&nodes[7], &nodes[6]);
    for(int i = 1;i < 8;++i) {
      //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
      //  }
    }
    
    Node* lca61 = tree->lca(&nodes[6], &nodes[1]);
    Node* lca73 = tree->lca(&nodes[7], &nodes[3]);
    Node* lca57 = tree->lca(&nodes[5], &nodes[7]);
    Node* lca72 = tree->lca(&nodes[7], &nodes[2]);
    size_t dist61 = tree->getDist(&nodes[6], &nodes[1]);
    size_t dist73 = tree->getDist(&nodes[7], &nodes[3]);
    size_t dist57 = tree->getDist(&nodes[5], &nodes[7]);
    size_t dist72 = tree->getDist(&nodes[7], &nodes[2]);
    
    
    std::cout <<"lca(6,1): " << lca61->key << std::endl;
    std::cout <<"lca(7,3): " << lca73->key << std::endl;
    std::cout <<"lca(5,7): " << lca57->key << std::endl;
    std::cout <<"lca(7,2): " << lca72->key << std::endl;
   
    std::cout <<"dist(6,1): " << dist61 << std::endl;
    std::cout <<"dist(7,3): " << dist73 << std::endl;
    std::cout <<"dist(5,7): " << dist57 << std::endl;
    std::cout <<"dist(7,2): " << dist72 << std::endl;
    std::cout << "min_to_root_from_7: " << tree->getMinEdge(&nodes[7])->key << std::endl;
    tree->cut(&nodes[6], &nodes[4]);
   // tree->revert(&nodes[4]);
   // tree->supportRoot(&nodes[6]);
    //nodes[6].edgeWeight = 8;
    //nodes[6].treePtr->updateTreeSize(7nodes[6]);
    tree->link(&nodes[6], &nodes[3]);
   //
    
    //tree->setWeight(&nodes[6], 1);
    tree->removeWeightInPath(1, &nodes[5]);
    
    std::cout << "*********************" << std::endl;
    
    lca61 = tree->lca(&nodes[1], &nodes[6]);
    lca73 = tree->lca(&nodes[3], &nodes[7]);
    lca57 = tree->lca(&nodes[7], &nodes[5]);
    
    dist61 = tree->getDist(&nodes[1], &nodes[6]);
    dist73 = tree->getDist(&nodes[3], &nodes[7]);
    dist57 = tree->getDist(&nodes[7], &nodes[5]);
    dist72 = tree->getDist(&nodes[7], &nodes[2]);
    
    for(int i = 1;i < 8;++i) {
        //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
        //  }
    }
    
    std::cout <<"lca(6,1): " << lca61->key << std::endl;
    std::cout <<"lca(7,3): " << lca73->key << std::endl;
    std::cout <<"lca(5,7): " << lca57->key << std::endl;
    std::cout <<"lca(7,2): " << lca72->key << std::endl;
    std::cout <<"dist(6,1): " << dist61 << std::endl;
    std::cout <<"dist(7,3): " << dist73 << std::endl;
    std::cout <<"dist(5,7): " << dist57 << std::endl;
    std::cout <<"dist(7,2): " << dist72 << std::endl;
    std::cout << "min_to_root_from_7: " << tree->getMinEdge(&nodes[7])->key << std::endl;
}

using namespace std;

#define loop(i,n) for(int i = 0;i < n;++i)

void fullTest() {
    vector <Node> nodes(10, *(new Node(0)));
    vector <SplayTree> trees;
    for(int i = 0;i < 10; ++i) {
        nodes[i].key = i;
        trees.push_back(*(new SplayTree(&nodes[i])));
    }
    LinkCutTree* tree = new LinkCutTree();
    
    tree->link(&nodes[1], &nodes[2]);
    tree->link(&nodes[3], &nodes[2]);
    tree->link(&nodes[4], &nodes[1]);
    tree->link(&nodes[5], &nodes[4]);
    tree->link(&nodes[6], &nodes[4]);
    tree->link(&nodes[7], &nodes[6]);
    //tree->linkEdge(&nodes[2], &nodes[6]);
   // tree->revert(&nodes[5]);
    std::cout << "lca(7,3): " << tree->lca(&nodes[7], &nodes[3])->key << std::endl;
    for(int i = 1;i < 8;++i) {
        //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
        //  }
    }
    
    tree->revert(&nodes[2]);
    std::cout << "lca(7,3): " << tree->lca(&nodes[5], &nodes[7])->key << std::endl;
    for(int i = 1;i < 8;++i) {
        //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
        //  }
    }
}

void solve() {
    size_t vert, edge, to, from , capacity;
    vector <DirectEdge> edgeList;
    DirectEdge curEdge;
    cin >> vert >> edge;
    for(size_t i = 0;i < edge;++i) {
        cin >> to >> from >> capacity;
        --to;
        --from;
        curEdge.start = to;
        curEdge.finish = from;
        curEdge.capacity = capacity;
        edgeList.push_back(curEdge);
    }
    
    Graph graph(vert, edgeList);
    LinkCutBlockFlowFinder linkCutBlockflowFinder;
    DinicFlowFinder dinicFlowFinder(&linkCutBlockflowFinder);
    Network network(&graph, 0, vert - 1);
    network.getMaxFlow(dinicFlowFinder);
    
    cout << dinicFlowFinder.maxFlow << endl;
    
    for(size_t i = 0;i < network.flow->size(); ++i) {
        cout << (*network.flow)[i] << endl;
    }
}

//4 7
//1 4 10
//1 2 10
//2 3 1
//3 2 10
//3 4 10
//4 3 10
//2 1 10
