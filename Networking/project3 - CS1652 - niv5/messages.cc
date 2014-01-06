#include "messages.h"

/////////////////////  NEW CODE  ////////////////////////
#if defined(GENERIC)
RoutingMessage::RoutingMessage()
{}

RoutingMessage::RoutingMessage(const RoutingMessage &rhs) {
    *this = rhs;
}

RoutingMessage & RoutingMessage::operator=(const RoutingMessage & rhs) {
    /* For now, change if you add data members to the struct */
    return *this;
}

ostream &RoutingMessage::Print(ostream &os)
{
	os << "Generic RoutingMessage()";
	return os;
}
#endif

#if defined(LINKSTATE)

#endif

#if defined(DISTANCEVECTOR)

// Default constructor
RoutingMessage::RoutingMessage() { }

// Constructor that takes in a distance vector map as the message and the source node that is sending it
RoutingMessage::RoutingMessage(map<int, double> p_dvMessage, int p_dvSource_id) {
  dvMessage = p_dvMessage;
  dvSource_id = p_dvSource_id;
}

// Constructor that takes in another routing message
RoutingMessage::RoutingMessage( const RoutingMessage &rhs ) {
	*this = rhs;
}

ostream &RoutingMessage::Print(ostream &os) {
	// create the iterator to iterate through the message
	map<int, double>::iterator dvMessage_it;
	int counter = 0;

	for ( dvMessage_it = dvMessage.begin(); dvMessage_it != dvMessage.end(); dvMessage_it++ )
		// Print out the node - latency pairs
		os << "Message " << counter << " from source " << dvSource_id << ": " << (*dvMessage_it).first << " => " << (*dvMessage_it).second << endl;
	return os;
}

RoutingMessage & RoutingMessage::operator=(const RoutingMessage & rhs) {
    /* For now.  Change if you add data members to the struct */
    return *this;
}
#endif

///////////////////  END NEW CODE  ////////////////////////
