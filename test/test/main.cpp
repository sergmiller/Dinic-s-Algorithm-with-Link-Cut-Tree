//
//  main.cpp
//  Dinic's Alogorithm with Link-Cut-Tree
//
//  Created by Сергей Миллер on 10.10.15.
//  Copyright © 2015 Сергей Миллер. All rights reserved.
//
//!

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include <cassert>
#define nullptr NULL


//#define SIZE_T_MAX 1e15
using namespace std;

//**********************************************************************************************

class Node;
class SplayTree;

const size_t INF = 1e16;

class Node{
public:
    static void removeWeight(size_t value, Node* vertex);
    static void updateNodeParams(Node* vertex);
    static void recursiveDelete(Node* vertex);
    static void push(Node* vertex);
    
    static size_t getSize(Node* vertex);
    static size_t getMin(Node* vertex);
    static size_t getSum(Node* vertex);
    
    Node(size_t key, size_t weight = 0);
    
    size_t key;
    size_t edgeWeight;
    size_t sizeOfSubtree;
    size_t subtreeMinWeight;
    size_t removedWeightValue;
    
    Node* leftChild;
    Node* rightChild;
    Node* parent;
    Node* link;
    
    SplayTree* treePtr;
};

class SplayTree
{
    friend class LinkCutTree;
private:
    Node* _find(size_t position, Node* vertex);
    void _keepParent(Node* vertex);
    void _setParent(Node* parent, Node* vertex);
    void _rotate(Node* parent, Node* vertex);
    void _merge(SplayTree* addedTree); //added tree is right merged tree
    SplayTree* _split(size_t position); //returned tree is tight splited tree
    
    Node* _root;
public:
    SplayTree(Node* root);
    ~SplayTree();
    
    Node* find(size_t position);
    
    static SplayTree* merge(SplayTree* leftTree, SplayTree* rightTree);
    static std::pair<SplayTree*, SplayTree*> split(SplayTree* tree,size_t position);
    
    void splay(Node* vertex);
    
    Node* getRoot() { return _root; };
};


//**********************************************************************************************



Node::Node(size_t key, size_t edgeWeight):key(key), sizeOfSubtree(1), leftChild(nullptr), rightChild(nullptr), parent(nullptr), link(nullptr), subtreeMinWeight(edgeWeight), removedWeightValue(0), treePtr(nullptr), edgeWeight(edgeWeight) {
}

void Node::recursiveDelete(Node* vertex) {
    if(vertex) {
        Node::recursiveDelete(vertex->leftChild);
        Node::recursiveDelete(vertex->rightChild);
        delete vertex;
    }
}

void Node::removeWeight(size_t value, Node* vertex) {
    if(vertex) {
        vertex->removedWeightValue += value;
    }
}

void Node::push(Node* vertex) {
    if(vertex) {
        vertex->edgeWeight -= vertex->removedWeightValue;
        Node::removeWeight(vertex->removedWeightValue, vertex->leftChild);
        Node::removeWeight(vertex->removedWeightValue, vertex->rightChild);
        vertex->removedWeightValue = 0;
        Node::updateNodeParams(vertex);
    }
}

SplayTree::SplayTree(Node* root) {
    _root = root;
    if(root) {
        root->treePtr = this;
    }
}

SplayTree::~SplayTree() {
    Node::recursiveDelete(_root);
}

void SplayTree::_setParent(Node* vertex, Node* parent) {
    if(vertex) {
        vertex->parent = parent;
    }
}

size_t Node::getSize(Node* vertex) {
    if(vertex) {
        return vertex->sizeOfSubtree;
    }
    return 0;
}

size_t Node::getMin(Node* vertex) {
    if(vertex) {
        return vertex->subtreeMinWeight - vertex->removedWeightValue;
    }
    return INF;
}

void Node::updateNodeParams(Node* vertex) {
    if(vertex) {
        vertex->sizeOfSubtree = getSize(vertex->leftChild) + getSize(vertex->rightChild) + 1;
        vertex->subtreeMinWeight = min(min(getMin(vertex->leftChild), getMin(vertex->rightChild)), vertex->edgeWeight);
    }
}

void SplayTree::_keepParent(Node* vertex) {
    _setParent(vertex->leftChild, vertex);
    _setParent(vertex->rightChild, vertex);
    Node::updateNodeParams(vertex);
}

