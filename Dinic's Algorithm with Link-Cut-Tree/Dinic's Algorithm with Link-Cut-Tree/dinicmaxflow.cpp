
//
//  dinicmaxflow.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 14.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//

#include <cstdio>
#include "time.h"
#include "dinicmaxflow.h"
#include <fstream>
#include <iostream>

using std::vector;
using std::pair;
using std::queue;
using std::cout;

Graph::Graph(size_t vertices, vector <DirectEdge>& edges):edgeList(edges)
{
    sizeEdge = edges.size();
    sizeVert = vertices;
    DirectEdge curEdge;
    outgoingList = new vector <vector <size_t> >(sizeVert);
    incomingList = new vector <vector <size_t> >(sizeVert);
    for(size_t i = 0;i < edges.size(); ++i)
    {
        curEdge = edgeList[i];
        (*outgoingList)[curEdge.start].push_back(i);
        (*incomingList)[curEdge.finish].push_back(i);
    }
}

Graph::~Graph()
{
    delete incomingList;
    delete outgoingList;
    delete &edgeList;
}


Network::Network(Graph *graph, size_t source, size_t sink)
{
    flow = new vector <size_t>(graph->sizeEdge, 0);
    this->graph = graph;
    this->source = source;
    this->sink = sink;
}

Network::~Network()
{
    delete graph;
    delete flow;
}

size_t Network::getMaxFlow(FlowFinder& flowFinder) {
    flowFinder.initFlowFinder(this);
    flowFinder.calcMaxFlow();
    return flowFinder.maxFlow;
}

DinicFlowFinder::DinicFlowFinder() {
    DinicFlowFinder(linkCutBlockFlowFinder);
}

DinicFlowFinder::DinicFlowFinder(BlockFlowFinder* blockFlowFinder) {
    this->blockFlowFinder = blockFlowFinder;
    shortPathNetwork = nullptr;
}

void DinicFlowFinder::initFlowFinder(Network* network) {
    this->network = network;
    maxFlow = 0;
}

void DinicFlowFinder::calcMaxFlow() {
    blockFlowFinder->init(network);
    while(getShortPathNetwork()) {
        double clock7 = clock();
        blockFlowFinder->findBlockFlow(*shortPathNetwork);
        //std::cout << "aft: " << (clock() - clock7)/CLOCKS_PER_SEC << std::endl;
        sumTime += (clock() - clock7)/CLOCKS_PER_SEC;
        DirectEdge originalEdge;
        DirectEdge imageEdge;
        vector <DirectEdge>& originEdgeList = network->graph->edgeList;
        vector <DirectEdge>& shortPathEdgeList = shortPathNetwork->graph->edgeList;
        
        for(size_t i = 0;i < shortPathNetwork->edgeID->size(); ++i) {
            originalEdge = originEdgeList[(*shortPathNetwork->edgeID)[i]];
            imageEdge = shortPathEdgeList[i];
            if(originalEdge.start == imageEdge.start) {
                (*network->flow)[(*shortPathNetwork->edgeID)[i]] += (*shortPathNetwork->flow)[i];
            } else {
                (*network->flow)[(*shortPathNetwork->edgeID)[i]] -= (*shortPathNetwork->flow)[i];
            }
        }
        
        delete shortPathNetwork;
    }
    
    vector <size_t>& outgoingListFromSource = (*network->graph->outgoingList)[network->source];
    maxFlow = 0;
    
    for(size_t i = 0;i < outgoingListFromSource.size(); ++i) {
            maxFlow += (*network->flow)[outgoingListFromSource[i]];
    }
    
    return;
}

