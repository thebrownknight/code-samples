#ifndef _messages
#define _messages

#include <iostream>
#include "node.h"
#include "link.h"


///////////////  BEGIN MESSAGE HEADER  /////////////////////////

struct RoutingMessage {
    RoutingMessage();
    RoutingMessage( const RoutingMessage &rhs );
    RoutingMessage &operator=( const RoutingMessage &rhs );

    ostream & Print(ostream &os);

    // Anything else you need

    #if defined(LINKSTATE)
    #endif
    #if defined(DISTANCEVECTOR)
    int dvSource_id;	// the node ID of the source sending the message
	map<int, double> dvMessage;	// will hold the message as a map of node, latency pairs

	// Extra constructor
	RoutingMessage( map<int, double> p_dvMessage, int p_dvSource_id );

    #endif
};

inline ostream & operator<<(ostream &os, RoutingMessage & m) { return m.Print(os); }

///////////////// END MESSAGE HEADER //////////////////

#endif