void SplayTree::_rotate(Node* parent, Node* vertex) {
    Node* grandParent = parent->parent;
    
    Node::push(grandParent);
    Node::push(parent);
    Node::push(vertex);
    
    if(grandParent) {
        if(grandParent->leftChild == parent) {
            grandParent->leftChild = vertex;
        } else {
            grandParent->rightChild = vertex;
        }
    }
    
    if(parent->leftChild == vertex) {
        parent->leftChild = vertex->rightChild;
        vertex->rightChild = parent;
    } else {
        parent->rightChild = vertex->leftChild;
        vertex->leftChild = parent;
    }
    
    _keepParent(parent);
    _keepParent(vertex);
    
    _setParent (vertex, grandParent);
}

void SplayTree::splay(Node* vertex){
    while(true) {
        if(!vertex->parent) {
            _root = vertex;
            _root->treePtr = this;
            return;
        }
        
        Node* parent = vertex->parent;
        Node* grandParent = parent->parent;
        
        if(!grandParent) {
            _rotate(parent, vertex);
            _root = vertex;
            _root->treePtr = this;
            return;
        }
        
        bool zigZigFlag = ((grandParent->leftChild == parent) == (parent->leftChild == vertex));
        
        if(zigZigFlag) {
            _rotate(grandParent, parent);
            _rotate(parent, vertex);
        } else {
            _rotate(parent, vertex);
            _rotate(grandParent, vertex);
        }
    }
}

Node* SplayTree::find(size_t position) {
    size_t treeSize = Node::getSize(_root);
    
    if(position >= treeSize) {
        return NULL;
        // throw std::out_of_range("out of range in SplayTree::find\n");
    }
    
    return _find(position, _root);
}

Node* SplayTree::_find(size_t position, Node* vertex) {
    Node::push(vertex);
    
    size_t indexLeft = Node::getSize(vertex->leftChild);
    
    if(position == indexLeft) {
        splay(vertex);
        return vertex;
    }
    
    if(position < indexLeft) {
        return _find(position, vertex->leftChild);
    }
    
    return _find(position - indexLeft - 1, vertex->rightChild);
}

std::pair<SplayTree*, SplayTree*> SplayTree::split(SplayTree* tree,size_t position) {
    SplayTree* leftTree = nullptr;
    SplayTree* rightTree = nullptr;
    if(tree) {
        rightTree = tree->_split(position);
        leftTree = tree;
    }
    return std::make_pair(leftTree, rightTree);
}


SplayTree* SplayTree::_split(size_t position){
    size_t treeSize = (_root ? _root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        return NULL;
        //    throw std::out_of_range("out of range in SplayTree::split\n");
    }
    
    if(position == treeSize) {
        return new SplayTree(nullptr);
    }
    
    Node* newRoot = _find(position, _root);
    
    SplayTree* rightTree = new SplayTree(newRoot);
    
    _root = newRoot->leftChild;
    newRoot->leftChild = nullptr;
    _setParent(_root, nullptr);
    
    if(rightTree->_root) {
        rightTree->_root->treePtr = rightTree;
    }
    
    Node::push(rightTree->_root);
    Node::push(_root);
    
    return rightTree;
}

SplayTree* SplayTree::merge(SplayTree* leftTree, SplayTree* rightTree) {
    if(!leftTree) {
        return rightTree;
    }
    
    leftTree->_merge(rightTree);
    
    return leftTree;
}

void SplayTree::_merge(SplayTree* addedTree) {
    if(!addedTree->_root)
    {
        delete addedTree;
        addedTree = nullptr;
    }
    
    
    if(!_root) {
        _root = addedTree->_root;
        addedTree->_root = nullptr;
        delete addedTree;
        addedTree = nullptr;
        return;
    }
    
    find(_root->sizeOfSubtree - 1);
    addedTree->find(0);
    
    Node::push(_root);
    
    _root->rightChild = addedTree->_root;
    addedTree->_root = nullptr;
    delete addedTree;
    addedTree = nullptr;
    _keepParent(_root);
}

//**********************************************************************************************
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

