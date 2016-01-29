
//
//  dinicmaxflow.cpp
//  Dinic's Algorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 14.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include <cstdio>
#include "time.h"
#include "dinicmaxflow.h"
#include <fstream>
#include <iostream>

//using std::vector;
//using std::pair;
//using std::queue;
//using std::cout;

Graph::Graph(size_t vertices, vector <DirectEdge>& edges):sizeVert(vertices), sizeEdge(edges.size()), edgeList( *(new vector <DirectEdge>)), incomingList(*(new vector <vector <size_t> >(vertices))), outgoingList(*(new vector <vector <size_t> >(vertices)))
{
    edgeList = edges;
    DirectEdge curEdge;
    for(size_t i = 0;i < edges.size(); ++i)
    {
        curEdge = edgeList[i];
        outgoingList[curEdge.start].push_back(i);
        incomingList[curEdge.finish].push_back(i);
    }
}

Graph::~Graph()
{
    delete &incomingList;
    delete &outgoingList;
    delete &edgeList;
}


Network::Network(Graph *graph, size_t source, size_t sink): _graph(graph), sink(sink), source(source), _maxFlow(0)
{
    _flow.resize(graph->sizeEdge, 0);
}

Network::~Network()
{
    delete _graph;
    //delete flow;
}

size_t Network::getMaxFlow(FlowFinder& flowFinder) {
    flowFinder.initFlowFinder(this);
    flowFinder.getMaxFlow();
    return _maxFlow = flowFinder.maxFlow;
}

DinicFlowFinder::DinicFlowFinder(BlockFlowFinder* blockFlowFinder): blockFlowFinder(blockFlowFinder), shortPathNetwork(nullptr)
{
}

DinicFlowFinder::~DinicFlowFinder() {
    blockFlowFinder->~BlockFlowFinder();
}

void DinicFlowFinder::initFlowFinder(Network* network) {
    this->network = network;
    maxFlow = 0;
}

void DinicFlowFinder::calcMaxFlow() {
    vector <DirectEdge>& edgeList = network->graph()->edgeList;
    vector <size_t>& flow = network->flow();
    size_t source = network->source;
    
    for(size_t i = 0;i < edgeList.size(); ++i) {
        if(edgeList[i].start == source) {
            maxFlow += flow[i];
        }
        if(edgeList[i].finish == source) {
            maxFlow -= flow[i];
        }
    }
}

void DinicFlowFinder::updateFlow() {
    DirectEdge originalEdge;
    DirectEdge imageEdge;
    vector <DirectEdge>& originEdgeList = network->graph()->edgeList;
    vector <DirectEdge>& shortPathEdgeList = shortPathNetwork->graph()->edgeList;
    vector <size_t>& networkFlow = network->flow();
    vector <size_t>& shortPathNetworkFlow = shortPathNetwork->flow();
    
    for(size_t i = 0;i < shortPathNetwork->edgeID.size(); ++i) {
        originalEdge = originEdgeList[shortPathNetwork->edgeID[i]];
        imageEdge = shortPathEdgeList[i];
        if(originalEdge.start == imageEdge.start) {
            networkFlow[shortPathNetwork->edgeID[i]] += shortPathNetworkFlow[i];
        } else {
            networkFlow[shortPathNetwork->edgeID[i]] -= shortPathNetworkFlow[i];
        }
    }
}

void DinicFlowFinder::getMaxFlow() {
    while(getShortPathNetwork()) {
        blockFlowFinder->shortPathNetwork = shortPathNetwork;
        blockFlowFinder->findBlockFlow();
        
        updateFlow();
        delete shortPathNetwork;
    }
    
    calcMaxFlow();
    
    return;
}

bool DinicFlowFinder::getShortPathNetwork() {
    vector <DirectEdge> shortPathEdges;// = new vector <DirectEdge>;
    vector <size_t>* edgeID = new vector <size_t>;
    
    bfs.init(network);
    
    if(!bfs.run()) {
        return false;
    }
    
    DirectEdge curEdge;
    vector <DirectEdge>& edgeList = network->graph()->edgeList;
    for(size_t i = 0;i < edgeList.size(); ++i) {
        curEdge = edgeList[i];
        if(checkEdgeForShortPath(i, curEdge)) {
            edgeID->push_back(i);
            shortPathEdges.push_back(curEdge);
        }
    }
    
    shortPathNetwork = new ShortPathNetwork(new Graph(network->graph()->sizeVert, shortPathEdges),
                                            network->source, network->sink, *edgeID);
    return true;
}

