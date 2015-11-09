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


//#define SIZE_T_MAX 1e15
using namespace std;

//**********************************************************************************************

class Node;
class SplayTree;

const size_t INF = 1e16;

size_t getSize(Node* vertex);
size_t getMin(Node* vertex);
size_t getSum(Node* vertex);
void removeWeight(size_t value, Node* vertex);
void updateTreeSize(Node* vertex);


class Node{
public:
    Node(size_t key, size_t weight = 0);
    size_t key;
    size_t edgeWeight;
    size_t subtreeMinWeight;
    size_t removedWeightValue;
    //bool reverseFlag;
    Node* leftChild;
    Node* rightChild;
    Node* parent;
    Node* link;
    size_t sizeOfSubtree;
    SplayTree* treePtr;
    
    void push();
    //void reverse();
    //void removeValue(size_t value, Node* vertex);
    //void reverse(Node* vertex);
    void recursiveDelete();
};

class SplayTree
{
private:
    Node* find(size_t position, Node* vertex);
    void keepParent(Node* vertex);
    void setParent(Node* parent, Node* vertex);
    void rotate(Node* parent, Node* vertex);
public:
    Node* root;
    SplayTree(Node* root);
    ~SplayTree();
    
    void insert(int key, size_t position);
    void remove(size_t position);
    void merge(SplayTree* addedTree); //added tree is right merged tree
    SplayTree* split(size_t position); //returned tree is tight splited tree
    Node* find(size_t position);
    
    void splay(Node* vertex);
    
    Node* getRoot() { return root; };
};


//**********************************************************************************************



Node::Node(size_t key, size_t edgeWeight) {
    this->key = key;
    sizeOfSubtree = 1;
    leftChild = nullptr;
    rightChild = nullptr;
    parent = nullptr;
    link = nullptr;
    subtreeMinWeight = edgeWeight;
    removedWeightValue = 0;
    sizeOfSubtree = 1;
    treePtr = nullptr;
    
    this->edgeWeight = edgeWeight;
}

void Node::recursiveDelete() {
    if(leftChild) {
        leftChild->recursiveDelete();
    }
    
    if(rightChild) {
        rightChild->recursiveDelete();
    }
    
    this->~Node();
}

void removeWeight(size_t value, Node* vertex) {
    if(vertex) {
        vertex->removedWeightValue += value;
    }
}

void Node::push() {
    edgeWeight -= removedWeightValue;
    removeWeight(removedWeightValue, leftChild);
    removeWeight(removedWeightValue, rightChild);
    removedWeightValue = 0;
    updateTreeSize(this);
}

SplayTree::SplayTree(Node* root) {
    this->root = root;
    if(root) {
        root->treePtr = this;
    }
}

SplayTree::~SplayTree() {
    if(root) {
        root->recursiveDelete();
    }
}

void SplayTree::setParent(Node* vertex, Node* parent) {
    if(vertex) {
        vertex->parent = parent;
    }
}

size_t getSize(Node* vertex) {
    if(vertex) {
        return vertex->sizeOfSubtree;
    }
    return 0;
}

size_t getMin(Node* vertex) {
    if(vertex) {
        return vertex->subtreeMinWeight - vertex->removedWeightValue;
    }
    return INF;
}

void updateTreeSize(Node* vertex) {
    if(vertex) {
        vertex->sizeOfSubtree = getSize(vertex->leftChild) + getSize(vertex->rightChild) + 1;
        vertex->subtreeMinWeight = min(min(getMin(vertex->leftChild), getMin(vertex->rightChild)), vertex->edgeWeight);
    }
}

void SplayTree::keepParent(Node* vertex) {
    setParent(vertex->leftChild, vertex);
    setParent(vertex->rightChild, vertex);
    updateTreeSize(vertex);
    //vertex->push();
}

void SplayTree::rotate(Node* parent, Node* vertex) {
    Node* grandParent = parent->parent;
    
    if(grandParent) {
        grandParent->push();
    }
    
    parent->push();
    vertex->push();
    
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
    
    keepParent(parent);
    keepParent(vertex);
    
    //updateTreeSize(vertex);
    //updateTreeSize(grandParent);
    
    setParent (vertex, grandParent);
}

