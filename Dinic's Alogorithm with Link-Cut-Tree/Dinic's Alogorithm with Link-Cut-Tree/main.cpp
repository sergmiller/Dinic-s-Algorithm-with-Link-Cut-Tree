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
#include <vector>

void splayTest();
void linkCutTest();
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
    std::vector <Node> nodes(10, *(new Node(0)));
    std::vector <SplayTree> trees;
    for(int i = 0;i < 10; ++i) {
        nodes[i].key = i;
        trees.push_back(*(new SplayTree(&nodes[i])));
    }
    LinkCutTree* tree = new LinkCutTree();
    
    tree->linkEdge(&nodes[0], &nodes[1]);
    tree->linkEdge(&nodes[2], &nodes[1]);
    tree->linkEdge(&nodes[3], &nodes[2]);
    tree->linkEdge(&nodes[4], &nodes[2]);
    tree->revert(&nodes[1]);
    
    for(int i = 0;i < 5;++i) {
      //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
      //  }
    }
    
    for(int i = 0; i < 5;++i) {
      //  if(i != 0) {
            std::cout << "dist(0," << i << "): " << tree->dist(&nodes[0], &nodes[i]) << std::endl;
      //  }
    }
    
    tree->revert(&nodes[1]);
    tree->cut(&nodes[2], &nodes[1]);
    tree->revert(&nodes[4]);
    tree->link(&nodes[4], &nodes[0]);
   //

    
    std::cout << "*********************" << std::endl;
    
    for(int i = 0;i < 5;++i) {
        //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
        //  }
    }
    
    for(int i = 0; i < 5;++i) {
        //  if(i != 0) {
        std::cout << "dist(0," << i << "): " << tree->dist(&nodes[0], &nodes[i]) << std::endl;
        //  }
    }
    
}

using namespace std;

#define loop(i,n) for(int i = 0;i < n;++i)

void solve() {
    int n,m;
    scanf("%d",&n);
    vector <Node> nodes(n, *(new Node(0)));
    vector <SplayTree> trees;
    vector <size_t> ans;
    for(int i = 0;i < n; ++i) {
        //nodes[i].key = i;
        trees.push_back(*(new SplayTree(&nodes[i])));
    }
    LinkCutTree* tree = new LinkCutTree();
    
    cout <<"OK" << endl;
    
    int x,y;
    loop(i,n-1) {
        scanf("%d%d", &x,&y);
        tree->linkEdge(&nodes[x], &nodes[y]);
    }
    
    cout <<"OK" << endl;
    
    scanf("%d", &m);
    
    loop(i,m) {
        scanf("%d%d", &x, &y);
        ans.push_back(tree->dist(&nodes[x], &nodes[y]));
    }
    
    loop(i,m) {
        printf("%lld\n", (long long)ans[i]);
    }
}


