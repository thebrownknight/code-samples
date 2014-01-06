#include "node.h"
#include "context.h"
#include "error.h"
#include "linkstate.h"
#include "distancevector.h"

Node::Node(unsigned n, SimulationContext *c, double b, double l) : 
    number(n), context(c), bw(b), lat(l), routing_table() 
{}

Node::Node() {
    throw GeneralException(); 
}

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat),
  routing_table(rhs.routing_table) {}

Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(unsigned n) 
{ number=n;}

unsigned Node::GetNumber() const 
{ return number;}

void Node::SetLatency(double l)
{ lat=l;}

double Node::GetLatency() const 
{ return lat;}

void Node::SetBW(double b)
{ bw=b;}

double Node::GetBW() const 
{ return bw;}

Node::~Node()
{}

// These functions post events to the event queue in the event simulator so that the 
// corresponding node(s) can recieve the ROUTING_MESSAGE_ARRIVAL event(s) at the proper time(s)
void Node::SendToNeighbors(RoutingMessage* m)
{
    deque<Link*> links = *GetOutgoingLinks();
    deque<Node*> neighbors = *GetNeighbors();
    deque<Link*>::iterator it_l;
    deque<Node*>::iterator it_n;

    for (it_n = neighbors.begin(); it_n != neighbors.end(); it_n++) {
        // For now, find link to get the correct latency
        // In future, latencies can be gotten from routing table
        for (it_l = links.begin(); it_l != links.end(); it_l++) {
            if ((*it_n)->GetNumber() == (*it_l)->GetDest()) {
                context->PostEvent(
                    new Event(context->GetTime() + (*it_l)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, 
                        (void*)*it_n, (void*)m)
                );
                break;
            }
        }
    }
}

void Node::SendToNeighbor(Node* n, RoutingMessage* m)
{
    deque<Link*> links = *GetOutgoingLinks();
    deque<Link*>::iterator it;

    // For now, find link to get the correct latency
    // In future, latencies can be gotten from routing table
    for (it = links.begin(); it != links.end(); it++) {
        if (n->GetNumber() == (*it)->GetDest()) {
            context->PostEvent(
                new Event(context->GetTime() + (*it)->GetLatency(), ROUTING_MESSAGE_ARRIVAL, 
                    (void*)n, (void*)m)
            );
            break;
        }
    }
}

deque<Node*> *Node::GetNeighbors()
{
    return context->GetNeighbors(this);
}

deque<Link*> *Node::GetOutgoingLinks()
{
    return context->GetOutgoingLinks(this);
}

void Node::SetTimeOut(double timefromnow)
{
    context->TimeOut(this,timefromnow);
}

bool Node::Matches(Node* rhs)
{
    return number == rhs->number;
}

bool Node::Matches(unsigned rhs)
{
    return number == rhs;
}

/** Implementations of virtual functions for GENERIC use **/
void Node::LinkHasBeenUpdated(Link *l)
{
    cerr << *this << " got a link update: "<<*l<<endl;
}