void SplayTree::splay(Node* vertex){
    if(!vertex->parent) {
        root = vertex;
        root->treePtr = this;
        return;
    }
    
    Node* parent = vertex->parent;
    Node* grandParent = parent->parent;
    
    if(!grandParent) {
        rotate(parent, vertex);
        root = vertex;
        root->treePtr = this;
        return;
    }
    
    //grandParent->push();
    //parent->push();
    //vertex->push();
    
    bool zigZigFlag = ((grandParent->leftChild == parent) == (parent->leftChild == vertex));
    
    if(zigZigFlag) {
        rotate(grandParent, parent);
        rotate(parent, vertex);
    } else {
        rotate(parent, vertex);
        rotate(grandParent, vertex);
    }
    
    splay(vertex);
    return;
}

Node* SplayTree::find(size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position >= treeSize) {
        return NULL;
        // throw std::out_of_range("out of range in SplayTree::find\n");
    }
    
    return find(position, root);
}

Node* SplayTree::find(size_t position, Node* vertex) {
    vertex->push();
    size_t indexLeft = (vertex->leftChild ? vertex->leftChild->sizeOfSubtree : 0);
    
    if(position == indexLeft) {
        splay(vertex);
        return vertex;
    }
    
    if(position < indexLeft) {
        return find(position, vertex->leftChild);
    }
    
    return find(position - indexLeft - 1, vertex->rightChild);
}


SplayTree* SplayTree::split(size_t position){
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        return NULL;
        //    throw std::out_of_range("out of range in SplayTree::split\n");
    }
    
    if(position == treeSize) {
        return new SplayTree(nullptr);
    }
    
    Node* newRoot = find(position, root);
    
    SplayTree* rightTree = new SplayTree(newRoot);
    
    root = newRoot->leftChild;
    newRoot->leftChild = nullptr;
    setParent(root, nullptr);
    
    if(rightTree->root) {
        rightTree->root->treePtr = rightTree;
        rightTree->root->push();
    }
    
    if(root) {
        root->push();
    }
    
    // updateTreeSize(root);
    //updateTreeSize(rightTree->root);
    
    return rightTree;
}

void SplayTree::insert(int key, size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        return;
        //    throw std::out_of_range("out of range in SplayTree::insert\n");
    }
    
    SplayTree* rightTree = split(position);
    Node* newRoot = new Node(key);
    newRoot->treePtr = this;
    newRoot->leftChild = this->root;
    newRoot->rightChild = rightTree->root;
    root = newRoot;
    keepParent(root);
    
    rightTree->root = nullptr;
    
    rightTree->~SplayTree();
}

void SplayTree::merge(SplayTree* addedTree) {
    if(!addedTree || !addedTree->root)
    {
        return;
    }
    
    if(!root) {
        root = addedTree->root;
        addedTree->root = nullptr;
        return;
    }
    
    find(root->sizeOfSubtree - 1);
    addedTree->find(0);
    
    root->push();
    
    root->rightChild = addedTree->root;
    addedTree->root = nullptr;
    keepParent(root);
}

void SplayTree::remove(size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position >= treeSize) {
        return;
        //throw std::out_of_range("out of range in SplayTree::remove\n");
    }
    
    find(position);
    SplayTree* leftTree = new SplayTree(root->leftChild);
    SplayTree* rightTree = new SplayTree(root->rightChild);
    
    setParent(leftTree->root, nullptr);
    setParent(rightTree->root, nullptr);
    leftTree->merge(rightTree);
    
    delete root;
    root = leftTree->root;
    leftTree->root = nullptr;
    
    if(root) {
        root->treePtr = this;
    }
    
    updateTreeSize(root);
    
    leftTree->~SplayTree();
    rightTree->~SplayTree();
}
//**********************************************************************************************
class LinkCutTree {
private:
    //std::vector <SplayTree>* trees;
    //std::map <Node*, size_t> numberOfTree;
    SplayTree* getTree(Node* vertex);
    Node* cutout(Node* vertex);
    Node* cleanUp(Node* vertex);
    Node* getLca(Node* vertex1, Node* vertex2);
    Node* findLeftestMin(size_t minValue, Node* vertex);
public:
    Node* lastExposed;
    // LinkCutTree(std::vector<SplayTree>* trees);
    ~LinkCutTree();
    void makeTree(Node* vertex);
    //void revert(Node* vertex);
    void removeWeightInPath(size_t weight, Node* vertex);
    void cut(Node* vertex, Node* parent);
    void link(Node* treeRoot, Node* vertex);
    void linkEdge(Node* vertex1, Node* vertex2);
    void cutEdge(Node* vertex1, Node* vertex2);
    void setWeight(Node* vertex, size_t weight);
    size_t getEdgeWeight(Node* vertex);
    size_t getDist(Node* vertex1, Node* vertex2);
    Node* lca(Node* vertex1, Node* vertex2);
    Node* supportRoot(Node* vertex);   //it's splay current vertex
    Node* getMinEdge(Node* vertex);
    Node* findRoot(Node* vertex);
    Node* leftest(Node* root);
    size_t dist(Node* vertex1, Node* vertex2);
    size_t depth(Node* vertex);
    