bool DinicFlowFinder::checkEdgeForShortPath(size_t edgeNumber, DirectEdge& edge) {
    if((*bfs.dist)[edge.start] + 1 == (*bfs.dist)[edge.finish] && network->flow()[edgeNumber] < edge.capacity && edge.start != network->sink) {
        edge.capacity -= network->flow()[edgeNumber];
        return true;
    }
    
    if((*bfs.dist)[edge.finish] + 1 == (*bfs.dist)[edge.start] &&  network->flow()[edgeNumber] > 0 && edge.finish != network->sink) {
        std::swap(edge.start, edge.finish);
        edge.capacity = network->flow()[edgeNumber];
        return true;
    }
    
    return false;
}

Bfs::Bfs() {
    used = nullptr;
    dist = nullptr;
}

Bfs::~Bfs() {
    delete used;
    delete dist;
}

void Bfs::init(Network* network) {
    size_t sizeVert = network->graph()->sizeVert;
    if(!used) {
        used = new vector <bool> (sizeVert);
        dist = new vector <size_t> (sizeVert);
    }
    
    vector <bool>::iterator usedIt = used->begin();
    vector <size_t>::iterator distIt = dist->begin();
    for(;usedIt != used->end(); ++usedIt, ++distIt) {
        *usedIt = false;
        *distIt = INF;
    }
    this->network = network;
}

void Bfs::checkOutgoingEdges(size_t vert, size_t levelDist) {
    size_t numEdge;
    vector <size_t>& flow = network->flow();
    for(size_t i = 0;i < graph->outgoingList[vert].size();++i) {
        numEdge = graph->outgoingList[vert][i];
        curEdge = (graph->edgeList)[numEdge];
        if(!(*used)[curEdge.finish] && flow[numEdge] < curEdge.capacity) {
            (*used)[curEdge.finish] = true;
            (*dist)[curEdge.finish] = levelDist;
            bfsQueue.push(std::make_pair(curEdge.finish, levelDist));
        }
    }
}

void Bfs::checkIncomingEdges(size_t vert, size_t levelDist) {
    size_t numEdge;
    vector <size_t>& flow = network->flow();
    for(size_t i = 0;i < graph->incomingList[vert].size();++i) {
        numEdge = graph->incomingList[vert][i];
        curEdge = graph->edgeList[numEdge];
        if(!(*used)[curEdge.start] && flow[numEdge] > 0) {
            (*used)[curEdge.start] = true;
            (*dist)[curEdge.start] = levelDist;
            bfsQueue.push(std::make_pair(curEdge.start, levelDist));
        }
    }
}

bool Bfs::run() {
    size_t vert;
    size_t levelDist;
    
    graph = network->graph();
    source = network->source;
    sink = network->sink;
    
    (*dist)[source] = 0;
    (*used)[source] = true;
    bfsQueue.push(std::make_pair(source, 0));
    
    while(!bfsQueue.empty()) {
        vert = bfsQueue.front().first;
        levelDist = bfsQueue.front().second;
        bfsQueue.pop();
        
        if(vert == sink) {
            continue;
        }
        
        ++levelDist;
        
        checkOutgoingEdges(vert, levelDist);
        checkIncomingEdges(vert, levelDist);
    }
    
    return (*used)[sink];
}

ShortPathNetwork::ShortPathNetwork(Graph* graph, size_t source, size_t sink, vector <size_t>& edgeID):
Network(graph, source, sink), edgeID(edgeID){
}

ShortPathNetwork::~ShortPathNetwork() {
    delete &edgeID;
    //delete graph;
}

LinkCutBlockFlowFinder::LinkCutBlockFlowFinder(size_t sizeVert, size_t source, size_t sink): linkCut(LinkCutTree(sizeVert)), source(source), sink(sink) {
};

LinkCutBlockFlowFinder::~LinkCutBlockFlowFinder() {
    //delete &linkCut;
}