bool DinicFlowFinder::getShortPathNetwork() {
    vector <DirectEdge>* shortPathEdges = new vector <DirectEdge>;
    vector <size_t>* edgeID = new vector <size_t>;
    //double clock7 = clock();
    bfs.init(network);
    
    if(!bfs.run()) {
        return false;
    }
    
    //std::cout << "bfs: " << (clock() - clock7)/CLOCKS_PER_SEC << std::endl;
    //clock7 = clock();
    DirectEdge curEdge;
    vector <DirectEdge>& edgeList = network->graph->edgeList;
    for(size_t i = 0;i < edgeList.size(); ++i) {
        curEdge = edgeList[i];
        if(checkEdgeForShortPath(i, curEdge)) {
            edgeID->push_back(i);
            shortPathEdges->push_back(curEdge);
        }
    }
    
    shortPathNetwork = new ShortPathNetwork(new Graph(network->graph->sizeVert, *shortPathEdges),
                                            network->source, network->sink, edgeID);
    //std::cout << "init: " << (clock() - clock7)/CLOCKS_PER_SEC << std::endl;
    return true;
}

bool DinicFlowFinder::checkEdgeForShortPath(size_t edgeNumber, DirectEdge& edge) {
    if((*bfs.dist)[edge.start] + 1 == (*bfs.dist)[edge.finish] && (*network->flow)[edgeNumber] < edge.capacity && edge.start != network->sink) {
        edge.capacity -= (*network->flow)[edgeNumber];
        return true;
    }
    
    if((*bfs.dist)[edge.finish] + 1 == (*bfs.dist)[edge.start] &&  (*network->flow)[edgeNumber] > 0 && edge.finish != network->sink) {
        std::swap(edge.start, edge.finish);
        edge.capacity = (*network->flow)[edgeNumber];
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
    size_t sizeVert = network->graph->sizeVert;
    if(!used) {
        used = new vector <bool> (sizeVert);
        dist = new vector <size_t> (sizeVert);
        this->network = network;
    }
    
    vector <bool>::iterator usedIt = used->begin();
    vector <size_t>::iterator distIt = dist->begin();
    for(;usedIt != used->end(); ++usedIt, ++distIt) {
        *usedIt = false;
        *distIt = INF;
    }
}

bool Bfs::run() {
    queue <pair <size_t, size_t> > bfsQueue;
    Graph* graph = network->graph;
    size_t source = network->source;
    size_t sink = network->sink;
    size_t vert;
    size_t numEdge;
    size_t levelDist;
    DirectEdge curEdge;
    
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
        
        for(size_t i = 0;i < (*(graph->outgoingList))[vert].size();++i) {
            numEdge = (*graph->outgoingList)[vert][i];
            curEdge = (graph->edgeList)[numEdge];
            if(!(*used)[curEdge.finish] && (*network->flow)[numEdge] < curEdge.capacity) {
                (*used)[curEdge.finish] = true;
                (*dist)[curEdge.finish] = levelDist;
                bfsQueue.push(std::make_pair(curEdge.finish, levelDist));
            }
        }
        
        for(size_t i = 0;i < (*graph->incomingList)[vert].size();++i) {
            numEdge = (*graph->incomingList)[vert][i];
            curEdge = (graph->edgeList)[numEdge];
            if(!(*used)[curEdge.start] && (*network->flow)[numEdge] > 0) {
                (*used)[curEdge.start] = true;
                (*dist)[curEdge.start] = levelDist;
                bfsQueue.push(std::make_pair(curEdge.start, levelDist));
            }
        }
    }
    
    return (*used)[sink];
}

ShortPathNetwork::ShortPathNetwork(Graph* graph, size_t source, size_t sink, vector <size_t>* edgeID):
Network(graph, source, sink){
    this->edgeID = edgeID;
}

ShortPathNetwork::~ShortPathNetwork() {
    delete edgeID;
}

void LinkCutBlockFlowFinder::init(Network* network) {
    
    for(size_t i = 0;i < network->graph->sizeVert; ++i) {
        nodes.push_back(new Node(i, 0));
        trees.push_back(new SplayTree(nodes[i]));
    }
    source = network->source;
    sink = network->sink;
}

void LinkCutBlockFlowFinder::clearTree() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        if(trees[i] != nullptr) {
            delete nodes[i];
        }
        nodes[i] = new Node(i, 0);
        trees[i] = new SplayTree(nodes[i]);
    }
}
    