//**********************************************************************************************
LinkCutTree::LinkCutTree(size_t sizeVert){
    nodes.resize(sizeVert, Node(0));
    for(size_t i = 0;i < nodes.size(); ++i) {
        new SplayTree(&nodes[i]);
        nodes[i].key = i;
    }
}

LinkCutTree::~LinkCutTree() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        if(nodes[i].parent == nullptr) {
            SplayTree* buff = nodes[i].treePtr;
            //assert(buff->_root = &nodes[i]);
            buff->_root = nullptr;
            delete buff;
        }
    }
}

void LinkCutTree::clearTrees() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        if(nodes[i].parent != nullptr) {
            nodes[i] = Node(i,0);
            nodes[i].treePtr = new SplayTree(&nodes[i]);
        } else {
            SplayTree* buff = nodes[i].treePtr;
            nodes[i] =  Node(i,0);
            nodes[i].treePtr = buff;
            //buff->_root = &nodes[i];
        }
    }
}

void LinkCutTree::link(size_t indRoot, size_t indVert) {
    Node* vertex = &nodes[indVert];
    Node* treeRoot = &nodes[indRoot];
    treeRoot->link = vertex;
    _expose(treeRoot);
}

void LinkCutTree::cut(size_t indVert, size_t indParent) {
    Node* vertex = &nodes[indVert];
    Node* parent = &nodes[indParent];
    _expose(parent);
    vertex->link = nullptr;
}

Node* LinkCutTree::findRoot(size_t ind) {
    Node* vertex = &nodes[ind];
    if(vertex != lastExposed) {
        _expose(vertex);
    }
    return _leftest(_liftUpToRoot(vertex));
}

Node* LinkCutTree::_cleanUp(Node* vertex) {
    Node* root;
    
    if(vertex->parent) {
        root = _cleanUp(vertex->parent);
    } else {
        root = vertex;
    }
    
    Node::push(vertex);
    
    return root;
}

inline Node* LinkCutTree::_liftUpToRoot(Node* vertex) {
    if(!vertex) {
        return nullptr;
    }
    
    if(!vertex->parent) {
        return vertex;
    }
    
    Node* root = _cleanUp(vertex);
    root->treePtr->splay(vertex);
    return vertex;
}

Node* LinkCutTree::_leftest(Node* root) {
    return root->treePtr->find(0);
}

Node* LinkCutTree::_cutout(Node* vertex) {
    _liftUpToRoot(vertex);
    std::pair<SplayTree*, SplayTree*> splitedTrees = SplayTree::split(vertex->treePtr, Node::getSize(vertex->leftChild) + 1);
    SplayTree* right = splitedTrees.second;
    if(right->getRoot()) {
        right->find(0)->link = vertex;
    } else {
        delete right;
    }
    return vertex;
}

Node* LinkCutTree::_expose(Node* vertex) {
    lastExposed = vertex;
    Node* next;
    vertex = _leftest(_liftUpToRoot(_cutout(vertex)));
    while(vertex->link != nullptr) {
        next = _cutout(vertex->link);
        vertex->link = nullptr;
        SplayTree::merge(_liftUpToRoot(next)->treePtr, _liftUpToRoot(vertex)->treePtr);
        vertex = _leftest(_liftUpToRoot(vertex));
    }
    return vertex;
}

Node* LinkCutTree::getMinEdge(size_t ind) {
    Node* vertex = &nodes[ind];
    _liftUpToRoot(vertex);
    size_t minValue = Node::getMin(vertex);
    return _findLeftestMin(minValue, vertex);
}

Node* LinkCutTree::_findLeftestMin(size_t minValue, Node* vertex) {
    Node::push(vertex);
    
    if(Node::getMin(vertex->leftChild) == minValue) {
        return _findLeftestMin(minValue, vertex->leftChild);
    }
    
    if(vertex->edgeWeight == minValue) {
        return vertex;
    }
    
    return _findLeftestMin(minValue, vertex->rightChild);
}

void LinkCutTree::setWeight(size_t indVert, size_t weight) {
    Node* vertex = &nodes[indVert];
    _liftUpToRoot(vertex);
    vertex->edgeWeight = weight;
    Node::updateNodeParams(vertex);
}

void LinkCutTree::removeWeightInPath(size_t added, size_t indVert) {
    Node::removeWeight(added, &nodes[indVert]);
}

