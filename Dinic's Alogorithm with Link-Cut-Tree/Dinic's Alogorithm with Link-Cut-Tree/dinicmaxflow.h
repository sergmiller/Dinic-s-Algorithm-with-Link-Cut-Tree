//
//  dinicmaxflow.h
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 14.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#ifndef dinicmaxflow_h
#define dinicmaxflow_h

#include <vector>
#include <stack>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include "linkcuttree.h"

using std::vector;
using std::pair;

class Bfs;
class Graph;
class Network;
class FlowFinder;
class DirectEdge;
class DinicFlowFinder;
class BlockFlowFinder;
class ShortPathNetwork;
class LinkCutBlockFlowFinder;

class DirectEdge
{
public:
    size_t start; //points of directed edge
    size_t finish;  //value of max flow for edge
    size_t capacity;
};

class Graph
{
public:
    vector <vector <size_t> > *outgoingList; //lists of numbers of edges that outgoing and incoming in vertex
    vector <vector <size_t> > *incomingList;
    vector <DirectEdge>& edgeList; //full info about edge
    size_t sizeVert; //total quantity of verticies and edges
    size_t sizeEdge;
    Graph(size_t vertices, vector <DirectEdge> &edges);  //get graph from list pairs of vertices
    
    ~Graph();
};

class Network
{
public:
    vector <size_t> *flow;  //current flow in each edge
    size_t source;    //source and sink in Network
    size_t sink;
    Graph *graph;
    Network(Graph *graph, size_t source, size_t sink);
    ~Network();
    size_t getMaxFlow(FlowFinder& flowFinder);   //workfunction
};


class Bfs{
public:
    vector <bool>* used;
    vector <size_t>* dist;
    Network* network;
    void init(Network* network);
    bool run();
};

class FlowFinder {
public:
    long long maxFlow;
    Network* network;
    virtual void initFlowFinder(Network* network) = 0;
    virtual void calcMaxFlow() = 0;
};

class DinicFlowFinder : public FlowFinder {
private:
    ShortPathNetwork* shortPathNetwork;
    BlockFlowFinder* blockFlowFinder;
public:
    double sumTime;
    DinicFlowFinder();
    DinicFlowFinder(BlockFlowFinder* blockFlowFinder);
    void calcMaxFlow();
    void initFlowFinder(Network* network);
    bool getShortPathNetwork();
    bool checkEdgeForShortPath(size_t edgeNumber, DirectEdge& edge);
    Bfs bfs;
};

class ShortPathNetwork : public Network{
public:
    vector <size_t>* edgeID;
    ShortPathNetwork(Graph* graph, size_t source, size_t sink, vector<size_t>* edgeID);
    ~ShortPathNetwork();
    void updateShortPathNetwork();
};


class BlockFlowFinder {
public:
    virtual void init(Network* network) = 0;
    virtual void findBlockFlow(ShortPathNetwork& shortPathNetwork) = 0;
};

class LinkCutBlockFlowFinder : public BlockFlowFinder {
private:
    LinkCutTree linkCut;
    vector <SplayTree*> trees;
    vector <Node*> nodes;
    size_t source;
    size_t sink;
public:
    void clearTree();
    void init(Network* network);
    Node* prevInPath(Node* source);
    void findBlockFlow(ShortPathNetwork& shortPathNetwork);
};

//bool getNewPath();//get new path in BFS algorithm from source to sink

#endif /* dinicmaxflow_h */
