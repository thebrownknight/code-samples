#ifndef _table
#define _table

#include <iostream>
#include <map>
#include <deque>

#include "link.h"

using namespace std;

////////////////  BEGIN TABLE DEFINITION //////////////////////////
// // Students should write this class
class Table {
    private:
        int node_id;
        map< int, map < int, double > > costtable;
    public:
        Table();
        Table(const Table &);
        Table & operator=(const Table &);

        ostream & Print(ostream &os);

        // Anything else you need

        #if defined(LINKSTATE)
        #endif

        #if defined(DISTANCEVECTOR)
        Table(map<int, map<int, double> > inputMap, int nodeID);
        
        // Utility methods to get commonly used iterators on the table
        // We encapsulate the actual map but expose certain methods
        map< int, map<int, double> >::iterator getEnd();
        map< int, map<int, double> >::iterator get(int sourceID);

        // Used to retrieve the latency between the source and destination
        double getCost(int sourceID, int destinationID);
        // Used to retrieve the distance vector for the given source node
        map<int, double> getDistanceValueVector(int sourceID);

        void updateTable(int sourceID, int destinationID, double cost);
        // swaps in the new map for the value that the iterator passed in is pointing to
        void swapMap(map<int, map<int, double> >::iterator mapIterator, map<int, double> newMap);
        // utility method to print out the routing table at the given node
        void printTable(int nodeID);

        #endif
};

inline ostream & operator<<(ostream &os, Table & t) { return t.Print(os);}
////////////////////  END TABLE DEFINITION    /////////////////////////////////

#endif