size_t LinkCutTree::getEdgeWeight(size_t indVert) {
    Node* vertex = &nodes[indVert];
    _liftUpToRoot(vertex);
    Node::push(vertex);
    size_t edgeWeight = vertex->edgeWeight;
    return edgeWeight;
}

Node* LinkCutTree::prevInPath(size_t ind) {
    Node* source = &nodes[ind];
    _expose(findRoot(ind));
    return _leftest(_liftUpToRoot(source));
}

//**********************************************************************************************
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
public:
    long long maxFlow;
    vector <size_t> flow;  //current flow in each edge
    size_t source;    //source and sink in Network
    size_t sink;
    Graph *graph;
    Network(Graph *graph, size_t source, size_t sink);
    ~Network();
    size_t getMaxFlow(FlowFinder& flowFinder);   //workfunction
    
};

class Bfs{
public:
    Bfs();
    ~Bfs();
    vector <bool>* used;
    vector <size_t>* dist;
    
    queue <pair <size_t, size_t> > bfsQueue;
    
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
    //virtual ~FlowFinder() = 0;
    virtual void initFlowFinder(Network* network) = 0;
    virtual void getMaxFlow() = 0;
};

class DinicFlowFinder : public FlowFinder {
private:
    ShortPathNetwork* shortPathNetwork;
    BlockFlowFinder* blockFlowFinder;
public:
    DinicFlowFinder(BlockFlowFinder* blockFlowFinder);
    ~DinicFlowFinder();
    void getMaxFlow();
    void updateFlow();
    void calcMaxFlow();
    void initFlowFinder(Network* network);
    bool getShortPathNetwork();
    bool checkEdgeForShortPath(size_t edgeNumber, DirectEdge& edge);
    Bfs bfs;
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
    //virtual ~BlockFlowFinder() = 0;
    ShortPathNetwork* shortPathNetwork;
    virtual void findBlockFlow() = 0;
};