void LinkCutBlockFlowFinder::addEdge(size_t vertex, size_t nextVert, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList) {
    linkCut.setWeight(vertex, edgeList[outEdges[vertex][curEdgeNumber[vertex]]].capacity);
    linkCut.link(vertex, nextVert);
    linkCut.findRoot(source);
    linkCut.setWeight(Node::getKey(linkCut.findRoot(source)), INF);
    edgeInsideTreeFlag[vertex] = true;
}

void LinkCutBlockFlowFinder::removeEdge(size_t vertex, size_t prevVert, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList) {
    linkCut.cut(prevVert, vertex);
    edgeList[outEdges[prevVert][curEdgeNumber[prevVert]]].capacity = linkCut.getEdgeWeight(prevVert);
    linkCut.setWeight(prevVert, INF);
    ++curEdgeNumber[prevVert];
    edgeInsideTreeFlag[prevVert] = false;
}

void LinkCutBlockFlowFinder::decreaseWeightsInPath(Node* minEdge, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList) {
    size_t minVert;
    linkCut.removeWeightInPath(Node::getWeight(minEdge), source);
    while(linkCut.getEdgeWeight(Node::getKey(minEdge = linkCut.getMinEdge(source))) == 0) {
        minVert = Node::getKey(minEdge);
        edgeList[outEdges[minVert][curEdgeNumber[minVert]]].capacity = 0;
        linkCut.cut(minVert, edgeList[outEdges[minVert][curEdgeNumber[minVert]]].finish);
        linkCut.setWeight(minVert, INF);
        ++curEdgeNumber[minVert];
        edgeInsideTreeFlag[minVert] = false;
        if(minVert == source) {
            break;
        }
    }
}

void LinkCutBlockFlowFinder::updateBlockFlow(vector <size_t>& flow, vector <vector <size_t> >& outEdges, vector <DirectEdge>& edgeList) {
    DirectEdge curEdge;
    for(size_t i = 0;i < flow.size(); ++i) {
        curEdge = edgeList[i];
        if(curEdgeNumber[curEdge.start] != outEdges[curEdge.start].size()
           && outEdges[curEdge.start][curEdgeNumber[curEdge.start]] == i && edgeInsideTreeFlag[curEdge.start]) {
            (flow)[i] -= linkCut.getEdgeWeight(edgeList[i].start);
        } else {
            (flow)[i] -= edgeList[i].capacity;
        }
    }
    
}

void LinkCutBlockFlowFinder::findBlockFlow() {
    curEdgeNumber.clear();
    edgeInsideTreeFlag.clear();
    curEdgeNumber.resize(shortPathNetwork->graph()->sizeVert, false);
    edgeInsideTreeFlag.resize(shortPathNetwork->graph()->sizeVert, false);
    vector <vector <size_t> >& outEdges = shortPathNetwork->graph()->outgoingList;
    vector <DirectEdge>& edgeList = shortPathNetwork->graph()->edgeList;
    vector <size_t>& flow = shortPathNetwork->flow();
    
    size_t vertex;
    size_t nextVert;
    size_t prevVert;
    
    for(size_t i = 0;i < edgeList.size(); ++i) {
        flow[i] = edgeList[i].capacity;
    }
    
    linkCut.clearTrees();
    
    while(true) {
        if((vertex = Node::getKey(linkCut.findRoot(source))) != sink) {
            if(curEdgeNumber[vertex] != outEdges[vertex].size()) {
                nextVert = edgeList[outEdges[vertex][curEdgeNumber[vertex]]].finish;
                
                addEdge(vertex, nextVert, outEdges, edgeList);//Step 1
            } else {
                if(vertex == source) {
                    edgeInsideTreeFlag[source] = false;//Step 2
                    break;
                } else {
                    prevVert = Node::getKey(linkCut.prevInPath(source));
                    
                    removeEdge(vertex, prevVert, outEdges, edgeList);//Step 3
                }
            }
        } else {
            Node* minEdge = linkCut.getMinEdge(source);
            
            decreaseWeightsInPath(minEdge, outEdges, edgeList);//Step 4
        }
    }
    
    updateBlockFlow(flow, outEdges, edgeList);
    
}

