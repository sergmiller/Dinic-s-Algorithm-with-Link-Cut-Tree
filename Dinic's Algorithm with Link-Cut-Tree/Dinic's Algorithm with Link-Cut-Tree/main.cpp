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
    //while(1) {
    freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);
        solve();
   //}
    return 0;
}

void solve() {
    //freopen("input.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);
    long long vert, edge, to, from, capacity;
    vector <DirectEdge> edgeList;// = *(new vector <DirectEdge>);
    DirectEdge curEdge;
    cin >> vert >> edge;
    edgeList.resize(edge);
    // scanf("%lld%lld",&vert,&edge);
    for(int i = 0;i < edge;++i) {
        cin >> to >> from >> capacity;
        --to;
        --from;
        curEdge.start = to;
        curEdge.finish = from;
        curEdge.capacity = capacity;
        edgeList[i] = curEdge;
    }
    
    Graph* graph = new Graph(vert, edgeList);
    LinkCutBlockFlowFinder linkCutBlockflowFinder(vert, 0, vert - 1);
    DinicFlowFinder dinicFlowFinder(&linkCutBlockflowFinder);
    Network network(graph, 0, vert - 1);
    network.getMaxFlow(dinicFlowFinder);
    cout << network.maxFlow() << endl;
    //delete graph;
    //delete graph;
    
//    for(size_t i = 0;i < network.flow.size(); ++i) {
//        cout << network.flow[i] << endl;
//    }
}