class LinkCutBlockFlowFinder : public BlockFlowFinder {
private:
    //ShortPathNetwork* shortPathNetwork;
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

//**********************************************************************************************
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


Network::Network(Graph *graph, size_t source, size_t sink): graph(graph), sink(sink), source(source), maxFlow(0)
{
    flow.resize(graph->sizeEdge, 0);
}

Network::~Network()
{
    delete graph;
    //delete flow;
}

size_t Network::getMaxFlow(FlowFinder& flowFinder) {
    flowFinder.initFlowFinder(this);
    flowFinder.getMaxFlow();
    return maxFlow = flowFinder.maxFlow;
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
    vector <DirectEdge>& edgeList = network->graph->edgeList;
    vector <size_t>& flow = network->flow;
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
    vector <DirectEdge>& originEdgeList = network->graph->edgeList;
    vector <DirectEdge>& shortPathEdgeList = shortPathNetwork->graph->edgeList;
    
    for(size_t i = 0;i < shortPathNetwork->edgeID.size(); ++i) {
        originalEdge = originEdgeList[shortPathNetwork->edgeID[i]];
        imageEdge = shortPathEdgeList[i];
        if(originalEdge.start == imageEdge.start) {
            network->flow[shortPathNetwork->edgeID[i]] += shortPathNetwork->flow[i];
        } else {
            network->flow[shortPathNetwork->edgeID[i]] -= shortPathNetwork->flow[i];
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
    vector <DirectEdge>& edgeList = network->graph->edgeList;
    for(size_t i = 0;i < edgeList.size(); ++i) {
        curEdge = edgeList[i];
        if(checkEdgeForShortPath(i, curEdge)) {
            edgeID->push_back(i);
            shortPathEdges.push_back(curEdge);
        }
    }
    
    shortPathNetwork = new ShortPathNetwork(new Graph(network->graph->sizeVert, shortPathEdges),
                                            network->source, network->sink, *edgeID);
    return true;
}

bool DinicFlowFinder::checkEdgeForShortPath(size_t edgeNumber, DirectEdge& edge) {
    if((*bfs.dist)[edge.start] + 1 == (*bfs.dist)[edge.finish] && network->flow[edgeNumber] < edge.capacity && edge.start != network->sink) {
        edge.capacity -= network->flow[edgeNumber];
        return true;
    }
    
    if((*bfs.dist)[edge.finish] + 1 == (*bfs.dist)[edge.start] &&  network->flow[edgeNumber] > 0 && edge.finish != network->sink) {
        std::swap(edge.start, edge.finish);
        edge.capacity = network->flow[edgeNumber];
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
    for(size_t i = 0;i < graph->outgoingList[vert].size();++i) {
        numEdge = graph->outgoingList[vert][i];
        curEdge = (graph->edgeList)[numEdge];
        if(!(*used)[curEdge.finish] && network->flow[numEdge] < curEdge.capacity) {
            (*used)[curEdge.finish] = true;
            (*dist)[curEdge.finish] = levelDist;
            bfsQueue.push(std::make_pair(curEdge.finish, levelDist));
        }
    }
}

void Bfs::checkIncomingEdges(size_t vert, size_t levelDist) {
    size_t numEdge;
    for(size_t i = 0;i < graph->incomingList[vert].size();++i) {
        numEdge = graph->incomingList[vert][i];
        curEdge = graph->edgeList[numEdge];
        if(!(*used)[curEdge.start] && network->flow[numEdge] > 0) {
            (*used)[curEdge.start] = true;
            (*dist)[curEdge.start] = levelDist;
            bfsQueue.push(std::make_pair(curEdge.start, levelDist));
        }
    }
}

bool Bfs::run() {
    size_t vert;
    size_t levelDist;
    
    graph = network->graph;
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
    linkCut.setWeight(linkCut.findRoot(source)->key, INF);
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
    linkCut.removeWeightInPath(minEdge->edgeWeight, source);
    while(linkCut.getEdgeWeight((minEdge = linkCut.getMinEdge(source))->key) == 0) {
        minVert = minEdge->key;
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
    curEdgeNumber.resize(shortPathNetwork->graph->sizeVert, false);
    edgeInsideTreeFlag.resize(shortPathNetwork->graph->sizeVert, false);
    vector <vector <size_t> >& outEdges = shortPathNetwork->graph->outgoingList;
    vector <DirectEdge>& edgeList = shortPathNetwork->graph->edgeList;
    vector <size_t>& flow = shortPathNetwork->flow;
    
    size_t vertex;
    size_t nextVert;
    size_t prevVert;
    
    for(size_t i = 0;i < edgeList.size(); ++i) {
        flow[i] = edgeList[i].capacity;
    }
    
    linkCut.clearTrees();
    
    while(true) {
        if((vertex = linkCut.findRoot(source)->key) != sink) {
            if(curEdgeNumber[vertex] != outEdges[vertex].size()) {
                nextVert = edgeList[outEdges[vertex][curEdgeNumber[vertex]]].finish;
                
                addEdge(vertex, nextVert, outEdges, edgeList);//Step 1
            } else {
                if(vertex == source) {
                    edgeInsideTreeFlag[source] = false;//Step 2
                    break;
                } else {
                    prevVert = linkCut.prevInPath(source)->key;
                    
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

//**********************************************************************************************

void solve();
void linkCutTest();

int main() {
    ios_base::sync_with_stdio(false);
    
    //freopen("output.txt", "w", stdout);
    //while(1) {
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
    
    //Graph graph(vert, edgeList);
    //LinkCutBlockFlowFinder* linkCutBlockFlowFinder = new LinkCutBlockFlowFinder(vert, 0, vert - 1);
    //dinicFlowFinder(new LinkCutBlockFlowFinder(vert, 0, vert - 1));
    //    Network network(new Graph(vert,edgeList), 0, vert - 1);
    //    network.getMaxFlow(*(new DinicFlowFinder(new LinkCutBlockFlowFinder(vert, 0, vert - 1))));
    //
    //
    Graph* graph = new Graph(vert, edgeList);
    LinkCutBlockFlowFinder linkCutBlockflowFinder(vert, 0, vert - 1);
    DinicFlowFinder dinicFlowFinder(&linkCutBlockflowFinder);
    Network network(graph, 0, vert - 1);
    network.getMaxFlow(dinicFlowFinder);
    cout << network.maxFlow << endl;
    //delete graph;
    //delete graph;
    
        for(size_t i = 0;i < network.flow.size(); ++i) {
            cout << network.flow[i] << endl;
        }
}

