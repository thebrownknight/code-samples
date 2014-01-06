#include "distancevector.h"
#include "context.h"

/////////////// BEGIN DISTANCE VECTOR IMPLEMENTATION ////////////////////

DistanceVector::DistanceVector(unsigned n, SimulationContext* c, double b, double l) : Node(n, c, b, l)
{}

DistanceVector::DistanceVector(const DistanceVector & rhs) : Node(rhs)
{
    *this = rhs;
}

DistanceVector & DistanceVector::operator=(const DistanceVector & rhs) {
    Node::operator=(rhs);
    return *this;
}

DistanceVector::~DistanceVector() { }

// Two things to do in this method:
// 1) Update the routing table for the current local node
// 2) Send the updated distance vector to its neighbors
void DistanceVector::LinkHasBeenUpdated(Link *incoming_link)
{
    int map_changed = 0;
    cerr << *this << ": Link Update: " << *incoming_link << endl;
    // Step 1: Update routing table for current local node
    this->routing_table.updateTable(incoming_link->GetSrc(), incoming_link->GetDest(), incoming_link->GetLatency());

    // 1) Check to see if the map has changed 
    // 2) Create the routing message - the updated distance vector of the current local node
    // 3) Send this message out to its neighbors
    if ( (map_changed = updateDistanceVector()) == 1 ) 
      SendToNeighbors(getDistanceVector());
}

