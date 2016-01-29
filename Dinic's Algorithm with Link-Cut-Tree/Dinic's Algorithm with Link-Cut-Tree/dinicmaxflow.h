//
//  dinicmaxflow.h
//  Dinic's Algorithm with Link-Cut-Tree
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
    vector <vector <size_t> >& outgoingList; //lists of numbers of edges that outgoing and incoming in vertex
    vector <vector <size_t> >& incomingList;
    vector <DirectEdge>& edgeList; //full info about edge
    size_t sizeVert; //total quantity of verticies and edges
    size_t sizeEdge;
    Graph(size_t vertices, vector <DirectEdge>& edges);  //get graph from list pairs of vertices
    ~Graph();
};

class Network
{
    friend class FlowFinder;
    Graph *_graph;
    long long _maxFlow;
    vector <size_t> _flow;  //current flow in each edge
    public:
    const size_t source;    //source and sink in Network
    const size_t sink;
    Network(Graph *graph, size_t source, size_t sink);
    ~Network();
    size_t getMaxFlow(FlowFinder& flowFinder);   //workfunction
    long long maxFlow() { return _maxFlow; };
    vector <size_t>& flow() { return _flow; };
    Graph* graph() {return _graph;}
};

class Bfs{
public:
    Bfs();
    ~Bfs();
    vector <bool>* used;
    vector <size_t>* dist;
    
    std::queue <std::pair <size_t, size_t> > bfsQueue;
    
    Graph* graph;
    
    size_t source;
    size_t sink;
    DirectEdge curEdge;
    
    Network* network;
    void init(Network* network);
    void checkOutgoingEdges(size_t vert, size_t leveldist);
    void checkIncomingEdges(size_t vert, size_t levelDist);
    
    bool run();//return true if sink is available from source
};

class FlowFinder {
public:
    long long maxFlow;
    Network* network;
    virtual void initFlowFinder(Network* network) = 0;
    virtual void getMaxFlow() = 0;
};

class DinicFlowFinder : public FlowFinder {
private:
    ShortPathNetwork* shortPathNetwork;
    BlockFlowFinder* blockFlowFinder;
    void getMaxFlow();
    void updateFlow();
    void calcMaxFlow();
    void initFlowFinder(Network* network);
    bool getShortPathNetwork();
    bool checkEdgeForShortPath(size_t edgeNumber, DirectEdge& edge);
    Bfs bfs;
public:
    DinicFlowFinder(BlockFlowFinder* blockFlowFinder);
    ~DinicFlowFinder();
};

class ShortPathNetwork : public Network{
public:
    vector <size_t>& edgeID;
    ShortPathNetwork(Graph* graph, size_t source, size_t sink, vector<size_t>& edgeID);
    ~ShortPathNetwork();
    void updateShortPathNetwork();
};

class BlockFlowFinder {
public:
    ShortPathNetwork* shortPathNetwork;
    virtual void findBlockFlow() = 0;
};

class LinkCutBlockFlowFinder : public BlockFlowFinder {
private:
    vector <size_t> curEdgeNumber;
    vector <bool> edgeInsideTreeFlag;
    void addEdge(size_t vertex, size_t nextVert, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList);
    void removeEdge(size_t vertex, size_t prevVert, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList);
    void decreaseWeightsInPath(Node* minEdge, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList);
    void updateBlockFlow(vector <size_t>& flow, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList);
    LinkCutTree linkCut;
    size_t source;
    size_t sink;
public:
    LinkCutBlockFlowFinder(size_t sizeVert, size_t source, size_t sink);
    ~LinkCutBlockFlowFinder();
    void findBlockFlow();
};


#endif /* dinicmaxflow_h */