void Node::ProcessIncomingRoutingMessage(RoutingMessage *m)
{
    cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
    cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(Node *destination)
{
    return NULL;
}

Table *Node::GetRoutingTable()
{
    return NULL;
}

/* ----------------------------------------------------- */

void Node::createRoutingTable(Node *currentNode, deque<Node*> *nodeList)
{
    // flag to determine if we found a neighbor in the nodeList or not
    bool neighbor_found = false;

    // Get the iterators for the node list
    deque<Node*>::iterator o_nodelist_iterator;
    deque<Node*>::iterator i_nodelist_iterator;

    // These will contain the data to insert into the routing tables
    map<int, map<int, double> > o_map;
    map<int, double> i_map;

    // Used as the return values for the insert methods into the map
    pair<map<int, double>::iterator,bool> i_pair;
    pair<map<int, map<int, double> >::iterator,bool> o_pair;

    // Get the neighbor list of the current node
    deque<Node*> *nList = GetNeighbors();
      
    // We initialize the current node ID and set it's latency cost to 0 and insert it into an inner map
    i_pair = i_map.insert( pair<int, double>((*currentNode).GetNumber(), 0) );

    // We go through the rest of the nodes in the list and add their IDs and latencies to a map before
    // adding it to the larger map
    for (i_nodelist_iterator = (*nodeList).begin(); i_nodelist_iterator != (*nodeList).end(); i_nodelist_iterator++) {
        // Loop through the neighbor list now
        for (o_nodelist_iterator = (*nList).begin(); o_nodelist_iterator != (*nList).end(); o_nodelist_iterator++) {
            // If the node IDs match up, we have found a neighbor in the node list
            if ((**i_nodelist_iterator).GetNumber() == (**o_nodelist_iterator).GetNumber())
            {
                // the node in the nodeList is a neighbor of this node,
                // so we set the neighbor_found flag to true and break out
                neighbor_found = true;
                break;
            }
        }

        // We found a neighbor node within the node list, we insert the node
        // in the map with the node's latency and set the neighbor_found flag back to false
        if (neighbor_found == true) {
            i_pair = i_map.insert( pair<int, double>((**i_nodelist_iterator).GetNumber(), (**i_nodelist_iterator).GetLatency()) );
            neighbor_found = false;
        } 
        // else the node is not a neighbor, and we insert the node into the map with an infinite latency cost
        else 
            i_pair = i_map.insert( pair<int, double>((**i_nodelist_iterator).GetNumber(), INF) );
    }

    // We insert the map we just created above (with the nodes and their latency costs) into the larger map
    // that will have the current node ID as the key and the full latency cost map as the value.  This will allow
    // each node to have full disclosure of it's distance vector and it's neighbors' distance vectors
    o_pair = o_map.insert( pair<int, map<int, double> >((*currentNode).GetNumber(), i_map) );

    // Loop through the neighbor list and do the same thing we did above
    for (o_nodelist_iterator = (*nList).begin(); o_nodelist_iterator != (*nList).end(); o_nodelist_iterator++)
    {
        // First clear the inner map to remove any previous adds
        i_map.clear();
        
        // Initially all latency costs are set to infinity since we don't know anything yet 
        i_pair = i_map.insert( pair<int, double>((*currentNode).GetNumber(), INF) );

        // Loop through the rest of the node list
        for (i_nodelist_iterator = (*nodeList).begin(); i_nodelist_iterator != (*nodeList).end(); i_nodelist_iterator++)
            i_pair = i_map.insert( pair<int, double>((**i_nodelist_iterator).GetNumber(), INF) );

        // and finally add this inner map to the current outer map
        o_pair = o_map.insert( pair<int, map<int, double> >((**o_nodelist_iterator).GetNumber(), i_map) );
    }

    // create the routing table given the mapping between each node and a map containing node - latency pairings
    routing_table = Table(o_map, (*currentNode).GetNumber());

    /*
     * Create the node's forwarding table (will just be pointing to itself for right now).
     */

    // Step 1: Loop through the node list
    for (o_nodelist_iterator = (*nodeList).begin(); o_nodelist_iterator != (*nodeList).end(); o_nodelist_iterator++)
    {
        // Step 2: If the node IDs match up, insert the node ID and the node into the forwarding table
        if ((**o_nodelist_iterator).GetNumber() == this->GetNumber())
            forwarding_table.insert( pair<int, Node*>(this->GetNumber(), this) );
        // Otherwise, there is no next hop so insert NULL for the node in the node list
        else
            forwarding_table.insert( pair<int, Node*>((**o_nodelist_iterator).GetNumber(), NULL) );
    }

    /////////////////////////////////////
    //        USED FOR DEBUGGING       //
    //(*currentNode).printRoutingTable();
    /////////////////////////////////////
}

// Utility print method for the node's routing table.
void Node::printRoutingTable()
{
  routing_table.printTable((*this).GetNumber());
}

// Utility print method for the node's forwarding table.
void Node::printForwardingTable()
{
      map<int, Node*>::iterator map_iterator;

      cout << "*********************************************************************" << endl;
      cout << "Node " << this->GetNumber() << " Forwarding Table: " << endl;
      cout << "Final Destination\tNext Hop" << endl;
      for (map_iterator = forwarding_table.begin(); map_iterator != forwarding_table.end(); map_iterator++)
      {
            cout << "        " << (*map_iterator).first << "\t=>      ";
            if ((*map_iterator).second == NULL)
              cout << "NULL";
            else
              cout << (*map_iterator).second->GetNumber();

            cout << endl;
      }
      cout << "*********************************************************************" << endl << endl;
}

ostream & Node::Print(ostream &os) const
{
    os << "Node(number=" << number << ", lat=" << lat << ", bw=" << bw << ")";
    return os;
}

Node* createNode(unsigned n, SimulationContext *c, double b, double l) {
#if defined(LINKSTATE)
    return new LinkState(n, c, b, l);
#elif defined(DISTANCEVECTOR)
    return new DistanceVector(n, c, b, l);
#else
    return new Node(n, c, b, l);
#endif
}

