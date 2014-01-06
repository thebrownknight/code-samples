#include <stdlib.h>
#include <stdio.h>
#include "context.h"

#include "node.h"
#include "link.h"

#include <iomanip>

int main(int argc, char **argv)
{
  bool singlestep;
  string topofile, eventfile;

  if (argc<3 || argc>4 ) {
    cerr <<"routesim topologyfile eventfile [singlestep]"<<endl;
    exit(-1);
  }
  singlestep=(argc==4);
  topofile=argv[1];
  eventfile=argv[2];

  SimulationContext c;

  // Get the loaded nodes
  deque<Node*> *nodeList = c.GetNodeList();
  // Iterator for node list
  deque<Node*>::iterator nodelist_iterator;
  
  c.LoadTopology(topofile);
  
  cerr << c <<endl;

  c.LoadEvents(eventfile);

  cerr << c << endl;

  c.Init();

  for (nodelist_iterator = (*nodeList).begin(); nodelist_iterator != (*nodeList).end(); nodelist_iterator++)
    (**nodelist_iterator).createRoutingTable(*nodelist_iterator, nodeList);

  cerr << c << endl;
  
  cerr << setprecision(20);

  Event *e;
  while ((e=c.GetEarliestEvent())) {
    printf("Printing event....\n");
    cerr << *e << endl;
    if (singlestep) {
      cerr << "=============================================================\n"
	   << "======Dispatching: "<<*e << endl;
    }
    c.DispatchEvent(e);
    if (singlestep) { 
      char buf[1024];
      cerr << "======Done. Hit Enter to continue"; 
      fflush(stdin);
      fgets(buf,1024,stdin);

    }
  }
}
  
  