    Node* expose(Node* vertex);
};

//**********************************************************************************************


LinkCutTree::~LinkCutTree() {
}

void LinkCutTree::link(Node* treeRoot, Node* vertex) {
    treeRoot->link = vertex;
    expose(treeRoot);
}

void LinkCutTree::cut(Node* vertex, Node* parent) {
    expose(parent);
    vertex->link = nullptr;
    //expose(vertex);
}

Node* LinkCutTree::findRoot(Node* vertex) {
    if(vertex != lastExposed) {
        expose(vertex);
    }
    return leftest(supportRoot(vertex));
}

Node* LinkCutTree::cleanUp(Node* vertex) {
    Node* root;
    if(vertex->parent) {
        root = cleanUp(vertex->parent);
    } else {
        root = vertex;
    }
    
    vertex->push();
    return root;
}

inline Node* LinkCutTree::supportRoot(Node* vertex) {
    if(!vertex) {
        return nullptr;
    }
    
    if(!vertex->parent) {
        return vertex;
    }
    
    Node* root = cleanUp(vertex);
    root->treePtr->splay(vertex);
    return vertex;
}

Node* LinkCutTree::leftest(Node* root) {
    return root->treePtr->find(0);
}

Node* LinkCutTree::cutout(Node* vertex) {
    supportRoot(vertex);
    SplayTree* left = vertex->treePtr;
    SplayTree* right = left->split(getSize(vertex->leftChild) + 1);
    if(right->getRoot()) {
        right->find(0)->link = vertex;
    }
    return vertex;
}

//inline Node* LinkCutTree::expose(Node* vertex) {
//    Node* memVert = vertex;
//    Node* next;
//    vertex = leftest(cutout(vertex));
//    while(vertex->link != nullptr) {
//        next = cutout(vertex->link);
//        vertex->link = nullptr;
//        (next->treePtr)->merge(vertex->treePtr);//<---!!!
//        vertex = leftest(supportRoot(vertex));
//    }
//    return supportRoot(memVert);
//}
Node* LinkCutTree::expose(Node* vertex) {
    lastExposed = vertex;
    Node* next;
    vertex = leftest(supportRoot(cutout(vertex)));
    while(vertex->link != nullptr) {
        next = cutout(vertex->link);
        vertex->link = nullptr;
        (supportRoot(next)->treePtr)->merge(supportRoot(vertex)->treePtr);
        vertex = leftest(supportRoot(vertex));
    }
    return vertex;
}

Node* LinkCutTree::getMinEdge(Node* vertex) {
    expose(vertex);
    //expose(findRoot(vertex));
    supportRoot(vertex);
    //vertex->push();
    size_t minValue = getMin(vertex);
    return findLeftestMin(minValue, vertex);
}

Node* LinkCutTree::findLeftestMin(size_t minValue, Node* vertex) {
    vertex->push();
    if(getMin(vertex->leftChild) == minValue) {
        return findLeftestMin(minValue, vertex->leftChild);
    }
    
    if(vertex->edgeWeight == minValue) {
        //expose(vertex);
        return supportRoot(vertex);
    }
    
    return findLeftestMin(minValue, vertex->rightChild);
}

void LinkCutTree::setWeight(Node* vertex, size_t weight) {
    supportRoot(vertex);
    vertex->edgeWeight = weight;
    updateTreeSize(vertex);
    //expose(vertex);
}

void LinkCutTree::removeWeightInPath(size_t added, Node* vertex) {
    expose(vertex);
    //expose(findRoot(vertex));
    supportRoot(vertex);
    removeWeight(added, vertex);
}

