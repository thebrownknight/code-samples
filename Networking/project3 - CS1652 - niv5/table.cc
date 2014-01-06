#include "table.h"

///////////////////   BEGIN TABLE IMPLEMENTATION   /////////////////////////
#if defined(GENERIC)
Table::Table() { }

Table::Table(const Table & rhs) {
    *this = rhs;
}

Table & Table::operator=(const Table & rhs) {
    // For now, change if you add more data members to the class 
    costtable = rhs.costtable;
    node_id = rhs.node_id;

    return *this;
}

ostream & Table::Print(ostream &os)
{
  os << "Generic Table()";
  return os;
}
#endif


#if defined(LINKSTATE)

#endif

///////////////////////////
// Distance Vector Methods
///////////////////////////

#if defined(DISTANCEVECTOR)

// Constructors

Table::Table() {}

Table::Table(const Table & rhs) {
    *this = rhs;
}

Table::Table(map<int, map<int, double> > inputMap, int nodeID)
{
  // Initialize the private members
  costtable = inputMap;
  node_id = nodeID;
}

Table & Table::operator=(const Table & rhs) {
     // For now, change if you add more data members to the class 
    costtable = rhs.costtable;
    node_id = rhs.node_id;
    return *this;
}

ostream & Table::Print(ostream &os)
{
    // Map iterators and maps
    map<int, map<int, double> >::iterator map_iterator;
    map<int, double> s_map;
    map<int, double>::iterator s_map_it;

    os << "**********************************************************************" << endl;
    os << "The complete distance vector table:";
    for (map_iterator = costtable.begin(); map_iterator != costtable.end(); map_iterator++)
    {
      os << "\tNode ID: " << (*map_iterator).first << endl;
      // Get the neighbor node - latency map
      s_map = (*map_iterator).second;
      // Loop through this map and output the neighbor ID and latency pairs
      for (s_map_it = s_map.begin(); s_map_it != s_map.end(); s_map_it++)
        os << "\tNeighbor ID: " << (*s_map_it).first << " with latency: " << (*s_map_it).second << endl;
    }
    os << "**********************************************************************" << endl << endl;
    return os;
}

// Utility method that takes a node ID and prints the routing table for it
void Table::printTable(int nodeID)
{
    // Map iterators and maps
    map<int, map<int, double> >::iterator map_iterator;
    map<int, double> s_map;
    map<int, double>::iterator s_map_it;

    cout << "**********************************************************************" << endl;
    cout << "Routing table for Node " << nodeID << ": " << endl;
    for (map_iterator = costtable.begin(); map_iterator != costtable.end(); map_iterator++)
    {
      cout << "\tNode ID: " << (*map_iterator).first << endl;
      // Get the neighbor node - latency map
      s_map = (*map_iterator).second;
      // Loop through this map and output the neighbor ID and latency pairs
      for (s_map_it = s_map.begin(); s_map_it != s_map.end(); s_map_it++)
        cout << "\tNeighbor ID: " << (*s_map_it).first << " with latency: " << (*s_map_it).second << endl;
    }
    cout << "**********************************************************************" << endl << endl;
}

// Common iterator retrieval functions - the costtable is a private variable, so only
// way to access it's methods are through exposed functions such as get(sourceID) and getEnd()
map<int, map<int, double> >::iterator Table::get(int sourceID) 
{
  return costtable.find(sourceID);
}

map<int, map<int, double> >::iterator Table::getEnd() 
{
  return costtable.end();
}

// Get the link costs between the source and destination nodes
// Return -1 as default if there are no matches
double Table::getCost(int sourceID, int destinationID)
{
  map<int, double> destinationCosts;

  if ( costtable.find(sourceID) != costtable.end() ) {
    destinationCosts = costtable[sourceID];
    if ( destinationCosts.find(sourceID) != destinationCosts.end() )
      return destinationCosts[destinationID];
    else
      return -1;
  }
  else
    return -1;
}

map<int, double> Table::getDistanceValueVector(int sourceID)
{
  // Check to make sure that the routing table contains the sourceID 
  if (costtable.find(sourceID) != costtable.end())
    return costtable[sourceID];

  // If we don't find anything, return the end of the map
  costtable.end();
}

/*
 * Description: this should swap the current value in the map with the copy of the local parameter, and then once
 *  the function exits the local parameter (now holding the old value that was in the map) is discarded
 */
void Table::swapMap(map<int, map<int, double> >::iterator mapIterator, map<int, double> nMap)
{
  // The map iterator already specifies the location in the map so simply swap out the values.
  (*mapIterator).second.swap(nMap);
}

/*
 * Description: this is the main method that handles updating the routing table for the nodes.
 * It takes in a source node ID, destination node ID, and the new cost.
 */
void Table::updateTable(int sourceID, int destinationID, double in_cost)
{
  // Get the iterator returned after calling find on the cost table
  map<int, map<int, double> >::iterator map_iterator = costtable.find(sourceID);
  // This is the inner map of the cost table
  map<int, double>::iterator second_map_iterator;
  
  // First we obtain the inner map, and set the value to the new cost passed in
  if (map_iterator != costtable.end()) {
    second_map_iterator = (*map_iterator).second.find(destinationID);
    if (second_map_iterator != (*map_iterator).second.end())
      (*second_map_iterator).second = in_cost;
  }

}

#endif
///////////////////   END TABLE IMPLEMENTATION  ///////////////////////