// This method is called when a new routing message comes
// We do the same thing as above: update routing table with the message
// details and then send out the updated distance vector to neighbors
void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *route_mes)
{
    cerr << *this << " got a routing message: " << *route_mes << endl;
    cout << "Node " << this->GetNumber() << " just received an update from " << route_mes->dvSource_id << ":" << endl;
    int dv_changed = 0;
    // Replace the local distance vector with the received distance vector in the routing message
    // The incoming message will include the updated distance vector from a particular source node
    // The recipients should update their distance vectors accordingly
    updateRoutingTable(route_mes);

    // Check to see if the local distance vector has changed
    // If it has, then send out the updated distance vector to its neighbors
    if ( (dv_changed = updateDistanceVector()) == 1 )
        SendToNeighbors(getDistanceVector());
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node *DistanceVector::GetNextHop(Node *dest)
{
  // Obtain the iterator from the forwarding table for the queried node
  map<int, Node*>::iterator map_iterator = forwarding_table.find(dest->GetNumber());

  if (map_iterator != this->forwarding_table.end())  // get the next hop node from the local node
    return (*map_iterator).second;  // return the Node 
  else  // we don't have this particular entry in the forwarding table
    return NULL;
}

Table *DistanceVector::GetRoutingTable()
{
    return &this->routing_table;
}

// Method that updates the local node's distance vector
int DistanceVector::updateDistanceVector()
{
  // Variables
  //
  int table_changed = 0, counter = 0;  // to check if the routing table has changed after recalculation
  double current_latency_cost = 0, actual_latency_cost = 0, cost_to_neighbor = 0, neighbor_dest_cost = 0; 
  int lNodeID = this->GetNumber(); // store the ID of the current node being processed

  double *cost_ar;
  
  // Get the neighbors and the neighbor list iterator
  deque<Node*> *nList = GetNeighbors();
  deque<Node*>::iterator nList_iterator;

  // Get the distance vector and the distance vector iterator
  map<int, map<int, double> >::iterator distance_vector = routing_table.get(lNodeID);
  map<int, double>::iterator distance_vector_iterator;

  // First we loop through the distance vector for the local node
  for (distance_vector_iterator = (*distance_vector).second.begin(); distance_vector_iterator != (*distance_vector).second.end(); distance_vector_iterator++)
  {
      // reset the counter for each node in the distance vector   
      counter = 0;
      // Allocate memory for the array of doubles holding the latency costs between
      // the current node and each of its neighbors
      cost_ar = (double*)malloc( sizeof(double) * (*nList).size() + 1 );
      // this map holds latency costs for particular nodes
      map<double, Node*> node_cost;

      // Don't perform any calculations if the distance vector key is simply the local node's ID
      if ((*distance_vector_iterator).first == lNodeID) { }
      else {
        // Retrieve the latency cost between the local node and the current node in the distance vector being processed
        current_latency_cost = routing_table.getCost(lNodeID, (*distance_vector_iterator).first);

        // Loop through all of the neighbors to get latency costs
        for (nList_iterator = (*nList).begin(); nList_iterator != (*nList).end(); nList_iterator++)
        {
            // Store the cost from local node to current neighbor
            cost_to_neighbor = getCost(lNodeID, (**nList_iterator).GetNumber());
            cout << "Temp cost from " << lNodeID << " to " << (**nList_iterator).GetNumber() << " = " << cost_to_neighbor << endl;

            // Exit early if there is no latency cost associated with the neighbor
            if (cost_to_neighbor == -1) 
              return 0;

            // Retrieve the latency cost between the neighbor and the destination
            neighbor_dest_cost = routing_table.getCost((**nList_iterator).GetNumber(), (*distance_vector_iterator).first);
            cout << "Distance from neighbor " << (**nList_iterator).GetNumber() << " to " << (*distance_vector_iterator).first << " = " << neighbor_dest_cost << endl;

            // If the cost between a neighbor and destination is unknown,
            // simply initialize it to 0 to avoid errors
            if (neighbor_dest_cost == INF) 
              neighbor_dest_cost = 0;

            // The total cost between local node to destination is the cost
            // between the node and it's neighbor + the cost between the
            // neighbor and the destination, update the cost array
            cost_ar[counter] = cost_to_neighbor + neighbor_dest_cost;

            // We need to know the latency cost - Node pairs so we can later 
            // update forwarding tables
            node_cost.insert( pair<double, Node*>(cost_ar[counter], *nList_iterator) );

            counter++;  //increment counter for each neighbor
        }
        // Calculate the actual latency cost by finding the minimum element within the array of neighbor latency costs
        actual_latency_cost = *min_element(cost_ar, cost_ar+(*nList).size());

        // Check to see if the latency costs have changed from before
        if (actual_latency_cost != current_latency_cost)
        {
          routing_table.updateTable(lNodeID, (*distance_vector_iterator).first, actual_latency_cost);
          table_changed = 1;  // the routing table has changed!
        }

        // Get the node from the map we created above using the latency cost we calculated
        // Either the latency cost was updated or remains the same as before
        map<double, Node*>::iterator cost_iterator = node_cost.find(actual_latency_cost);

        // update the forwarding table for the currently processed node
        if (cost_iterator != node_cost.end())
          updateForwardingTable((*distance_vector_iterator).first, (*cost_iterator).second);

        // Clean up
        delete cost_ar;
      }
  }
  // DEBUG
  // this->printForwardingTable();
  return table_changed;
}

// Method that retrieves the latency between a source and destination nodes
double DistanceVector::getCost(int sourceID, int destinationID)
{
    // The latency cost that we are returning
    double latency = -1;
    // Store the neighboring links from the currently processed node
    deque<Link*> *nLinks = this->context->GetOutgoingLinks(this);
    // The iterator for the returned deque
    deque<Link*>::iterator link_iterator;

    // Loop through all of the links
    for (link_iterator = (*nLinks).begin(); link_iterator != (*nLinks).end(); link_iterator++)
    {
        // If the current link matches the source and destination IDs passed in, then return the latency
        if ( (**link_iterator).Matches(sourceID, destinationID) )
        {
            latency = (**link_iterator).GetLatency();
            break;
        }
    }

    // If the latency cost could not be found in the neighboring links, get the latency
    // cost from the routing table
    if (latency == -1)
        latency = routing_table.getCost(sourceID, destinationID);

    return latency;
}

void DistanceVector::updateForwardingTable(int nodeID, Node *nextHopNode)
{
  map<int, Node*>::iterator ft_iterator = forwarding_table.find(nodeID);

  if (ft_iterator != forwarding_table.end())
    (*ft_iterator).second = nextHopNode;  // set the passed in node as the new 'next hop node' in the forwarding table
}

void DistanceVector::updateRoutingTable(RoutingMessage *route_mes)
{
    // Extract the node ID from the routing message
    int in_nodeID = route_mes->dvSource_id;

    // Search the routing table for the node ID in the message
    map<int, map<int, double> >::iterator map_iterator = routing_table.get(in_nodeID);

    // If the iterator has reached the end of the table, we don't have any matches
    if (map_iterator == routing_table.getEnd())
      cout << "There is no entry in the routing table for " << in_nodeID << "." << endl;
    // Otherwise, we get the message portion in the routing message, get the position of the iterator,
    // and swap the current map out for the one we pass in
    else
      routing_table.swapMap(map_iterator, route_mes->dvMessage);
}

RoutingMessage *DistanceVector::getDistanceVector()
{
    // Format: RoutingMessage( map<int, double> p_dvMessage, int p_dvSource_id )
    return new RoutingMessage(this->routing_table.getDistanceValueVector((*this).GetNumber()), (*this).GetNumber());
}

ostream & DistanceVector::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}

/////////////////// END DISTANCE VECTOR IMPLEMENTATION //////////////////////