size_t LinkCutTree::getEdgeWeight(Node* vertex) {
    supportRoot(vertex);
    vertex->push();
    size_t edgeWeight = vertex->edgeWeight;
    //expose(vertex);
    return edgeWeight;
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
    Bfs();
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


//**********************************************************************************************
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
    
    vector <DirectEdge>& edgeList = network->graph->edgeList;
    maxFlow = 0;
    
    for(size_t i = 0;i < edgeList.size(); ++i) {
        if(edgeList[i].start == network->source) {
            maxFlow += (*network->flow)[i];
        }
        if(edgeList[i].finish == network->source) {
            maxFlow -= (*network->flow)[i];
        }
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
    if(edgeID)
        delete edgeID;
}

void LinkCutBlockFlowFinder::init(Network* network) {
    /*if(nodes.empty()) {
     for(size_t i = 0;i < network->graph->sizeVert; ++i) {
     nodes.push_back(new Node(i));
     trees.push_back(new SplayTree(nodes[i]));
     }
     // vertexInShortPathNetwork->resize(network->graph->sizeVert);
     return;
     }*/
    
    for(size_t i = 0;i < network->graph->sizeVert; ++i) {
        nodes.push_back(new Node(i, INF));
        trees.push_back(new SplayTree(nodes[i]));
    }
    
    source = network->source;
    sink = network->sink;
}

//void LinkCutBlockFlowFinder::clearTree() {
//    for(size_t i = 0;i < nodes.size(); ++i) {
//        delete nodes[i];
//        delete trees[i];
//        nodes[i] = new Node(i);
//        trees[i] = new SplayTree(nodes[i]);
//    }
//}

void LinkCutBlockFlowFinder::clearTree() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        nodes[i]->leftChild = nodes[i]->rightChild = nullptr;
        nodes[i]->parent = nodes[i]->link = nullptr;
        nodes[i]->treePtr = trees[i];
        nodes[i]->sizeOfSubtree = 1;
        nodes[i]->edgeWeight = nodes[i]->removedWeightValue = 0;
        trees[i]->root = nodes[i];
    }
}

void LinkCutBlockFlowFinder::findBlockFlow(ShortPathNetwork& shortPathNetwork) {
    vector <size_t> curEdgeNumber(shortPathNetwork.graph->sizeVert, false);
    vector <vector <size_t> >* outEdges = shortPathNetwork.graph->outgoingList;
    vector <DirectEdge>& edgeList = shortPathNetwork.graph->edgeList;
    vector <bool> edgeInsideFlag(shortPathNetwork.graph->sizeVert, false);
    size_t vertex;
    size_t nextVert;
    size_t prevVert;
    
    for(size_t i = 0;i < edgeList.size(); ++i) {
        (*shortPathNetwork.flow)[i] = edgeList[i].capacity;
    }
    
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
                    prevVert = prevInPath(nodes[source])->key;
                    linkCut.cut(nodes[prevVert], nodes[vertex]);
                    edgeList[(*outEdges)[prevVert][curEdgeNumber[prevVert]]].capacity = linkCut.getEdgeWeight(nodes[prevVert]);
                    linkCut.setWeight(nodes[prevVert], INF);
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
                linkCut.cut(nodes[minVert], nodes[edgeList[(*outEdges)[minVert][curEdgeNumber[minVert]]].finish]);
                linkCut.setWeight(nodes[minVert], INF);
                ++curEdgeNumber[minVert];
                edgeInsideFlag[minVert] = false;
                if(minVert == source) {
                    break;
                }
            }
        }
    }
    
    //std::cout << "block: " << (clock() - clock7)/CLOCKS_PER_SEC << std::endl;
    
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
//**********************************************************************************************

void solve();
void linkCutTest();

int main() {
    ios_base::sync_with_stdio(false);
//    freopen("input.txt", "r", stdin);
//    freopen("output.txt", "w", stdout);
    solve();
    //linkCutTest();
    //cout << "aaa" << endl;
    return 0;
}

void solve() {
    long long vert, edge, to, from, capacity;
    vector <DirectEdge> edgeList;
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
        edgeList.push_back(curEdge);
    }
    
    //cout <<"OK" << endl;
    
    Graph graph(vert, edgeList);
    LinkCutBlockFlowFinder linkCutBlockflowFinder;
    DinicFlowFinder dinicFlowFinder(&linkCutBlockflowFinder);
    Network network(&graph, 0, vert - 1);
    network.getMaxFlow(dinicFlowFinder);
    //std::cout << "end: " << (clock() - clock7)/CLOCKS_PER_SEC << std::endl;
    //std::cout << "sum: " << dinicFlowFinder.sumTime << std::endl;
    
    //cout << "OK" << endl;
    
    cout << dinicFlowFinder.maxFlow << endl;
    
//    for(size_t i = 0;i < network.flow->size(); ++i) {
//        cout << (*network.flow)[i] << endl;
//    }
}
