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

/*
class Node;
class SplayTree;

size_t size(Node* vertex);
size_t sumWeight(Node* vertex);
size_t minWeight(Node* vertex);

class Node{
public:
    Node(size_t key, size_t weight = 0);
    size_t key;
    size_t edgeWeight;
    size_t subtreeMinWeight;
    size_t subtreeSumWeight;
    size_t removedWeightValue;
    bool reverseFlag;
    Node* leftChild;
    Node* rightChild;
    Node* parent;
    Node* link;
    size_t sizeOfSubtree;
    SplayTree* treePtr;
    
    void reverse();
    void removeValue(size_t value, Node* vertex);
    void reverse(Node* vertex);
    void push();
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
    void updateTreeSize(Node* vertex);
    void remove(size_t position);
    void merge(SplayTree* addedTree); //added tree is right merged tree
    SplayTree* split(size_t position); //returned tree is tight splited tree
    Node* find(size_t position);
    
    void splay(Node* vertex);
    
    Node* getRoot() { return root; };

};

Node::Node(size_t key, size_t weight) {
    this->key = key;
    sizeOfSubtree = 1;
    leftChild = nullptr;
    rightChild = nullptr;
    parent = nullptr;
    treePtr = nullptr;
    link = nullptr;
    reverseFlag = false;
    edgeWeight = weight;
    removedWeightValue = 0;
    subtreeMinWeight = weight;
    subtreeSumWeight = weight;
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

void Node::push() {
    if(reverseFlag) { //guarantee that this != nullptr
        std::swap(leftChild, rightChild);
        reverseFlag = false;
        
        reverse(leftChild);
        reverse(rightChild);
    }
    if(removedWeightValue) {
        edgeWeight -= removedWeightValue;
        removeValue(removedWeightValue, leftChild);
        removeValue(removedWeightValue, rightChild);
        removedWeightValue  = 0;
        treePtr->updateTreeSize(this);
    }
}

void Node::reverse() {
    reverse(this);
}

void Node::removeValue(size_t value, Node* vertex) {
    if(vertex) {
        vertex->removedWeightValue += value;
    }
}

void Node::reverse(Node* vertex) {
    if(vertex) {
        vertex->reverseFlag ^= true;
    }
}

void SplayTree::setParent(Node* vertex, Node* parent) {
    if(vertex) {
        vertex->parent = parent;
    }
}

void SplayTree::updateTreeSize(Node* vertex) {
    if(vertex) {
        vertex->sizeOfSubtree = size(vertex->leftChild) + size(vertex->rightChild) + 1;
        vertex->subtreeSumWeight = sumWeight(vertex->leftChild) + sumWeight(vertex->rightChild) + vertex->edgeWeight;
        vertex->subtreeMinWeight = min(min(minWeight(vertex->leftChild), minWeight(vertex->rightChild)), vertex->edgeWeight);
    }
}

void SplayTree::keepParent(Node* vertex) {
    setParent(vertex->leftChild, vertex);
    setParent(vertex->rightChild, vertex);
}

void SplayTree::rotate(Node* parent, Node* vertex) {
    Node* grandParent = parent->parent;
    
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
    
    keepParent(vertex);
    keepParent(parent);
    
    updateTreeSize(parent);
    updateTreeSize(vertex);
    updateTreeSize(grandParent);
    
    setParent (vertex, grandParent);
}

void SplayTree::splay(Node* vertex){
    if(!vertex->parent) {
        root = vertex;
        vertex->treePtr = this;
        return;
    }
    
    Node* parent = vertex->parent;
    Node* grandParent = parent->parent;
    
    if(!grandParent) {
        rotate(parent, vertex);
        root = vertex;
        //setParent(vertex, nullptr);
        vertex->treePtr = this;
        return;
    }
    
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
        throw std::out_of_range("out of range in SplayTree::find\n");
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
        throw std::out_of_range("out of range in SplayTree::split\n");
    }
    
    if(position == treeSize) {
        return new SplayTree(nullptr);
    }
    
    Node* newRoot = find(position, root);
    
    SplayTree* rightTree = new SplayTree(newRoot);
    
    root = newRoot->leftChild;
    newRoot->leftChild = nullptr;
    setParent(root, nullptr);
    
    updateTreeSize(root);
    updateTreeSize(rightTree->root);
    
    return rightTree;
}

void SplayTree::insert(int key, size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        throw std::out_of_range("out of range in SplayTree::insert\n");
    }
    
    SplayTree* rightTree = split(position);
    Node* newRoot = new Node(key);
    newRoot->leftChild = this->root;
    newRoot->rightChild = rightTree->root;
    root = newRoot;
    keepParent(root);
    
    updateTreeSize(root);
    
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
    
    find(this->root->sizeOfSubtree - 1);
    addedTree->find(0);
    
    this->root->rightChild = addedTree->root;
    addedTree->root = nullptr;
    keepParent(this->root);
    updateTreeSize(this->root);
}

void SplayTree::remove(size_t position) {
    size_t treeSize = (root ? root->sizeOfSubtree : 0);
    
    if(position >= treeSize) {
        throw std::out_of_range("out of range in SplayTree::remove\n");
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
    
    updateTreeSize(root);
    
    leftTree->~SplayTree();
    rightTree->~SplayTree();
}

size_t size(Node* vertex) {
    return (vertex ? vertex->sizeOfSubtree : 0);
}

size_t sumWeight(Node* vertex) {
    return (vertex ? vertex->subtreeSumWeight - vertex->sizeOfSubtree * vertex->removedWeightValue : 0);
}

size_t minWeight(Node* vertex) {
    return (vertex ? vertex->subtreeMinWeight -= vertex->removedWeightValue : SIZE_T_MAX);
}

//*******************************************************************************************************

class LinkCutTree {
private:
    //std::vector <SplayTree>* trees;
    //std::map <Node*, size_t> numberOfTree;
    SplayTree* getTree(Node* vertex);
    Node* supportRoot(Node* vertex);   //it's splay current vertex
    Node* leftest(Node* root);
    Node* cutout(Node* vertex);
    Node* cleanUp(Node* vertex);
    Node* getLca(Node* vertex1, Node* vertex2);
    Node* findLeftestMin(size_t minValue, Node* vertex);
public:
    // LinkCutTree(std::vector<SplayTree>* trees);
    ~LinkCutTree();
    void makeTree(Node* vertex);
    void revert(Node* vertex);
    void removeWeightInPath(size_t weight, Node* vertex);
    void cut(Node* vertex, Node* parent);
    void link(Node* treeRoot, Node* vertex);
    void linkEdge(Node* vertex1, Node* vertex2);
    void cutEdge(Node* vertex1, Node* vertex2);
    void setWeight(Node* vertex, size_t weight);
    size_t getEdgeWeight(Node* vertex);
    size_t getDist(Node* vertex1, Node* vertex2);
    Node* lca(Node* vertex1, Node* vertex2);
    Node* getMinEdge(Node* vertex);
    Node* findRoot(Node* vertex);
    size_t dist(Node* vertex1, Node* vertex2);
    size_t depth(Node* vertex);
    
    Node* expose(Node* vertex);
};

LinkCutTree::~LinkCutTree() {
    /* for(int i  = 0;i < trees->size(); ++i) {
     (*trees)[i].~SplayTree();
     }*
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
    expose(vertex);
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

Node* LinkCutTree::supportRoot(Node* vertex) {
    if(!vertex) {
        return nullptr;
    }
    
    Node* root = cleanUp(vertex);
    
    SplayTree* currentTree = root->treePtr;
    currentTree->splay(vertex);
    return vertex;
}

Node* LinkCutTree::leftest(Node* root) {
    return root->treePtr->find(0);
}

size_t LinkCutTree::depth(Node* vertex) {
    expose(vertex);
    return supportRoot(vertex)->sizeOfSubtree - 1;
}

Node* LinkCutTree::cutout(Node* vertex) {
    supportRoot(vertex);
    SplayTree* left = vertex->treePtr;
    SplayTree* right = left->split(size(vertex->leftChild) + 1);
    if(right->getRoot()) {
        right->find(0)->link = vertex;
    }
    return vertex;
}

Node* LinkCutTree::expose(Node* vertex) {
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

void LinkCutTree::revert(Node* vertex) {
    Node* root = expose(vertex);
    supportRoot(root);
    root->reverse();
}

void LinkCutTree::linkEdge(Node* vertex1, Node* vertex2) {
    revert(vertex1);
    link(vertex1, vertex2);
}

void LinkCutTree::cutEdge(Node* vertex1, Node* vertex2) {
    revert(vertex2);
    cut(vertex1, vertex2);
}

size_t LinkCutTree::dist(Node* vertex1, Node* vertex2) {
    revert(vertex2);
    return depth(vertex1);
}

Node* LinkCutTree::lca(Node* vertex1, Node* vertex2) {
    if(vertex1 == vertex2) {
        return vertex1;
    }
    Node* candidate1 = getLca(vertex1, vertex2);
    Node* candidate2 = getLca(vertex2, vertex1);
    if(candidate1 == candidate2) {
        return candidate1;
    } else {
        if(candidate2 == vertex2 || candidate2 == vertex1) {
            return candidate2;
        } else {
            return candidate1;
        }
    }
}

Node* LinkCutTree::getLca(Node* vertex1, Node* vertex2) {
    if(!vertex1 || !vertex2) {
        return nullptr;
    }
    
    expose(vertex1);
    expose(vertex2);
    return leftest(supportRoot(vertex1))->link;
}

size_t LinkCutTree::getDist(Node* vertex1, Node* vertex2) {
    size_t dist = 0;
    
    if(vertex1 == vertex2) {
        return dist;
    }
    
    Node* lcaVert = lca(vertex1, vertex2);
    
    if(!lcaVert) {
        return SIZE_T_MAX;
    }
    
    if(lcaVert == vertex2) {
        std::swap(vertex1, vertex2);
    }
    
    expose(vertex2);
    expose(vertex1);
    expose(lcaVert);
    
    dist += sumWeight(supportRoot(vertex2));
    
    if(vertex1 != lcaVert) {
        dist += sumWeight(supportRoot(vertex1));
    }
    
    return dist;
}

Node* LinkCutTree::getMinEdge(Node* vertex) {
    expose(findRoot(vertex));
    supportRoot(vertex);
    vertex->push();
    size_t minValue = minWeight(vertex);
    return findLeftestMin(minValue, vertex);
}

Node* LinkCutTree::findLeftestMin(size_t minValue, Node* vertex) {
    vertex->push();
    if(minWeight(vertex->leftChild) == minValue) {
        return findLeftestMin(minValue, vertex->leftChild);
    }
    
    if(minWeight(vertex) == minValue) {
        return vertex;
    }
    
    return findLeftestMin(minValue, vertex->rightChild);
}

void LinkCutTree::setWeight(Node* vertex, size_t weight) {
    supportRoot(vertex);
    vertex->edgeWeight = weight;
    vertex->treePtr->updateTreeSize(vertex);
    expose(vertex);
}

void LinkCutTree::removeWeightInPath(size_t added, Node* vertex) {
    expose(findRoot(vertex));
    supportRoot(vertex);
    vertex->removedWeightValue += added;
}

size_t LinkCutTree::getEdgeWeight(Node* vertex) {
    supportRoot(vertex);
    size_t edgeWeight = vertex->edgeWeight;
    expose(vertex);
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
    vector <vector <size_t> > *outgoingList = nullptr; //lists of numbers of edges that outgoing and incoming in vertex
    vector <vector <size_t> > *incomingList = nullptr;
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
    vector <bool> used;
    vector <size_t> dist;
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
    vector <SplayTree*> trees;
    vector <Node*> nodes;
    size_t source;
    size_t sink;
public:
    void clearTree();
    void init(Network* network);
    void findBlockFlow(ShortPathNetwork& shortPathNetwork);
};

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
}

void DinicFlowFinder::initFlowFinder(Network* network) {
    this->network = network;
    maxFlow = 0;
}

void DinicFlowFinder::calcMaxFlow() {
    
    blockFlowFinder->init(network);
    while(getShortPathNetwork()) {
        blockFlowFinder->findBlockFlow(*shortPathNetwork);
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
    };
    
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
            shortPathEdges->push_back(curEdge);
        }
    }
    
    
    if(shortPathNetwork) {
        delete shortPathNetwork;
    }
    
    shortPathNetwork = new ShortPathNetwork(new Graph(network->graph->sizeVert, *shortPathEdges),
                                            network->source, network->sink, edgeID);
    return true;
}

bool DinicFlowFinder::checkEdgeForShortPath(size_t edgeNumber, DirectEdge& edge) {
    if(bfs.dist[edge.start] + 1 == bfs.dist[edge.finish] && (*network->flow)[edgeNumber] < edge.capacity && edge.start != network->sink) {
        edge.capacity -= (*network->flow)[edgeNumber];
        return true;
    }
    
    if(bfs.dist[edge.finish] + 1 == bfs.dist[edge.start] &&  (*network->flow)[edgeNumber] > 0 && edge.start != network->sink) {
        std::swap(edge.start, edge.finish);
        edge.capacity = (*network->flow)[edgeNumber];
        return true;
    }
    
    return false;
}

void Bfs::init(Network* network) {
    size_t sizeVert = network->graph->sizeVert;
    used.clear();
    used.resize(sizeVert, false);
    dist.clear();
    dist.resize(sizeVert, SIZE_T_MAX);
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
    
    dist[source] = 0;
    used[source] = true;
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
            if(!used[curEdge.finish] && (*network->flow)[numEdge] < curEdge.capacity) {
                used[curEdge.finish] = true;
                dist[curEdge.finish] = levelDist;
                bfsQueue.push(std::make_pair(curEdge.finish, levelDist));
            }
        }
        
        for(size_t i = 0;i < (*graph->incomingList)[vert].size();++i) {
            numEdge = (*graph->incomingList)[vert][i];
            curEdge = (graph->edgeList)[numEdge];
            if(!used[curEdge.start] && (*network->flow)[numEdge] > 0) {
                used[curEdge.start] = true;
                used[curEdge.start] = levelDist;
                bfsQueue.push(std::make_pair(curEdge.start, levelDist));
            }
        }
    }
    
    return used[sink];
}

ShortPathNetwork::ShortPathNetwork(Graph* graph, size_t source, size_t sink, vector <size_t>* edgeID):
Network(graph, source, sink){
    this->edgeID = edgeID;
}

ShortPathNetwork::~ShortPathNetwork() {
    //  delete[] edgeID;
}

void LinkCutBlockFlowFinder::init(Network* network) {
    trees.clear();
    nodes.clear();
    for(size_t i = 0;i < network->graph->sizeVert; ++i) {
        nodes.push_back(new Node(i));
        trees.push_back(new SplayTree(nodes[i]));
    }
    
    source = network->source;
    sink = network->sink;
}

void LinkCutBlockFlowFinder::clearTree() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        nodes[i]->leftChild = nodes[i]->rightChild = nullptr;
        nodes[i]->parent = nodes[i]->link = nullptr;
        nodes[i]->treePtr = trees[i];
        nodes[i]->sizeOfSubtree = 1;
        nodes[i]->reverseFlag = false;
        nodes[i]->edgeWeight = nodes[i]->removedWeightValue = 0;
        nodes[i]->subtreeMinWeight = nodes[i]->subtreeSumWeight = 0;
        trees[i]->root = nodes[i];
    }
}

void LinkCutBlockFlowFinder::findBlockFlow(ShortPathNetwork& shortPathNetwork) {
    vector <size_t> curEdgeNumber(shortPathNetwork.graph->sizeVert, 0);
    vector <vector <size_t> >* outEdges = shortPathNetwork.graph->outgoingList;
    vector <DirectEdge>& edgeList = shortPathNetwork.graph->edgeList;
    //vector <size_t> startCapacity(edgeList.size());
    vector <size_t> prev(shortPathNetwork.graph->sizeVert, 0);
    vector <size_t> next(shortPathNetwork.graph->sizeVert, 0);
    LinkCutTree linkCut;
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
                prev[nextVert] = vertex;
                next[vertex] = nextVert;
                linkCut.setWeight(nodes[vertex], edgeList[(*outEdges)[vertex][curEdgeNumber[vertex]]].capacity);
                linkCut.link(nodes[vertex], nodes[nextVert]);
                continue;
            } else {
                if(vertex == source) {
                    break;
                } else {
                    prevVert = prev[vertex];
                    linkCut.cut(nodes[prevVert], nodes[vertex]);
                    edgeList[(*outEdges)[prevVert][curEdgeNumber[prevVert]]].capacity = linkCut.getEdgeWeight(nodes[prevVert]);
                    ++curEdgeNumber[prevVert];
                }
            }
        } else {
            Node* minEdge = linkCut.getMinEdge(nodes[source]);
            size_t minVert;
            linkCut.removeWeightInPath(minEdge->edgeWeight, nodes[source]);
            while(linkCut.getEdgeWeight(minEdge = linkCut.getMinEdge(nodes[source])) == 0) {
                
                minVert = minEdge->key;
                edgeList[(*outEdges)[minVert][curEdgeNumber[minVert]]].capacity = 0;
                linkCut.cut(nodes[minVert], nodes[next[minVert]]);
                ++curEdgeNumber[minVert];
                if(minVert == source) {
                    break;
                }
            }
        }
    }
    
    DirectEdge curEdge;
    for(size_t i = 0;i < shortPathNetwork.flow->size(); ++i) {
        curEdge = edgeList[i];
        if(curEdgeNumber[curEdge.start] != (*outEdges)[curEdge.start].size()
           && (*outEdges)[curEdge.start][curEdgeNumber[curEdge.start]] == i) {
            (*shortPathNetwork.flow)[i] = (*shortPathNetwork.flow)[i] - linkCut.getEdgeWeight(nodes[edgeList[i].start]);
        } else {
            (*shortPathNetwork.flow)[i] = (*shortPathNetwork.flow)[i] - edgeList[i].capacity;
        }
    }
    
    clearTree();
}

*/
//**********************************************************************************************

