//
//  main.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 10.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include "splaytree.h"
#include "linkcuttree.h"
#include "dinicmaxflow.h"

//#define SIZE_T_MAX 1e15
using namespace std;

//**********************************************************************************************

void solve();
void linkCutTest();

int main() {
    ios_base::sync_with_stdio(false);
    while(1) {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
        solve();
   }
    //linkCutTest();
    cout << "aaa" << endl;
    return 0;
}

void solve() {
    long long vert, edge, to, from, capacity;
    vector <DirectEdge>* edgeList = new vector <DirectEdge>;
    DirectEdge curEdge;
    cin >> vert >> edge;
    // scanf("%lld%lld",&vert,&edge);
    //cout << "OK" << endl;
    for(int i = 0;i < edge;++i) {
        cin >> to >> from >> capacity;
        //     scanf("%lld%lld%lld",&to,&from,&capacity);
        --to;
        --from;
        curEdge.start = to;
        curEdge.finish = from;
        curEdge.capacity = capacity;
        edgeList->push_back(curEdge);
    }
    
    //cout <<"OK" << endl;
    
    //Graph graph(vert, edgeList);
    LinkCutBlockFlowFinder linkCutBlockflowFinder;
    DinicFlowFinder dinicFlowFinder(&linkCutBlockflowFinder);
    Network network(new Graph(vert,*edgeList), 0, vert - 1);
    double clock7 = clock();
    network.getMaxFlow(dinicFlowFinder);
    std::cout << "end: " << (clock() - clock7)/CLOCKS_PER_SEC << std::endl;
    std::cout << "sum: " << dinicFlowFinder.sumTime << std::endl;
    
    //cout << "OK" << endl;
    
    cout << dinicFlowFinder.maxFlow << endl;
    
    for(size_t i = 0;i < network.flow->size(); ++i) {
        cout << (*network.flow)[i] << endl;
    }
}