LinkCutBlockFlowFinder::~LinkCutBlockFlowFinder() {
    for(size_t i = 0;i  < trees.size(); ++i) {
        delete trees[i];
    }
}

void LinkCutBlockFlowFinder::findBlockFlow(ShortPathNetwork& shortPathNetwork) {
    vector <size_t> curEdgeNumber(shortPathNetwork.graph->sizeVert, false);
    vector <bool> badVert(shortPathNetwork.graph->sizeVert,0);
    vector <vector <size_t> >* outEdges = shortPathNetwork.graph->outgoingList;
    vector <DirectEdge>& edgeList = shortPathNetwork.graph->edgeList;
    //vector <size_t> startCapacity(edgeList.size());
    vector <bool> edgeInsideFlag(shortPathNetwork.graph->sizeVert, false);
    size_t vertex;
    size_t nextVert;
    size_t prevVert;
    
    for(size_t i = 0;i < edgeList.size(); ++i) {
        (*shortPathNetwork.flow)[i] = edgeList[i].capacity;
    }
    
    double clock7 = clock();
    
    while(1) {
        if((vertex = linkCut.findRoot(nodes[source])->key) != sink) {
            if(curEdgeNumber[vertex] != (*outEdges)[vertex].size()) {
                nextVert = edgeList[(*outEdges)[vertex][curEdgeNumber[vertex]]].finish;
                
                linkCut.setWeight(nodes[vertex], edgeList[(*outEdges)[vertex][curEdgeNumber[vertex]]].capacity);
                linkCut.link(nodes[vertex], nodes[nextVert]);
                linkCut.setWeight(linkCut.findRoot(nodes[source]), INF);
                edgeInsideFlag[vertex] = true;
                continue;
            } else {
                if(vertex == source) {
                    edgeInsideFlag[source] = false;
                    break;
                } else {
                    Node* prevEdge = prevInPath(nodes[source]);
                    prevVert = prevEdge->key;
                    linkCut.cut(prevEdge, nodes[vertex]);
                    edgeList[(*outEdges)[prevVert][curEdgeNumber[prevVert]]].capacity = linkCut.getEdgeWeight(prevEdge);
                    linkCut.setWeight(prevEdge, INF);
                    ++curEdgeNumber[prevVert];
                    edgeInsideFlag[prevVert] = false;
                }
            }
        } else {
            Node* minEdge = linkCut.getMinEdge(nodes[source]);
            size_t minVert;
            linkCut.removeWeightInPath(minEdge->edgeWeight, nodes[source]);
            while(linkCut.getEdgeWeight(minEdge = linkCut.getMinEdge(nodes[source])) == 0) {
                minVert = minEdge->key;
                edgeList[(*outEdges)[minVert][curEdgeNumber[minVert]]].capacity = 0;
                linkCut.cut(minEdge, nodes[edgeList[(*outEdges)[minVert][curEdgeNumber[minVert]]].finish]);
                linkCut.setWeight(minEdge, INF);
                ++curEdgeNumber[minVert];
                edgeInsideFlag[minVert] = false;
                if(minVert == source) {
                    break;
                }
            }
        }
    }
    
    std::cout << "block: " << (clock() - clock7)/CLOCKS_PER_SEC << std::endl;
    
    DirectEdge curEdge;
    for(size_t i = 0;i < shortPathNetwork.flow->size(); ++i) {
        curEdge = edgeList[i];
        if(curEdgeNumber[curEdge.start] != (*outEdges)[curEdge.start].size()
           && (*outEdges)[curEdge.start][curEdgeNumber[curEdge.start]] == i && edgeInsideFlag[curEdge.start]) {
            (*shortPathNetwork.flow)[i] -= linkCut.getEdgeWeight(nodes[edgeList[i].start]);
        } else {
            (*shortPathNetwork.flow)[i] -= edgeList[i].capacity;
        }
    }
    clearTree();
}
    

Node* LinkCutBlockFlowFinder::prevInPath(Node* source) {
    linkCut.expose(linkCut.findRoot(source));
    return linkCut.leftest(linkCut.supportRoot(source));
}