void solve();
void linkCutTest();

int main() {
    solve();
    //linkCutTest();
    return 0;
}


void solve() {
    size_t vert, edge, to, from , capacity;
    vector <DirectEdge> edgeList;
    DirectEdge curEdge;
    cin >> vert >> edge;
    for(size_t i = 0;i < edge;++i) {
        cin >> to >> from >> capacity;
        --to;
        --from;
        curEdge.start = to;
        curEdge.finish = from;
        curEdge.capacity = capacity;
        edgeList.push_back(curEdge);
    }
    
    Graph graph(vert, edgeList);
    LinkCutBlockFlowFinder linkCutBlockflowFinder;
    DinicFlowFinder dinicFlowFinder(&linkCutBlockflowFinder);
    Network network(&graph, 0, vert - 1);
    network.getMaxFlow(dinicFlowFinder);
    
    cout << endl;
    
    cout << dinicFlowFinder.maxFlow << endl;
    
    for(size_t i = 0;i < network.flow->size(); ++i) {
        cout << (*network.flow)[i] << endl;
    }
}

//4 7
//1 4 10
//1 2 10
//2 3 1
//3 2 10
//3 4 10
//4 3 10
//2 1 10

void linkCutTest() {
    size_t weights[10] = {0, 5, 10, 3, 4, 3, 4, 3, 0, 0};
    std::vector <Node> nodes(10, *(new Node(0)));
    std::vector <SplayTree> trees;
    for(int i = 0;i < 10; ++i) {
        nodes[i].key = i;
        nodes[i].edgeWeight = weights[i];
        trees.push_back(*(new SplayTree(&nodes[i])));
    }
    LinkCutTree* tree = new LinkCutTree();
    
    tree->link(&nodes[1], &nodes[2]);
    tree->link(&nodes[3], &nodes[2]);
    tree->link(&nodes[4], &nodes[1]);
    tree->link(&nodes[5], &nodes[4]);
    tree->link(&nodes[6], &nodes[4]);
    tree->link(&nodes[7], &nodes[6]);
    for(int i = 1;i < 8;++i) {
        //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
        //  }
    }
    
    Node* lca61 = tree->lca(&nodes[6], &nodes[1]);
    Node* lca73 = tree->lca(&nodes[7], &nodes[3]);
    Node* lca57 = tree->lca(&nodes[5], &nodes[7]);
    Node* lca72 = tree->lca(&nodes[7], &nodes[2]);
    size_t dist61 = tree->getDist(&nodes[6], &nodes[1]);
    size_t dist73 = tree->getDist(&nodes[7], &nodes[3]);
    size_t dist57 = tree->getDist(&nodes[5], &nodes[7]);
    size_t dist72 = tree->getDist(&nodes[7], &nodes[2]);
    
    
    std::cout <<"lca(6,1): " << lca61->key << std::endl;
    std::cout <<"lca(7,3): " << lca73->key << std::endl;
    std::cout <<"lca(5,7): " << lca57->key << std::endl;
    std::cout <<"lca(7,2): " << lca72->key << std::endl;
    
    std::cout <<"dist(6,1): " << dist61 << std::endl;
    std::cout <<"dist(7,3): " << dist73 << std::endl;
    std::cout <<"dist(5,7): " << dist57 << std::endl;
    std::cout <<"dist(7,2): " << dist72 << std::endl;
    std::cout << "min_to_root_from_7: " << tree->getMinEdge(&nodes[7])->key << std::endl;
    tree->cut(&nodes[6], &nodes[4]);
    // tree->revert(&nodes[4]);
    // tree->supportRoot(&nodes[6]);
    //nodes[6].edgeWeight = 8;
    //nodes[6].treePtr->updateTreeSize(7nodes[6]);
    tree->link(&nodes[6], &nodes[3]);
    //
    
    //tree->setWeight(&nodes[6], 1);
    tree->removeWeightInPath(1, &nodes[5]);
    
    std::cout << "*********************" << std::endl;
    
    lca61 = tree->lca(&nodes[1], &nodes[6]);
    lca73 = tree->lca(&nodes[3], &nodes[7]);
    lca57 = tree->lca(&nodes[7], &nodes[5]);
    
    dist61 = tree->getDist(&nodes[1], &nodes[6]);
    dist73 = tree->getDist(&nodes[3], &nodes[7]);
    dist57 = tree->getDist(&nodes[7], &nodes[5]);
    dist72 = tree->getDist(&nodes[7], &nodes[2]);
    
    for(int i = 1;i < 8;++i) {
        //  if(i != 1) {
        std::cout << i << ": " << tree->depth(&nodes[i]) << std::endl;
        //  }
    }
    
    std::cout <<"lca(6,1): " << lca61->key << std::endl;
    std::cout <<"lca(7,3): " << lca73->key << std::endl;
    std::cout <<"lca(5,7): " << lca57->key << std::endl;
    std::cout <<"lca(7,2): " << lca72->key << std::endl;
    std::cout <<"dist(6,1): " << dist61 << std::endl;
    std::cout <<"dist(7,3): " << dist73 << std::endl;
    std::cout <<"dist(5,7): " << dist57 << std::endl;
    std::cout <<"dist(7,2): " << dist72 << std::endl;
    std::cout << "min_to_root_from_7: " << tree->getMinEdge(&nodes[7])->key << std::endl;
    std::cout << "min_to_root_from_5: " << tree->getMinEdge(&nodes[4])->key << std::endl;
}