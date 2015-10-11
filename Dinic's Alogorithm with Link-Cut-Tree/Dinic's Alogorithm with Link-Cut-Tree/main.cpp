//
//  main.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 10.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include <iostream>
#include "splaytree.h"

int main() {
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
    return 0;
}
/*
 10
 1 2 3 4 5 6 7 8 9 10
 3 6
 */




