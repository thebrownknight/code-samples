#ifndef _node
#define _node

#include <new>
#include <iostream>
#include <deque>
#include <cfloat>
#include <algorithm>

class RoutingMessage;
class Table;
class Link;
class SimulationContext;

#include "table.h"
#include "messages.h"

// Used to set latency costs that haven't been specified yet
const float INF = DBL_MAX - 20000;

using namespace std;

class Node {
    protected:
        unsigned number;
        SimulationContext    *context;
        double   bw;
        double   lat;
        Table routing_table;
        map<int, Node*> forwarding_table;

    public:
        Node(unsigned n, SimulationContext *c, double b, double l);
        Node();
        Node(const Node &rhs);
        Node & operator=(const Node &rhs);
        virtual ~Node();

        bool Matches(Node* rhs);
        bool Matches(unsigned);

        void SetNumber(unsigned n);
        unsigned GetNumber() const;

        void SetLatency(double l);
        double GetLatency() const;
        void SetBW(double b);
        double GetBW() const;

        void SendToNeighbors(RoutingMessage* m);
        void SendToNeighbor(Node* n, RoutingMessage* m);
        deque<Node*> *GetNeighbors();
        deque<Link*> *GetOutgoingLinks();
        void SetTimeOut(double timefromnow);

        virtual void LinkHasBeenUpdated(Link *l);
        virtual void ProcessIncomingRoutingMessage(RoutingMessage *m);
        virtual void TimeOut();
        virtual Node *GetNextHop(Node *destination);
        virtual Table *GetRoutingTable();
        virtual ostream & Print(ostream &os) const;

        // New functions added
        // Initial functions called to populate the forwarding and routing tables
        void createRoutingTable(Node *curNode, deque<Node*> *nodeList);

        // Utility print methods
        void printRoutingTable();
        void printForwardingTable();

};

Node* createNode(unsigned, SimulationContext*, double, double);
inline ostream & operator<<(ostream &os, const Node & n) { 
    return n.Print(os);
}


#endif
