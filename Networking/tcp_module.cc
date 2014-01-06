/* ======================================================================== */
/* ======================================================================== */
/* Program:             tcp_module.cc                                       */
/* Description:         Implement the TCP module.                           */
/* Authors:             Nikhil Venkatesh <niv5@pitt.edu>                    */
/*                                                                          */
/* Date:                10/1/2013                                           */
/* ======================================================================== */
/* ======================================================================== */
 
/* ======================================================================== */
/*                               INCLUDES                                   */
/* ======================================================================== */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
 
// Networking header includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
 
// C++ includes
#include <fstream>      // for file i/o
#include <iostream>
#include <list>
#include <deque>
#include <vector>
#include <string>
 
#include <sstream> // for error logger
#include <sys/time.h> // for error logger
 
// Minet includes
#include "Minet.h"
#include "tcpstate.h"
#include "buffer.h"
#include "constate.h"
#include "ip.h"
#include "packet.h"
#include "sockint.h"
#include "tcp.h"
 
#define ACK_TIMEOUT 3
#define NO_OF_RETRIES 0
 
// namespace
using namespace std;
 
 
/* ======================================================================== */
/*                           MODULE CONSTANTS/SETTINGS                      */
/* ======================================================================== */
 
// TCP states
// ----------
static const int F_SYN = 1;
static const int F_ACK = 2;
static const int F_SYNACK = 3;
static const int F_PSHACK = 4;
static const int F_FIN = 5;
static const int F_FINACK = 6;
static const int F_RST = 7;
static bool bFirstSend = true;
static bool flagxxx = false;
 
// Error Logging/journaling
// ------------------------
FILE *logFile;                          // output file for error log
bool LOG_TO_FILE = true;                // TRUE or FALSE to send journal entries to the log file
 
stringstream ERR;                       // stream used for streaming errors to the ERRLOG function
bool need_pre_endl = false;             // set to TRUE if an ENDL needs printed with next journal entry
static const bool LOG_TO_MONITOR_WINDOW = true;     // TRUE/FALSE = send journal entries to monitor
static const bool LOG_TO_TCP_MODULE_WINDOW = true;  // TRUE/FALSE = send journal entries to tcp_module window
static const int LOG_LVL = 3;           // Journal level to use (0 to 5) for how much to journal or log
                                        // 0 = send nothing to log (up to) 5=send everything to log
static const int REMOTE_SECONDS_OFFSET = -(60 * 8 + 50);  // How many seconds to add (or subtract) to the timestamp
                                        // to better match the remote server time
static const int REMOTE_MICROSECONDS_OFFSET = 601200; // How many microseconds (0 to 999999) to add (or subtract)
                                        // to the timestamp to better match the remote server time
                                        // i.e. if the remote computer time is 145.123456 seconds AHEAD of this one,
                                        // then set  REMOTE_SECONDS_OFFSET = 145, and set REMOTE_MICROSECONDS_OFFSET = 123456
                                        // if it is BEHIND then these values become negative
                                        // TEST: time netlab-1=13:52:16.247195,   time netlab-2=13:43:23.105201
                                        //     diff between two: (60*8 + 53) seconds, and 141994ms
                                        // USE: for netlab-1(tcp_module) & netlab-2(running nc & wireshark)
                                        //   -(60 * 8 + 50)  & 601200
// >>>Future (if needed) >>>>
// enum ElogType {JALL, JMU, JSOCK, JTIMEOUT, JMISC); // Journal types, by tcp_module function
// static const int  LOG_LVL_MUX = 1;   // 1 through 5 journal level for for MUX
// static const int  LOG_LVL_SOCK = 1;  // 1 through 5 journal level for SOCKETS
// static const int  LOG_LVL_TIMEOUTS = 1;  // 1 to 5 journal level for TIMEOUTS
// static const int  LOG_LVL_MISC = 1; // 1 to 5 journal level for MISCELLANOUS
 
 
 
/* ======================================================================== */
/*                                PROTOTYPES                                */
/* ======================================================================== */
// int xyz; // DEMO 1
// Core TCP module functions
// -------------------------
// The main functionality of this is split into 3 parts:
// 1) Handling the IP multiplexor (reponsible for merging and distributing the data)
// 2) Handling the connections to the socket module from above
// 3) Handling the timeout caused for lost/dropped packets and ACKS
void handle_mux(    const MinetHandle &mux, const MinetHandle &sock, ConnectionList<TCPState> &clist);
void handle_sock(   const MinetHandle &mux, const MinetHandle &sock, ConnectionList<TCPState> &clist);
void handle_timeout(const MinetHandle &mux, const MinetHandle &sock, ConnectionList<TCPState> &clist);
void timeoutHandler( const MinetHandle &mux, MinetEvent event, ConnectionList<TCPState>::iterator cs, ConnectionList<TCPState> &clist);

 
 
// The following are for generating the timestamped error entries, and the journal output file
// -------------------------------------------------------------------------------------------
void wiresharkTimestamp();
void ERRLOG  (const std::ostream &jEnt);
void ERRLOG  (const int lgLvl, const std::ostream &jEnt);
void ERRLOG  (const int lgLvl, const std::ostream &jEnt, const bool useEndl);
void ERRLOGnt(const int lgLvl, const std::ostream &jEnt, const bool useEndl);
void _ERRLOG (const int lgLvl, const std::ostream &jEnt, const bool useEndl, const bool dumpTime);
void openLogFile();
void closeLogFile();
void hex_dump_row(char * mem_start_ptr,  char * mem_end_ptr, unsigned int row_offset,
        unsigned int fst_byte, unsigned int col_overflow);
void hexdump(char * mem_start_ptr, char * mem_end_ptr);
void hexdump_by_size(char * mem_to_dump_ptr, unsigned int mem_size);
 
 
// misc
// ----
//void createIPPacket(Packet &pack, ConnectionToStateMapping<TCPState> &a_mapping, int size_of_data,
//                    int header, bool bCustomizedSeq = false, int my_seq = 0);
void createIPPacket(Packet &packet, ConnectionToStateMapping<TCPState> &a_mapping,
                     unsigned int size_of_data, unsigned int header,
                     unsigned int seqNumber, unsigned int ackNumber);
 
 
/* ======================================================================== */
/* ======================================================================== */
/* ==                        **** MAIN ****                              == */
/* ======================================================================== */
/* ======================================================================== */
// FUNCTION:
// PARAMS:
//      const MinetHandle &mux = the connection through which the data will be handled
//      const MinetHandle &sock = the socket connection to which to write and read from
//      ConnectionList<TCPState> &clist = the C++ deque that handles all the connections
// DESCRIPTION:
//  Function will handle receiving the TCP segment, retrieving the header info,
//  and going through the various states of the finite state machine.
/* ======================================================================== */
int main(int argc, char * argv[])
{
  // prep the journaling system
  // --------------------------
  openLogFile();
  ERRLOG(5, ERR << "LOGFILE STARTUP!");
 
  // xyz = 0; // DEMO 1
  MinetHandle mux;
  MinetHandle sock;
  ConnectionList <TCPState> clist;
  MinetEvent event;
  double timeout = 1.0; // initial timeout to use
 
  // prep and init MINET
  // -------------------
  MinetInit (MINET_TCP_MODULE);
  mux =  MinetIsModuleInConfig(MINET_IP_MUX)      ? MinetConnect(MINET_IP_MUX)     : MINET_NOHANDLE;
  sock = MinetIsModuleInConfig(MINET_SOCK_MODULE) ? MinetAccept(MINET_SOCK_MODULE) : MINET_NOHANDLE;
 
  if ((mux == MINET_NOHANDLE) && (MinetIsModuleInConfig(MINET_IP_MUX))) {
    ERRLOG(5, ERR << "TCP_MODULE is unable to connect to ip_mux.  GOODBYE");
    return -1;
  }
 
  if ((sock == MINET_NOHANDLE) && (MinetIsModuleInConfig(MINET_SOCK_MODULE))) {
    ERRLOG(5, ERR << "TCP_MODULE cannot accept from sock_module.  GOODBYE");
    return -1;
  }
  clist.clear();
 
 
  // NIKHIL'S IP: 192.168.108.160
 
  // DEMO IP INFO:
  // -------------
  // netlab-1 computer:     ip address on eth1 = 192.168.42.2
  // netlab-2 computer:     ip address on eth1 = 192.168.42.3
  // netlab-1 minet stack:  ip address on eth1 (via minet.cfg file): 192.168.108.160
  // ifconfig
  // ip adr show eth1
  // wireshark settings:
  //      sudo wireshark -p -f "net 192.168.108.160"
  //      ...then, if desired, type "tcp" in the filter window, THOUGH NOT OFTEN NECESSARY when starting like this
 
 
  // -------------------------
  // This is [[PASSIVE OPEN]] (DEMO 1a)
  // -------------------------
  // THIS RECIEVES A COONECTION REQUEST FROM A REMOTE COMPUTER
  // 0) Make sure minet.cfg has its ip set to 192.168.108.160
  // 1) On minet computer:   ./start_minet "tcp_server u 8765"        // local
  // 2) On remote computer:  ping  192.168.108.160       // remote (do this to bypass minet bug p7 of project docs)
  // 3) on remote computer:  nc 192.168.108.160 8765 -p 8765        // remote
  // (observe SYN, SYN+ACK, ACK in wireshark.. observe moving to ESTABLISHED state in journal.
  // - - - - - - - - - - - - - - - - - - - - - - - - -  - - - -
  // *********** CURRENTLY DISABLED ************** //
  if (1==0) // enable/disable
  { // This is a hardcoded connection for testing ACTIVE CONNECTION
    ERRLOG(5, ERR << "TEST H-CODED ACTIVE CONN. FOR incoming connect request>>>" << endl);
 
    // Create and store a HARD CODE connection
    Connection con;
    con.dest = "192.168.42.3";
    con.src = "192.168.108.160";
    con.destport = 8765;
    con.srcport = 8765;
    con.protocol = IP_PROTO_TCP;
    ConnectionToStateMapping<TCPState> am;
    am.connection = con;
 
    TCPState atcpstate = TCPState( 987 , LISTEN , 3);  // (init seq#, state, timertries)
    am.state = atcpstate;
    //am.state.SetState(LISTEN);
 
    ERRLOG(5, ERR << ">>>>>ConnectionToStateMapping information>>>" << endl);
    am.Print(ERR);
    ERRLOG(5, ERR);
    ERRLOG(5, ERR << "\n");
    clist.push_front(am); // save the connection for the future incoming request
    ERRLOG(5, ERR << "state = " << am.state.GetState());
  } // 1==0
 
 
  // -------------------------
  // This is [[ACTIVE OPEN]] (DEMO 1b)
  // -------------------------
  // ---------------------------------
  // THIS INITIATES A COONECTION REQUEST TO A REMOTE COMPUTER
  // 0) Make sure minet.cfg has its ip set to 192.168.108.160
  // 1) on remote computer:  nc -l 8765        (to listen on port#8765 - MUST DO THIS FIRST!)
  // 3) On minet computer:   ./start_minet "tcp_server u 8765"        // local
  // (observe SYN, SYN+ACK, ACK in wireshark.. observe moving to ESTABLISHED state in journal.)
  //  NOTE#2: you do NOT have to do a ping.  This is handled with the two MinetSend(mux, psend)'s below.
  // (PLUS THE 2 SECOND DELAY!!!! THIS DELAY IS NECESSARY!) (to bypass the minet bug , p7 of project doc)
  // - - - - - - - - - - - - - - - - - - - - - - - - -  - - -
  // *********** CURRENTLY DISABLED ************** //
  if (1==0) // enable/disable 
  {
    flagxxx = true;
    // This is a hardcoded connection for testing an ACTIVE CONNECTION
    // Implementation: This computer running minet INITIATES a connection to a remote computer via SYN
    // remote computer responds via SYN ACK,  this computer responds with ACK
    ERRLOG(5, ERR << "[[MAIN!]]TEST H-CODED ACTIVE CONN. FOR outgoing CONNECT REQUEST>>>" << endl);
 
    //unsigned int sendSeqNum;
    //unsigned int rcvdAckNum;
 
    // create the hard-coded connection and store into the connection list
    // - - - - - - - -
    Connection con;
    con.src = "192.168.108.160";
    con.dest = "192.168.42.3";
    con.destport = 8765;
    con.srcport = 8765;
    con.protocol = IP_PROTO_TCP;
    ConnectionToStateMapping<TCPState> am;
    am.connection = con;
    // TCPState atcpstate = TCPState( rand() , SYN_SENT , 3); << future?
    TCPState atcpstate = TCPState( 54321 , SYN_SENT , 3);  // (init seq#, state, timertries)
    // <<<nikhil: NOT that we ever have an "official" closed state, i am only being pragmatic...
    am.state = atcpstate;
    am.state.SetSendRwnd(536); // set OUR window size
    ERRLOG(5, ERR << "[[MAIN!]]>>>>>ConnectionToStateMapping information>>>" << endl);
    am.Print(ERR);
    ERRLOG(5, ERR);
    //ERRLOG(5, ERR << "\n");
    clist.push_front(am); // write to the connection list
 
    // build the packet
    // - - - - - - - - -
    Packet psend;
    am.state.SetLastRecvd(0);
    // am.state.SetLastSent(1789);
    am.state.SetLastSent(rand() % 10000); // try a random number from 1 to 10000 this time
 
    // cs->state.SetLastAcked(cs->state.last_sent - 1);   <<<<<< Nikhil, what was this for????
 
    // i.e. createIPPacket(packetToSend, am, dataSize, F_SYN, seqNumToUse, ackNumToUse);
    // createIPPacket(psend, am, 0, F_SYN, am.state.GetLastSent(), 0); // build the packet
 
    //am.state.SetSendRwnd(536); // set OUR window size
    createIPPacket(psend, am, 0, F_SYN, am.state.GetLastSent(), 0); // build the packet
    ERRLOG(5, ERR << "[[MAIN!]]>>>>>Packet information>>>" << endl);
    psend.Print(ERR);
    ERRLOG(5, ERR);
    MinetSend(mux, psend);
    sleep(2);  // << need this!
    MinetSend(mux, psend); // <<<two here?  YES two here as we are NOT doing ping with this!!!
    ERRLOG(5, ERR << "[[MAIN!]]DONE WITH CONNECTION" << endl);
    ERRLOG(5, ERR << "[[MAIN!]]***changing state to SYN_SENT***");
    //am.state.SetState(SYN_SENT);   // set the current state to SYN_SENT (our OFFICIAL state now!)
  } // disable for demo#1
 
 
  // MAIN event loop
  // ===============
  ERRLOG(5, ERR << "STARTING TCP_MODULE...\n");
  //while ( MinetGetNextEvent(event, timeout) == 9999) { // NEEDED FOR DEMO 1
  while ( MinetGetNextEvent(event, timeout) == 0) {
 
    // MINET EVENTS
    // ------------
    if ((event.eventtype == MinetEvent::Dataflow) && (event.direction == MinetEvent::IN)) {
 
      // MUX events
      // ----------
      if (event.handle == mux) {
        ERRLOG(5, ERR << "\n[M]", false);
 
        // ip packet has arrived!
        // ..........................
        //if(xyz == 0) // NEEDED FOR DEMO 1
        handle_mux(mux, sock, clist);
        // ..........................
 
        // >>>>debug
        //Packet packet;
        //MinetReceive(mux, packet);
        // <<<<<<<<<<<<<<<<<<<
 
 
      } // (event.handle == mux)
 
      // Socket Events
      // -------------
      if (event.handle == sock) {
        ERRLOG(5, ERR << "\n[S]", false);
 
        // socket request or response has arrived
        // ..........................
        //if (xyz == 0) // NEEDED FOR DEMO 1
        handle_sock(mux, sock, clist);   // STILL HAVE TO FLESH OUT
        // ..........................
 
      } // (event.handle == sock)
 
      cout << endl;
    } // if we had MINET event
 
    // TIMEOUT EVENTS
    // --------------
    if (event.eventtype == MinetEvent::Timeout) {
 
      // timeout ! probably need to resend some packets
      ERRLOGnt(5, ERR << "\n[T]", false);
      // ..............................
      handle_timeout(mux, sock, clist);     //<<<<<THIS WAS COMMENTED OUT FOR DEMO#1
      // ..............................
      // >>>> remove after timeouts implemented
      //timeout = 120.0;  /// <<< EVENTUALLY will have to change this to timeout on the NEXT
      //timeout = -1.0;  /// <<< EVENTUALLY will have to change this to timeout on the NEXT
      // <<<
      // We need to resend packets at this point - use GBN method
    } // if we had a timeout events
  } // while()  MAIN event loop
 
 
  // deinitialize and shutdown Minet
  // -------------------------------
  MinetDeinit();
  closeLogFile(); // shut down the journaling system
 
  return 0;
} // int main()
 
 
 
/* ======================================================================== */
/* ======================================================================== */
/*                           PRIMARY  FUNCTIONS                             */
/* ======================================================================== */
/* ======================================================================== */
 
/* ======================================================================== */
// FUNCTION: handle_mux
// PARAMS:
//      const MinetHandle &mux = the connection through which the data will be handled
//      const MinetHandle &sock = the socket connection to which to write and read from
//      ConnectionList<TCPState> &clist = the C++ deque that handles all the connections
// DESCRIPTION:
//  Function will handle receiving the TCP segment, retrieving the header info,
//  and going through the various states of the finite state machine.
/* ======================================================================== */
void handle_mux(const MinetHandle &mux, const MinetHandle &sock, ConnectionList<TCPState> &clist)
{
  //ERRLOG(5, ERR << "[HANDLE_MUX]new ip packet arrived");
 
  Packet packet;
  Packet outgoing_packet;
  bool check_sum;
  TCPHeader tcp;
  IPHeader ip;
  Connection c;
 
  //unsigned int status;
  //unsigned int request = 0;
  unsigned int ackIN, seqIN;  // incoming packets ACK and SEQUENCE number values
  unsigned int ackOUT, seqOUT; // OUTGOING packets ACK and SEQUENCE number values (generated)
 
 
  SockRequestResponse response, request;
  unsigned short length;
  unsigned short len, tlen;
  unsigned char tcph_len, iph_len;
  unsigned char ilen;
 
  // used to store the TCP header length and IP header length, as well as the flag values (URG, ACK, PSH, RST, SYN, FIN)
  unsigned char inFlags;
  unsigned short window_size, urgent_p;
 
  Buffer buffer;  // used to store the packet payload
 
  // Use Minet API call to obtain information from the connection handle and place it in the packet
  MinetReceive(mux, packet);

  tlen = TCPHeader::EstimateTCPHeaderLength(packet);
  ERRLOG(5, ERR << "MUX[]TCP Header length = " << tlen);
 
  // Extract and find header from payload.  use the built in function call to estimate the TCP header length,
  // and then extract the header from the packet payload
  packet.ExtractHeaderFromPayload < TCPHeader > (TCPHeader::EstimateTCPHeaderLength(packet));
  tcp = packet.FindHeader(Headers::TCPHeader);
 
  // Extract IP Header
  ip = packet.FindHeader(Headers::IPHeader);
 
  // Extract IP Header Data. Note that this is flipped around because "source" is interpreted as "this machine",
  // and this function handles receiving data
  ip.GetDestIP(c.src);
  ip.GetSourceIP(c.dest);
  ip.GetProtocol(c.protocol);
 
  // Extract TCP Header Data
  tcp.GetSourcePort(c.destport);  // retrieve the source port and store it in the Connection object
  tcp.GetDestPort(c.srcport);   // retrieve the destination port and store it in the Connection object
  tcp.GetSeqNum(seqIN);   // retrieve the sequence number for packet and store it in seq
  tcp.GetAckNum(ackIN);   // retrieve the acknowledgement number for packet and store it in ack
  tcp.GetFlags(inFlags);    // retrieve the various 1 bit flags and store them in flags
  tcp.GetWinSize(window_size);  // retrieve the window size and store it in window_size
  tcp.GetUrgentPtr(urgent_p);   // retrieve the urgent pointer and store it in urgent_p
 
  // Get the data!
  ip.GetTotalLength(length);
  ERRLOG(5, ERR << "MUX[]total datagram length = " << length);
  ip.GetHeaderLength(iph_len);
  //ERRLOG(5, ERR << "MUX[]IP Header length = " << iph_len);
  iph_len = iph_len << 2; // shift left 2 bits
  tcp.GetHeaderLen(tcph_len);
  tcph_len = tcph_len << 2; // shift left 2 bits
  length = length - tlen - 20;
 
  // dump information out about the packet we just got
  ERRLOG(5, ERR << "MUX[]Datagram source ip field: " << c.src); //cout << "Connection source: " << c.src << endl;
  ERRLOG(5, ERR << "MUX[]datagram destination ip field: " << c.dest); //cout << "Connection destination: " << c.dest << endl;
  ERRLOG(5, ERR << "MUX[]Datagram source port field: " << c.srcport); //cout << "Connection source: " << c.src << endl;
  ERRLOG(5, ERR << "MUX[]datagram destination port field: " << c.destport); //cout << "Connection destination: " << c.dest << endl;
  ERRLOG(5, ERR << "MUX[]data length = " << length); // cout << "IP length = " << length << endl;
 
  // get the payload of the packet and extract the front portion (from 0 to length) of the packet
  buffer = packet.GetPayload().ExtractFront(length);

  buffer.Print(ERR);
 
  // Verify checksum
  check_sum = tcp.IsCorrectChecksum(packet);
  // <<<<IF CHECKSUM NOT CORRECT, MUST DO WE NEED TO HANDLE THIS... ???
  ERRLOG(5, ERR << "MUX[]recievedSEQ#=" << seqIN << " receivedACK#=" << ackIN);
 
  ConnectionList<TCPState>::iterator cs = clist.FindMatching(c); // look up the connection in the connection list
 
  /*
   * TCP ConnectionList:
   * Use c and ConnectionList structure to fetch a matched connectionState.
   * At this point, the Connection object is populated with the 5-tuple -
   * source host and port, destination host and port, and protocol
   * Each connection has a corresponding connection state.
   */
 
  // Now we look through the connection list to find the matching connection
 
  // >>>>re ENABLE FOR DEBUGGING if needed>>>>>>>>
  if (1==0)
  {
    ERRLOG(5, ERR << "MUX[]Printing connection list...");
    int conCnt = 0;
    for (ConnectionList<TCPState>::iterator cs = clist.begin(); cs != clist.end(); cs++) {
 
      ERRLOG(5, ERR << "MUX[]CONNECTION# " << ++conCnt);
      cs->Print(ERR);  //cs->Print(cout);
      ERRLOG(5, ERR); // timestamp the conneection list
      //cout << endl << endl;
    }
    ERRLOG(5, ERR << "MUX[]...Finished printing connection list");
  } // 1==0
 
 
  // -----------------------------------------------
  // CHECK for a conneciton, if it exists, handle it
  // -----------------------------------------------
  // **** CONNECTION DOES NOT EXIST ****
  // ------------------------------------
  if (cs == clist.end())  // connection does not exist!!!! >>>
  {
    ERRLOG(5, ERR << "MUX[]Connection does not exist!" << endl << "IP header of ROGUE packet>>");
    ip.Print(ERR);
    ERRLOG(5, ERR << "MUX[]TCP header of ROGUE packet>> ");
    tcp.Print(ERR);
    ERRLOG(5, ERR);
    ERRLOG(5, ERR << "MUX[State:[remaining handling of CONNECTION DOES NOT EXIST *********TEMPORRILY DISABLED***********]");
    /* >>>>>>>>>>>>Temporarily disabled>>>>>
    if (IS_FIN(inFlags)) {
      //ERRLOG(5, ERR << c); <<< we cannot print the connection if it does not exist!
      // Passive/Active open constructor
      // TCPState::TCPState(unsigned int initialSequenceNum, unsigned int state, unsigned int timertries)
      TCPState a_new_state(1, CLOSED, 3);
      a_new_state.SetLastRecvd(seq);
      a_new_state.last_acked = (ack - 1);
 
      // Constructor
      // ConnectionToStateMapping(const Connection &c, const Time &t, const STATE &s, const bool &b) : connection(c), timeout(t), state(s), bTmrActive(b) {}
      ConnectionToStateMapping < TCPState > a_new_map(c, Time(), a_new_state, false);
      createIPPacket(outgoing_packet, a_new_map, 0, F_ACK, cs->state.GetLastAcked(), cs->state.GetLastRecvd());
      MinetSend(mux, outgoing_packet);
      bFirstSend = false;
      ERRLOG(5, ERR << "Connection does not exist, ACK sent");
    }  // end if(IS_FIN(inFlags))
 
    else if (IS_SYN(inFlags)) {
      ERRLOG(5, ERR << "SYN");
    }
    */
  } // if (connection does not exist)
 
  // **** CONNECTION EXISTS - HANDLE THE INCOMING PACKET ****
  // ---------------------------------------------------------
  else
  {   //preliminary check to make sure the iterator hasn't reached the end of the deque
    ERRLOG(5, ERR << "MUX[]Connection exists:");
    ERRLOG(5, ERR << c);
 
    unsigned int currentState; //this will hold the current state of the TCP connection
 
    // populate the current state with the corresponding state from the finite state machine - cs->state returns a TCPState object
    currentState = cs->state.GetState();
    ERRLOG(5, ERR << "MUX[]state = " << currentState); // cout << "state = " << currentState << endl;
 
    //update the sender's receiving window size -- cs->state.rwnd is used for sender's side
    cs->state.rwnd = window_size;
    if (window_size <= cs->state.GetN()) { //if other's window size is less than our sending window size --> N IS THE WINDOW SIZE FOR RECEIVER
     cs->state.N = window_size / TCP_MAXIMUM_SEGMENT_SIZE * TCP_MAXIMUM_SEGMENT_SIZE; //appropriately update the receiver's window size by taking a fraction the MSS
    } 
    // cs->state.rwnd = window_size; // note the remote computers window size
    // if (window_size > 536) window_size = 536; // make our window size max of 536
    // cs->state.N = window_size; // set our window size
 
 
    // Main handling functions for finite state machine
    // ------------------------------------------------
    switch (currentState)
    {
      //  This case can only be invoked by call from application layer.
      //  Nothing to do in handle_mux.
      //  represents no connection state at all.
      // --------------------------------------
      case CLOSED:
        ERRLOG(5, ERR << "MUX[State:CLOSED] !!!!WARNING!!!! SHOULD NEVER GET TO HERE IN CODE");
        break; // end case CLOSED
 
 
      // One action: When SYN packet is received, transfer to SYN_RCVD state --> typical server transition
      // represents waiting for a connection request from any remote TCP and port
      // --------------------------------------
      case LISTEN:
        {
          ERRLOG(5, ERR << "MUX[State:LISTEN]");
 
          if (IS_SYN(inFlags)) // FOR PASSIVE OPEN: if we get a packet with the SYN bit flipped, we set the state to SYN_RCVD and send a SYN, ACK
          {
            ERRLOG(5, ERR << "MUX[State:LISTEN]SYN packet Received, move to state SYN_RCVD");
            cs->state.SetLastRecvd(seqIN);      // NOTE the last received packet's sequence # from remote computer (this is remote_ISN(0))
                  ERRLOG(5, ERR << "MUX[State:LISTEN]setting cs->state.SetLastRecvd() = " << seqIN);
            ackOUT = seqIN + 1; // for the "listen"(NOT the established) outgoing ack is the INCOMING seq#+1
            // seqOUT = 2468; // <<<< THIS WILL NEED TO BE A RANDOM NUMBER LATER!
            seqOUT = rand() % 10000; // now it is a random number
 
            //cs->state.SetLastAcked(ackOUT);   // NOTE: this is the
            //      ERRLOG(5, ERR << "MUX[State:LISTEN]setting cs->state.SetLastAcked() = " << ackOUT);
 
            // create syn ack packet, passing in a Packet object for the outgoing packet, the current
            // ConnectionToStateMapping reference, the size of the data, and the F_SYNACK flag
            ERRLOG(5, ERR << "MUX[State:LISTEN]doing createIPPacket() seqOUT=" << seqOUT << "  ackOUT=" << ackOUT);
 
            // > > > > > >
            ERRLOG(5, ERR << "MUX[State:LISTEN]***changing state to SYN_RCVD***");
            cs->state.SetState(SYN_RCVD);   // set the current state to SYN_RCVD
            // < < < < < < 
 
            cs->state.SetLastSent(seqOUT);   // NOTE LAST SEQUENCE NUMBER we are SENDING OUT.
                                              /// will eventually be a random# but: (this is local_ISN(0))
 
 
            // NIKHIL: THE FOLLOWING NEEDS TO BE HANDLED WITH A TIMER INSTEAD, AND THE "RESEND" OF THIS PACKET
            //  NEEDS TO BE HANDLED AS A TIMEOUT EVENT (I.E. IN case SYN_SENT IN handle_timeout() )
            // >>>if this is for the "lost first packet" thing, it will be cleaner if we use a ping instead
            //// check to see if this is the first packet sent
            //if (firstSent){  // if it is, sleep for 2 seconds before sending another packet with the same information, then set the flag to false
            //  sleep(2);
            //  MinetSend(mux, outgoing_packet);
            //  firstSent = false;
            //}
            // >>>I.E. this is what this is for>>>>>
            // set the timeout timer
            cs->bTmrActive = true;   // activate time out
            cs->timeout = Time() + ACK_TIMEOUT;    // set time out period for the expected ACK from the remote side
            //cs->timeout = Time() + 2.0; // Set time out period for the expected ACK from the remote side
            cs->connection = c;     // update connection       // <<<<I am not sure I understand this ???-DEB-
            createIPPacket(outgoing_packet, *cs, 0, F_SYNACK, seqOUT, ackOUT);  // (,,,,seq#,ack#)
            ERRLOG(5, ERR << "MUX[State:LISTEN]SYN ACK packet finished creating.  Here is the packet>>>");
            outgoing_packet.Print(ERR);  
            outgoing_packet.Print(cout);

            //call upon the Minet API to send the Packet via the mux MinetHandle
            MinetSend(mux, outgoing_packet);
            if (bFirstSend){  // if it is, sleep for 2 seconds before sending another packet with the same information, then set the flag to false
             sleep(2);
             // MinetSend(mux, outgoing_packet);
             bFirstSend = false;
            }
            break;
          } // if (IS_SYN(inFlags))
 
 
          // I don't think we have to worry about this just yet - i.e. getting a "FIN" while in a listen
          // state for a port.  Lets just ignore it for now.
          else if (IS_FIN(inFlags)) {  // if the packet we receive has the FIN bit set, send a FIN, ACK with data size = 0
            createIPPacket(outgoing_packet, *cs, 0, F_FINACK, cs->state.GetLastAcked(), cs->state.GetLastRecvd());
            MinetSend(mux, outgoing_packet);
          }
          break;
        } // end case LISTEN
        //break; //end case LISTEN
 
 
      // Three actions: 1) receive RST then transfer to LISTEN,
      //             or 2) get application data then transfer to FIN_WAIT_1
      //             or 3) receive ACK, then transfer to ESTABLISHED
      // represents waiting for a confirming connection acknowledgement
      // after having both received and sent a connection request.
      // -------------------------------------------------------------------
      case SYN_RCVD:
        {
          ERRLOG(5, ERR << "MUX[State:SYN_RCVD]");
 
          // If the inFlags received have the ACK bit set
          if (IS_ACK(inFlags)) {  // 3) receive ACK, then transfer to ESTABLISHED
            ERRLOG(5, ERR << "MUX[State:SYN_RCVD]RECEIVED ACK IN SYN_RCVD STATE.");     // cout << "Current ACK: " << ack << endl;
            ERRLOG(5, ERR << "MUX[State:SYN_RCVD]ackIN = " << ackIN);       // cout << "Last sent ACK: " << cs->state.GetLastSent() << endl;
            ERRLOG(5, ERR << "MUX[State:SYN_RCVD]cs->state.GetLastSent()= " << cs->state.GetLastSent());     // cs is the current connection being serviced from the connection list
 
            if (cs->state.GetLastSent() + 1 == ackIN) { // check to see if adding one to the last sent packet from the
                                                      // sender equals the correct ACK number in the packet
              // > > > > > >
              ERRLOG(5, ERR << "MUX[State:SYN_RCVD]***changing state to ESTABLISHED***");
              cs->state.SetState(ESTABLISHED);    // set the state to ESTABLISHED - yay!
              // < < < < < <
 
              //cs->state.SetLastAcked(ackIN);        // set the lastAcked packet to the current
              cs->state.SetSendRwnd(window_size);   // set the received
              cs->bTmrActive = false;   // don't set the timeout timer since the packet was received correctly
 
              // Populate the SockRequestResponse object
              response.type = WRITE;    // we want to write out the data (which is nothing at this point)
              response.connection = c;  // set the connection equal to the Connection object (5-tuple from above)
              response.error = EOK;     // set the error to none (EOK = no error)
              response.bytes = 0;       // no bytes are being sent
 
              ERRLOG(5, ERR << "MUX[State:SYN_RCVD]sending ESTABLISHED notificaiton back up to application layer");
              MinetSend(sock, response);  // send the SockRequestResponse on the other MinetHandle
            }
            else
              ERRLOG(5, ERR << "MUX[State:SYN_RCVD]ack not correct! (state not established) OR we got data on 3rd msg?");
          }
 
          // got a reset... so return state back to listen
          else if (IS_RST(inFlags)) { // 1) receive RST then transfer to LISTEN,
            
          }
          // else if (2 get application data then transfer to FIN_WAIT_1)
            // [[[TODO handle 2) get application data then transfer to FIN_WAIT_1]]]
        }
        break; // end case SYN_RCVD
 
 
      // Three actions: 1) receive RST then transfer to SYN_RCVD,
      //             or 2) get application data then transfer to CLOSED
      //             or 3) receive SYN ACK, then transfer to ESTABLISHED and send an ACK
      // represents waiting for a matching connection request after
      // having sent a connection request
      // --------------------------------------
      case SYN_SENT:
        {
          // ERRLOG(5, ERR << "[State:SYN_SENT*********TEMPORRILY DISABLED***********]");
          ERRLOG(5, ERR<< "MUX[State:SYN_SENT]ACK=" << ackIN << "  SEQ#=" << seqIN);
          if (IS_SYN(inFlags)) ERRLOG(5, ERR << "MUX[State:SYN_SENT]FLAG SET: IS SYN");
          if (IS_FIN(inFlags)) ERRLOG(5, ERR << "MUX[State:SYN_SENT]FLAG SET: IS FIN");
          if (IS_ACK(inFlags)) ERRLOG(5, ERR << "MUX[State:SYN_SENT]FLAG SET: IS ACK");
 
          // Got SYN and ACK flags from remote computer, move to ESTABLISHED
          if (IS_SYN(inFlags) && IS_ACK(inFlags))
          {
            // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            // Use this as a generic block for handling incoming packets/sending out new.... (modify for elsewhere)
            // >>>>>>>>>>>>>BEGIN of generic packet in-packet out block >>>>>>>>>>>>>>>>>>>
            cs->state.SetSendRwnd(window_size);   // set our window size
            cs->state.SetLastRecvd(seqIN);   // NOTE the last received packet's sequence # from remote computer (this is remote_ISN(#+n))
                  ERRLOG(5, ERR << "MUX[State:SYN_SENT]setting cs->state.SetLastRecvd() = " << seqIN);
            seqOUT = ackIN; // send out the seq# that the remote computer wants
            ackOUT = seqIN + 1; // this is the one seq# that WE want next...
                  //  ...for the "listen"(NOT the established) outgoing ack is the INCOMING seq#+1 or ???
            ERRLOG(5, ERR << "MUX[State:SYN_SENT]doing createIPPacket() seqOUT=" << seqOUT << "  ackOUT=" << ackOUT);
            //createIPPacket(outgoing_packet, *cs, 0, F_SYNACK, seqOUT, ackOUT);  // (,,,,seq#,ack#)
 
            createIPPacket(outgoing_packet, *cs, 0, F_ACK, seqOUT, ackOUT);
            ERRLOG(5,ERR<<"MUX[State:SYN_SENT]sending this OUTGOING_PACKET>>>>");
            outgoing_packet.Print(ERR);
            ERRLOG(5,ERR);
            MinetSend(mux, outgoing_packet);      // send the ACK packet via the MinetHandle mux
 
            // > > > > > >
            ERRLOG(5, ERR << "MUX[State:SYN_SENT]***changing state to ESTABLISHED***");
            cs->state.SetState(ESTABLISHED);
            // < < < < < <
 
            cs->state.SetLastSent(seqOUT);  // Save the last seq number we sent out. (NOTE: this is local_ISN(? + n))
            // cs->bTmrActive = false;         // disable timeout now.
            // >>>alternatively,   maybe set time out period for something here, such as:
             cs->bTmrActive = true;   // activate time out
             cs->timeout = Time() + ACK_TIMEOUT;  //<<or>>  cs->timeout = Time() + some floating point time in seconds
            // <<<<<<<<<<<<<<<end of generic packet in-packet out block <<<<<<<<<<<<<<<<<
            // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 
            // SEND the socket repsonse for this back up to the APPLICATION LAYER.
            // - - - - - - - - - - - - - - - - - - - - - - - - -
            //SockRequestResponse write (WRITE, cs->connection, buffer, 0, EOK);
            response.type = WRITE;
            response.connection = cs->connection;
            response.data = buffer;
            response.bytes = 0;
            response.error = EOK;
            ERRLOG(5,ERR<<"MUX[State:SYN_SENT]Response for MINETSEND>>>>>");
            response.Print(ERR);
            ERRLOG(5, ERR);
            MinetSend(sock, response);  // send to applicaiotn layer

          } //if (IS_SYN(inFlags) && IS_ACK(inFlags))
        }
        break; // end case SYN_SENT
 
 
      // No action taken in handle_mux.
      // --------------------------------------
      case SYN_SENT1:
        {
          ERRLOG(5, ERR << "[State:SYN_SENT1*********TEMPORRILY DISABLED***********]");
        }
        break; // end case SYN_SENT1
 
 
      //  Two actions: 1) receive FIN then tranfer to CLOSE_WAIT,
      //              or 2) get application data then tranfer to FIN_WAIT_1
      //  represents an open connection, data received can be delivered
      //  to the user.  The normal state for the data transfer phase of the connection.
      // ------------------------------------------------------------------
      case ESTABLISHED:
        {

          ERRLOG(5, ERR << "MUX[State:ESTABLISHED] ackIN = " << ackIN);
          ERRLOG(5, ERR << "MUX[State:ESTABLISHED] seqIN = " << seqIN);
          ERRLOG(5, ERR << "MUX[State:ESTABLISHED] cs->state.GetLastSent()= " << cs->state.GetLastSent());
          ERRLOG(5, ERR << "MUX[State:ESTABLISHED] cs->state.GetLastSent()=" << cs->state.GetLastSent() << "  cs->state.GetLastRecvd()=" << cs->state.GetLastRecvd());
          ERRLOG(5, ERR << "MUX[State:ESTABLISHED********* ENABLED ***********]");
 
          /*
          // -----------
          // Established
          // -----------
          */
          if (IS_PSH(inFlags)) {  // if the push bit is set
            ERRLOG(5, ERR << "MUX[State:ESTABLISHED] PSH set *****");
            ERRLOG(5, ERR << "MUX[State:ESTABLISHED] cs->state.GetLastRecvd() + 1 =" << cs->state.GetLastRecvd() + 1);
            // Check sequence number and checksum
            //if (seqIN == cs->state.GetLastRecvd() + 1) { // if the sequence number = the last one received on the
                                                                // receiver's side and the check_sum 'checks' out... :-)
          
              ERRLOG(5, ERR << "recv data good");
              cs->state.SetLastRecvd((unsigned int) seqIN + length); // set the last received sequence number to the current seq plus the length of the data
                                                                   // (total packet length - tcp header length - ip header length)
 
              // cout << "set acked " << ((cs->state.SetLastAcked(ack)) ? "yes" : "no") << endl;
              (cs->state.SetLastAcked(ackIN)) ? ERRLOG(5, ERR << "set acked YES") : ERRLOG(5, ERR << "set acked NO");
              ERRLOG(5, ERR << "MUX[State:ESTABLISHED] cs->state.GetLastAcked()=" << cs->state.GetLastAcked());
 
              ERRLOG(5, ERR << "MUX[State:ESTABLISHED] cs->state.GetRwnd() = " << cs->state.GetRwnd());
              if (cs->state.GetRwnd() >= length) {      // there is enough space in receive window
                ERRLOG(5, ERR << "MUX[State:ESTABLISHED] Length fits within window size = " << cs->state.GetRwnd() << ", " << length);
                cs->state.RecvBuffer.AddBack(buffer);   // get the receiver's buffer and add the data stored in buffer to it
                // Write to socket
                response.type = WRITE;
                response.connection = cs->connection;
                response.data = cs->state.RecvBuffer;
                response.bytes = cs->state.RecvBuffer.GetSize();
                response.error = EOK;
                MinetSend(sock, response);
              }
              else {    //not enough space in recv buffer
                ERRLOG(5, ERR << "Not enough space in the receiving buffer.");
                cs->state.SetLastRecvd(seqIN);  // set the last received sequence number to the same one as previous
                ERRLOG(5, ERR << "MUX[State:ESTABLISHED] cs->state.GetLastSent()=" << cs->state.GetLastSent() << "  cs->state.GetLastRecvd()=" << cs->state.GetLastRecvd());
              }
              // Make and send an ACK Packet, assuming ordering
              ERRLOG(5, ERR << "Making and sending ACK packet inside PSH flag handling.");
              // seq number, ack number
              createIPPacket(outgoing_packet, *cs, 0, F_ACK, cs->state.GetLastAcked() + 1, seqIN + length);    //create ack packet
              MinetSend(mux, outgoing_packet);
            //} // if (seq == cs->state.GetLastRecvd() && check_sum)
            // else
            // {
            //   // Unexpected seq: create ACK Packet
            //   ERRLOG(5, ERR << "Unexpected sequence number. Creating ACK packet.");
            //   createIPPacket(outgoing_packet, *cs, 0, F_ACK, cs->state.GetLastAcked() + 1, seqIN + length); 
            //   MinetSend(mux, outgoing_packet);
            // }
          } // if (IS_PSH(inFlags))
          // /////////// FIN FLAG SET /////////////
          else if (IS_FIN(inFlags)) {  // if the FIN bit is set, send an ACK and a FIN and go to LAST_ACK
                                     // (*** TODO: should be going to CLOSE_WAIT first, and then LAST_ACK)
            cs->state.SetLastRecvd(seqIN + 1);
            createIPPacket(outgoing_packet, *cs, 0, F_FINACK, cs->state.GetLastAcked(), cs->state.GetLastRecvd()); //create fin ack packet, should be sending ACK,
                                                               // and then CLOSE_WAIT should be sending the FIN
            MinetSend(mux, outgoing_packet);
 
            // Update state -- TODO: GO TO CLOSE_WAIT STATE FIRST AND THEN LAST_ACK, as in diagram
            // > > > > > >
            cs->state.SetState(LAST_ACK);
            // < < < < < <
          } // else if (IS_FIN(inFlags))
          ////////////////////////////////////////
          else if (IS_ACK(inFlags)) { // if we have the ACK bit set, we came from SYN_RCVD state, typical server transition
            ERRLOG(5, ERR << "Only ACK ");
            if (ackIN == cs->state.GetLastAcked()) {  // duplicate ack
                                                        // it will time out
              ERRLOG(5, ERR << "Duplicate ");
            }
            else {  //update tcp state
              cs->state.SetLastRecvd((unsigned int) seqIN + length);
              if (cs->state.SetLastAcked(ackIN)) {
                ERRLOG(5, ERR << "Timer off ");
                cs->bTmrActive = false;    // turn off timer
              }
            }
            cout << endl;
          } // else if (IS_ACK(inFlags))
          
        }
        break;  // end case ESTABLISHED
 
 
 
      //  No action taken in handle_mux.
      // --------------------------------------
      case SEND_DATA:
        {
          ERRLOG(5, ERR << "[State:SEND_DATA*********TEMPORRILY DISABLED***********]");
 
        }
        break; // end case SEND_DATA
 
 
      //  No action taken in handle_mux.
      //  represents waiting for a connection termination request
      //  from the local user.
      // --------------------------------------
      case CLOSE_WAIT:
        {
          ERRLOG(5, ERR << "[State:CLOSE_WAIT*********TEMPORRILY DISABLED***********]");
 
        }
        break;  // end case CLOSE_WAIT
 
 
      //  Three actions: 1) receive ACK then tranfer to FIN_WAIT_2,
      //              or 2) receive FIN then tranfer to CLOSING
      //              or 3) receive FIN ACK, then tranfer to TIME_WAIT
      //  represents waiting for a connection termination request
      //  from the remote TCP, or an ACK of the connection termination
      //  request previously sent.
      // --------------------------------------
      case FIN_WAIT1:
        {
          ERRLOG(5, ERR << "[State:FIN_WAIT1********* ENABLED ***********]");
          
          if (IS_ACK(inFlags)) {  // go to FIN_WAIT2 state
 
          }
 
          else if (IS_FIN(inFlags)) { // go to CLOSING state, and send an ACK
 
          }
 
          else if (IS_FIN(inFlags) && IS_ACK(inFlags)) {    // send out last ack
            cs->state.SetLastRecvd(seqIN + 1);
            cs->state.SetLastAcked(ackIN);
            createIPPacket(outgoing_packet, *cs, 0, F_ACK, cs->state.GetLastAcked(), cs->state.GetLastRecvd());
            MinetSend(mux, outgoing_packet);
 
            cs->bTmrActive = true;       // turn on timer
            cs->timeout = Time() + 5;    // wait 5 seconds
            // > > > > > >
            cs->state.SetState(TIME_WAIT);  // set the state to TIME_WAIT
            // < < < < < <
          } // else if (IS_FIN(inFlags) && IS_ACK(inFlags))
          
        }
        break; // end case FIN_WAIT1
 
 
      //  One action: receive ACK and transfer to TIME_WAIT state.
      //  represents waiting for a connection termination request ACK
      //  from the remote TCP.
      // --------------------------------------
      case CLOSING:
        {
          ERRLOG(5, ERR << "[State:CLOSING*********TEMPORRILY DISABLED***********]");
          if (IS_ACK(inFlags)) {
            ERRLOG(5, ERR << "ACK received in CLOSING state.");
          }
 
        }
        break; // end case CLOSING
 
 
      //  One action: receive ACK then transfer to CLOSED state.
      //  represents waiting for an ACK of the connection termination
      //  request previously sent to the remote TCP (which includes
      //  an ACK of its connection termination request).
      // --------------------------------------
      case LAST_ACK: //waiting for last ack
        {
          ERRLOG(5, ERR << "[State:LAST_ACK********* ENABLED ***********]");
          
          if (IS_ACK(inFlags)) {
            ERRLOG(5, ERR << "Last ACK coming");
            response.connection = c;
            response.type = WRITE;
            response.bytes = 0;
            if (cs == clist.end()) {
              response.error = ENOMATCH;
            }
            else {
              ERRLOG(5, ERR << "Connection ok");
              response.error = EOK;
            }
            MinetSend(sock, response);
            // > > > > > >
            cs->state.SetState(CLOSED); //set the state of the connection to CLOSED after the last ACK
            // < < < < < <
            clist.erase(cs);    //delete the connection from the ConnectionList
            ERRLOG(5, ERR << "Send close to sock");
          }
          
        } // if (IS_ACK(inFlags))
        break;  // end case LAST_ACK
 
 
      //  One action: receive FIN then transfer to TIME_WAIT state.
      //  represents waiting for a connection termination request
      //  from the remote TCP
      // --------------------------------------
      case FIN_WAIT2:
        {
          ERRLOG(5, ERR << "[State:FIN_WAIT2*********TEMPORRILY DISABLED***********]");
          if (IS_FIN(inFlags)) {   // go to TIME_WAIT state, send an ACK
 
          }
        }
        break; // end case FIN_WAIT2
 
 
      //  represents waiting for enough time to pass to be sure
      //  the remote TCP received the ACK of its connection termination
      //  request.
      // --------------------------------------
      case TIME_WAIT:
        {
          ERRLOG(5, ERR << "[State:TIME_WAIT*********TEMPORRILY DISABLED***********]");
        }
        break; // end case TIME_WAIT
 
 
      // --------------------------------------
      default:
        ERRLOG(5, ERR << "Default case.");
        // end default:
    } // end switch(currentState) (Main handlin functions for the finiate sate machine)
 
  } // end if (cs == clist.end())
 
} // void handle_mux()
 
 
 
/* ======================================================================== */
// FUNCTION: handle_sock
// PARAMS:
//      const MinetHandle &mux
//      const MinetHandle &sock
//      ConnectionList<TCPState> &clist)
// DESCRIPTION:
//      This function handles incoming socket requests and based on the 
//      state of the request, proceeds accordingly.  It handles both 
//      non-existent and existing connections separately.  When a CONNECT
//      event is received, this is an ACTIVE OPEN.  The ACCEPT event is for 
//      a PASSIVE OPEN.
/* ======================================================================== */
void handle_sock(const MinetHandle &mux, const MinetHandle &sock, ConnectionList<TCPState> &clist)
{
  Packet outgoing_packet;
  SockRequestResponse reply, request;
  Buffer buff;
 
  //Receive sock request here using Minet API and print out the character output.
  MinetReceive(sock, request);
 
  ERRLOG(5, ERR << "Socket request =");
  request.Print(ERR);
  ERRLOG(5, ERR);
 
  // Try to find the matching connection given in the request SockRequestResponse object from the connection list
  ConnectionList<TCPState>::iterator cs = clist.FindMatching(request.connection);
  // cout << request << endl;
  ERRLOG(5, ERR << request);
 
  // If we cannot find the matching connection in the list...
  // Handle new connections here.
  if (cs == clist.end()) {
 
    ERRLOG(5, ERR << "Connection does not exist. ");
    
    ERRLOG(5, ERR << "REQUEST TYPE: " << request.type);
 
    switch (request.type)
    {
      // (ACTIVE OPEN) - send SYN to remote side, initialize TCP variables like seq number
      // and send window variables, also set a timeout
      // --------------------------------------
      case CONNECT:
        {
          ERRLOG(5, ERR << "[SockRequest:CONNECT*********TEMPORRILY DISABLED***********]");
 
          // 1) create a connection in clist
          // 2) send a connect "SYN" packet to remote comptuer
          // 3) set state of the connection to SYN SENT.
          // 4) Tell the application it is ready to accept packets from remote computer
 
          
          //ERRLOG(5, ERR << "Connect event called");
          Packet p;
 
          // Active open, build a TCP connection and set the current status to SYN_SENT
          // rand() return a random int for the initial seq number, SYN_SENT = 3, last parameter is number of timer tries
          TCPState atcpstate(1, SYN_SENT, 3);
 
 
          // Set the connection to request socket's connection, set the timeout to the current time plus a predefined ACK timeout,
          // set the TCP state to SYN_SENT (see above for the details of the TCPState), and set the timer to active
          ConnectionToStateMapping < TCPState > new_map(request.connection, Time() + ACK_TIMEOUT, atcpstate, true);
          //new_map.Print(cout);
          new_map.Print(ERR);
          ERRLOG(5, ERR);
 
          //Create and send a SYN packet!
          //createIPPacket(outgoing_packet, new_map, 0, F_SYN, cs->state.GetLastAcked(), cs->state.GetLastRecvd()); // call helper method to create the SYN packet
          createIPPacket(outgoing_packet, new_map, 0, F_SYN, cs->state.GetLastAcked() + 1, cs->state.GetLastRecvd());
          MinetSend(mux, outgoing_packet);
          if (bFirstSend) {       // if this is the first packet sent, sleep for 2 seconds, and send the packet again
            sleep(2);
            MinetSend(mux, outgoing_packet); // We have use MinetSend twice for the first packet sent
            bFirstSend = false;   // set the flag to false
          }
          new_map.state.SetLastSent(new_map.state.GetLastSent() + 1); //increment the last sent packet on the sender's side
          clist.push_back(new_map);  // add the connectiontostatemapping to the front of the connection list
 
          //Send a reply to socket layer.
          reply.type = STATUS;
          reply.connection = request.connection;
          reply.bytes = 0;
          reply.error = EOK;
          MinetSend(sock, reply);   // reply back to the application
          
        }
        break; // end case CONNECT:
 
 
      // ----ACCEPT----
      // --------------------------------------
      case ACCEPT:
        {
          // 1) create a connection in clist
          // 2) set state of the connection to LISTEN (for incoming connections)
          // 3) Tell the application it is ready to accept packets from remote computer
 
          ERRLOG(5, ERR << "[SockRequest*********TEMPORRILY DISABLED***********]");
          
          //ERRLOG(5, ERR << "Request type ACCEPT. ");
          // Passive/Active open constructor
          // TCPState::TCPState(unsigned int initialSequenceNum, unsigned int state, unsigned int timertries)
          TCPState new_state(1, LISTEN, 3);   // start the sequence number at 1, the state now moves to LISTEN,
                                              // and set the timer tries to 3
 
          // set the connection to the request's connection, set the timeout starting time to current time,
          // set the TCP state to the one created above, and don't activate timer
          ConnectionToStateMapping < TCPState > new_map(request.connection, Time(), new_state, false);
 
          clist.push_back(new_map);   // add this connectiontostatemapping to the connection list
          // Send back a socket reply
          reply.type = STATUS;
          reply.connection = request.connection;
          reply.bytes = 0;
          reply.error = EOK;
          MinetSend(sock, reply);   // reply back to the application
          reply.Print(cout);
          reply.Print(ERR);
          ERRLOG(5, ERR);
          ERRLOG(5, ERR << "\nSocket reply sent from ACCEPT");
          
        }
        break; // end case ACCEPT
 
 
      // --------------------------------------
      case WRITE:
        {
          ERRLOG(5, ERR << "[SockRequest:WRITE]");
          reply.type = STATUS;
          reply.connection = request.connection;
          reply.bytes = 0;
          reply.error = ENOMATCH;
          MinetSend(sock, reply);
        }
        break; // end case WRITE
      
      case FORWARD: { } break;
 
      // --------------------------------------
      case CLOSE:   // handle closing the connection
        {
          ERRLOG(5, ERR << "[SockRequest:CLOSE]");
          reply.type = STATUS;
          reply.connection = request.connection;
          reply.bytes = 0;
          reply.error = ENOMATCH;
          MinetSend(sock, reply);   // if a CLOSE is encountered on the socket layer, send a reply to the socket layer to close it
        }
        break; // end case CLOSE
 
 
      // --------------------------------------
      case STATUS:
        {
          ERRLOG(5, ERR << "[SockRequest:STATUS]");
          //nothing to do;
        }
        break; // end case STATUS
 
 
      // --------------------------------------
      default:
        {
          ERRLOG(5, ERR << "[SockRequest:????? DEFAULT ????]");
          reply.type = STATUS;
          reply.error = EWHAT; // we don't know what the type is, set this error
          MinetSend(sock, reply);
        }
        // end default
 
    } // switch (request.type)
  } // end if (cs == clist.end())
 
  else //This is an existing connection.
  {
    ERRLOG(5, ERR << "The connection exists. ");
    int state = cs->state.GetState();
    // cout << (*cs) << endl;
    ERRLOG(5, ERR << (*cs));
    Buffer buff;
    switch (request.type)
    {
      // --------------------------------------
      // Shouldn't come here ******************
      case CONNECT:   
        ERRLOG(5, ERR << "[SockRequest(existing):CONNECT*********TEMPORARILY DISABLED***********]");
        //ERRLOG(5, ERR << "Connect event");
        break;
 
 
      // --------------------------------------
      // Shouldn't come here ******************
      case ACCEPT:
        ERRLOG(5, ERR << "[SockRequest(existing):ACCEPT*********TEMPORARILY DISABLED***********]");
        ERRLOG(5, ERR << "Accept event");
        break;
      
      // --------------------------------------
      case STATUS: {
        cout << "In status state of already existing connection.";
        if (state == ESTABLISHED) { // status in response to write event
          unsigned datasend = request.bytes;  //number of bytes sent
          cs->state.RecvBuffer.Erase(0, datasend);
          if(cs->state.SendBuffer.GetSize() != 0) {
            // data did not write completely
            cout << "Data did not write out completely." << endl;
            SockRequestResponse write (WRITE, cs->connection, cs->state.RecvBuffer, cs->state.RecvBuffer.GetSize(), EOK);
            MinetSend(sock, write);
          }
        }
      }
      break;
      // --------------------------------------
      case WRITE: {
        ERRLOG(5, ERR << "[SockRequest(existing):WRITE********* ENABLED ***********]");
        ERRLOG(5, ERR << "Write event");
        cout << " In write state of already existing connection. ";
          if(state == ESTABLISHED) {
            cout << "==== We are currently in the established state." << endl;
            cout << "SEND BUFFER SIZE: " << cs->state.SendBuffer.GetSize() << ", DATA SIZE: " << request.data.GetSize() << endl;
              if(cs->state.SendBuffer.GetSize()+request.data.GetSize() > cs->state.TCP_BUFFER_SIZE) {
                cout << "==== Send buffer does NOT have enough space." << endl;
                //send buffer does NOT have enough space
                reply.type = STATUS;
                reply.connection = request.connection;
                reply.bytes = 0;
                reply.error = EBUF_SPACE;
                MinetSend(sock, reply);
              }
              else {  // there is enough space in sendbuffer
                cout << "==== We have space in the send buffer so we're adding it." << endl;
                cs->state.SendBuffer.AddBack(request.data);//append new data
                cs->state.SendBuffer.Print(cout);
              }
              //to do
              /*if last_acked < last_sent, we are still on timer, dont send
               else we do the folloing,
               modify timer
               */
             cout << "LAST SENT: " << cs->state.last_sent << " LAST ACKED: " << cs->state.last_acked << " RWND: " << cs->state.rwnd << endl;
              //flow control, wouldn't send data overflow other conns
              if(cs->state.last_sent - cs->state.last_acked < cs->state.rwnd) {
                cout << "==== The last sent minus the last acked is less than the receiving window - we're good." << endl;
                if(cs->state.last_sent == cs->state.last_acked) { //no data on transition
                  //activate timer
                  cs->bTmrActive = true;
                  cs->timeout = Time() + 5; //expire in 5 sec
                }
                unsigned int total_to_send = 0; //total data send this time
                Buffer buffer = cs->state.SendBuffer; //copy buffer
                //get rid of data we already send, what left is data we haven't sent
                //buffer = buffer.ExtractFront(cs->state.last_sent-cs->state.last_acked);
                buffer = buffer.ExtractFront(request.data.GetSize());  
                unsigned int total_remaining = buffer.GetSize();
                
                int kk =0 ;
                //maximum amount of data we can sent and not cause over flow
                //Note, N is updated to be always less than rwnd
                unsigned int MAX_SENT = cs->state.GetN() - (cs->state.last_sent - cs->state.last_acked) - TCP_MAXIMUM_SEGMENT_SIZE;
                //while we have data left to send and total data send does not exceed 
                //our window size
                cout << "GET LAST RECVD: " << cs->state.GetLastRecvd() << " GET LAST ACKED: " << cs->state.GetLastAcked() << endl;
                while(total_remaining != 0 && total_to_send < MAX_SENT) {
                    unsigned int bytes = min(total_remaining, TCP_MAXIMUM_SEGMENT_SIZE); //data we send this time
                    cout << "INITIAL total_to_send: " << total_to_send << ", BYTES: " << bytes << endl;
                    outgoing_packet = buffer.Extract(0,bytes);
                    
                    //create packet - last two, seq number, ack number
                    createIPPacket(outgoing_packet, *cs, bytes, F_PSHACK, cs->state.GetLastAcked() + 1 + total_to_send, cs->state.GetLastRecvd() + 1);
                    //send ip packet
                    //if(kk!=0) {
                      MinetSend(mux, outgoing_packet);
                    //}
                    
                    kk++;
                    cs->state.SetLastSent(cs->state.GetLastSent() + bytes);
                    total_to_send += bytes;
                    //update total_remaining
                    total_remaining -= bytes;
                    cout << "TOTAL SEND: " << total_to_send << ", TOTAL LEFT: " << total_remaining << endl;

                }
              
                reply.type = STATUS;
                reply.connection = request.connection;
                reply.bytes = total_to_send;
                reply.error = EOK;
                reply.Print(cout);
                MinetSend(sock, reply);
              }
              
          } // end state ESTABLISHED
          else {
            ERRLOG(5, ERR <<"******** INVALID STATE ***********");
          } // end state INVALID STATE
      
        }
        break;

      case FORWARD: {
        ERRLOG(5, ERR << "[SockRequest(existing): FORWARD ********** ENABLED ************");
      }
      break;

      case CLOSE: {
        ERRLOG(5, ERR << "[SockRequest(existing): CLOSE ********** ENABLED ************");
        // Perform a check to determine from which state the CLOSE call was made
        if (state == ESTABLISHED) {
            ERRLOG(5, ERR << "ESTABLISHED state in CLOSE");
            // Create the IP packet to send out with the FIN flag set
            createIPPacket(outgoing_packet, *cs, 0, F_FIN, cs->state.GetLastAcked() + 1, cs->state.GetLastRecvd());
            MinetSend(mux, outgoing_packet);
            cs->state.SetLastSent(cs->state.GetLastSent() + 1);
            cs->state.SetState(FIN_WAIT1);
        }
        // If the state is already closed, simply send a status reply informing the sock module
        // that everything is ok and delete the connection from the master list
        else if (state == CLOSED) {
          reply.type = STATUS;
          reply.connection = request.connection;
          reply.bytes = 0;
          reply.error = EOK;
          MinetSend(sock, reply);
          clist.erase(cs);    // Erase the connection from the connection list
        }
        else {
          ERRLOG(5, ERR <<"******** INVALID STATE ***********");
        } // End invalid state

      } // end CLOSE state of existing connection
      break;
      // --------------------------------------
      default:
        ERRLOG(5, ERR << "[SockRequest(existing):????? default ??????]");
        break;
    } // end switch (request.type)
  } // end else (this is an existing connection)
} // void handle_sock
 
 
 
/* ======================================================================== */
// FUNCTION: handle_timeout
// PARAMS:
//      const MinetHandle &mux
//      const MinetHandle &sock
//      ConnectionList<TCPState> &clist)
// DESCRIPTION:
//      This method handles all incoming timeout requests. It loops through
//      all the connections in the master connections list, checks the
//      timer status of each one and then looks at the state of the
//      connection to determine how to proceed. 
/* ======================================================================== */
void handle_timeout(const MinetHandle &mux, const MinetHandle &sock, ConnectionList<TCPState> &clist)
{
  Time now; //get current time
  // The list that will store the connections to remove
  list <ConnectionList<TCPState>::iterator> connections_to_delete;
 
  // for each connection...
  for (ConnectionList<TCPState>::iterator cs = clist.begin(); cs != clist.end(); cs++) {
    if (!cs->bTmrActive) // if the timer was not activated, skip the connection
      continue;
 
    // cout << now << ", " << cs->timeout << endl << endl;
    if (now >= cs->timeout) {   //a timeout occurs if the time now is greater than or equal to the timeout specified in the connection
 
      ERRLOG(5, ERR << "TIME OUT HAS OCCURRED");
 
      switch (cs->state.GetState())
      {
        // --------------------------------------
        case SYN_SENT:
          {    //sent out syn waiting for synack
            ERRLOG(5, ERR << "[Timeout:SYN_SENT********* ENABLED ***********]");
            
            if (cs->state.ExpireTimerTries()) {   //we run out of tries, close the connection
              ERRLOG(5, ERR << "The number of timer tries has run out.");
              // Close the connection
              Buffer buffer;
              SockRequestResponse reply(WRITE, cs->connection, buffer, 0, ECONN_FAILED); //send a response writing back the connection failed error
              MinetSend(sock, reply);
              cs->bTmrActive = false;  // now the timer is deactivated
              // > > > > > >
              cs->state.SetState(CLOSING); // set the state to closing for the connection after there is no response on this connection
              // < < < < < <
            }
            else {   // send SYN again
              ERRLOG(5, ERR << "Sending out SYN again.");
              Packet outgoing_packet;
              createIPPacket(outgoing_packet, *cs, 0, F_SYN, cs->state.GetLastAcked(), cs->state.GetLastRecvd());    // Create the SYN packet, no bytes are sent
              MinetSend(mux, outgoing_packet);
              cs->timeout = now + 2;   // set the timeout to be 2 seconds from now
            }
            
          }
          break; // end case SYN_SENT:
 
 
        // ------------------------------------------
        // If we get a timeout in the SYN_RCVD state,
        // that means we didn't receive the proper ACK
        // ------------------------------------------
        case SYN_RCVD:
          {  //waiting for last ack coming
            ERRLOG(5, ERR << "[Timeout:SYN_RCVD********* ENABLED ***********]");
            
            if (cs->state.ExpireTimerTries()) {   //we run out of tries, close the connection
              ERRLOG(5, ERR << "The number of timer tries has run out.");
              //close the connection
              Buffer buffer;
              SockRequestResponse reply(WRITE, cs->connection, buffer, 0, ECONN_FAILED);  //close it
              MinetSend(sock, reply);
              cs->bTmrActive = false;
              // > > > > > >
              cs->state.SetState(LISTEN); // if from the SYN_RCVD state we have a failed conneciton, we go back to LISTEN state, not SYN_SENT
              // < < < < < <
            }
            else {
              ERRLOG(5, ERR << "Sending out SYN ACK again.");
              Packet outgoing_packet;
              createIPPacket(outgoing_packet, *cs, 0, F_SYNACK, cs->state.GetLastAcked(), cs->state.GetLastRecvd()); //create syn ack packet, no bytes sent
              MinetSend(mux, outgoing_packet);
              cs->timeout = now + 2;
            }
            
          }
          break; // end case SYN_RCVD:
 
        // --------------------------------------
        case TIME_WAIT:
          {
            ERRLOG(5, ERR << "[Timeout:TIME_WAIT********* ENABLED ***********]");
            //close the connection
            connections_to_delete.push_back(cs);
          }
          break; //end TIME_WAIT case
 
 
        // --------------------------------------
        case ESTABLISHED:
          {
            ERRLOG(5, ERR << "[Timeout:ESTABLISHED********* ENABLED ***********]");
            
            // we have an outstanding ACK
            // if the last acked packet number IS LESS THAN the last packet sent, that means that between
            // the last acked and last sent, we have unacknowledged packets
            if (cs->state.GetLastAcked() < cs->state.GetLastSent()) {
              //therefore, we need to activate the timer for the current connection
              cs->bTmrActive = true;
              cs->timeout = Time() + 5;   //expire in 5 sec
              unsigned int total_to_send = 0;   //initialize total data to send this time
              Buffer buffer = cs->state.SendBuffer;   //copy buffer
              //get rid of data we already sent, what's left is data we haven't sent
              // We have to retransmit from last Acked to last sent
              buffer = buffer.ExtractFront(cs->state.GetLastSent() - cs->state.GetLastAcked());
              unsigned int total_remaining = buffer.GetSize();
 
              Packet outgoing_packet;
              // calculate the maximum amount of data we can send and not cause overflow
              // Note, N is updated to be always <= rwnd, and N is the window size for the receiver
              unsigned int MAX_SENT = cs->state.GetN() - (cs->state.last_sent - cs->state.last_acked) - TCP_MAXIMUM_SEGMENT_SIZE;
 
              //while we have data left to send and total data send DOES NOT EXCEED our window size
              while ( total_remaining != 0 && total_to_send < MAX_SENT) {
                //data we send this time --> get the minimum of the total bytes left and the MSS
                unsigned int bytes = min(total_remaining, TCP_MAXIMUM_SEGMENT_SIZE); 
                ERRLOG(5, ERR << total_to_send << ", " << bytes);
                outgoing_packet = buffer.Extract(0, bytes);
                //create packet
                //createIPPacket(outgoing_packet, *cs, bytes, F_PSHACK, true, cs->state.GetLastAcked() + 1 + total_to_send); <<OLD
                // cs->state.GetLastSent() + total_to_send
                createIPPacket(outgoing_packet, *cs, bytes, F_PSHACK, cs->state.GetLastAcked() + 1 + total_to_send, cs->state.GetLastRecvd() + 1);
 
                //send ip packet for retransmitted data
                MinetSend(mux, outgoing_packet);
 
                cs->state.SetLastSent(cs->state.GetLastSent() + bytes);
                total_to_send += bytes;
                //update the total bytes left to transmit
                total_remaining -= bytes;
              }
            }
            else {  // there is no outstanding ACK, turn off the timer
              cs->bTmrActive = false;
            }
            
          }
          break; // end case ESTABLISHED
 
        // --------------------------------------
        default:
          {
            ERRLOG(5, ERR << "[Timeout:???? default ?????]");
          }
 
      } // end switch statement
    } // end if-statement
  } // end for loop for each connection (for (ConnectionList<TCPState>::iterator cs = clist.begin(); cs != clist.end(); cs++))
 
  // *** Go through the list of connections that need to be deleted - the ones who have entered the TIME_WAIT state and are done being serviced
  for (list<ConnectionList<TCPState>::iterator>::iterator it = connections_to_delete.begin(); it != connections_to_delete.end(); it++) {
    clist.erase(*it); //after the connection has been serviced, close it (remove it from the connection list)
  }
} // void handle_timeout()
 
 
/* ======================================================================== */
// FUNCTION: createIPPacket
// PARAMS:
//    Packet &packet
//    ConnectionToStateMapping<TCPState> &a_mapping
//    int size_of_data
//    int header
//    bool bCustomizedSeq
//    int my_seq
// DESCRIPTION:
//
/* ======================================================================== */
void createIPPacket(Packet &packet, ConnectionToStateMapping<TCPState> &a_mapping,
                     unsigned int size_of_data, unsigned int header,
                     unsigned int seqNumber, unsigned int ackNumber)  // <<< now passed in
{
  ERRLOG(5, ERR << "Create new IP packet");
  unsigned char flags = 0;
 
  int packet_length = size_of_data + TCP_HEADER_BASE_LENGTH + IP_HEADER_BASE_LENGTH;
  IPHeader ip;
  TCPHeader tcp;
  IPAddress source                = a_mapping.connection.src;
  IPAddress destination           = a_mapping.connection.dest;
  unsigned short source_port      = a_mapping.connection.srcport;
  unsigned short destination_port = a_mapping.connection.destport;
 
  switch (header)
  {
    // --------------------------------------
    case F_SYN:
      {
        ERRLOGnt(5, ERR << "{make:F_SYN}",false);
        SET_SYN(flags);
      }
      break;
 
 
    // --------------------------------------
    case F_ACK:
      {
        ERRLOGnt(5, ERR << "{make:F_ACK}",false);
        SET_ACK(flags);
      }
      break;
 
 
    // --------------------------------------
    case F_SYNACK:
      {
        ERRLOGnt(5, ERR << "{make:F_SYNACK}",false);
        SET_ACK(flags);
        SET_SYN(flags);
      }
      break;
 
 
    // --------------------------------------
    case F_PSHACK:
      {
        ERRLOGnt(5, ERR << "{make:F_PSHACK}",false);
        SET_PSH(flags);
        SET_ACK(flags);
      }
      break;
 
 
    // --------------------------------------
    case F_FIN:
      {
        ERRLOGnt(5, ERR << "{make:F_FIN}",false);
        SET_FIN(flags);
      }
      break;
 
 
    // --------------------------------------
    case F_FINACK:
      {
        ERRLOGnt(5, ERR << "{make:F_FINACK}",false);
        SET_FIN(flags);
        SET_ACK(flags);
      }
      break;
 
    // --------------------------------------
    case F_RST:
      {
        ERRLOGnt(5, ERR << "{make:F_RST}",false);
        SET_RST(flags);
      }
      break;
 
    default:
      {
        ERRLOGnt(5, ERR << "{make:???? default ????}",false);
      }
  } // switch (header)
 
  // Create the IP header. NOTE: IP checksum is automatically handled every time a set call is made
  ip.SetSourceIP(source);
  ip.SetDestIP(destination);
  ip.SetTotalLength(packet_length);
  ip.SetProtocol(IP_PROTO_TCP);
  // ip.Setid = ???? <<< Nikhil, not sure if we need to do this
  ERRLOG(5, ERR << "IP HEADER>> ");
  ip.Print(ERR);
  ERRLOG(5, ERR);
 
  // add to the packet
  packet.PushFrontHeader(ip);   //push the IP header to the packet
 
 
  // Create the TCP header
  tcp.SetSourcePort(source_port, packet);
  tcp.SetDestPort(destination_port, packet);
  tcp.SetHeaderLen(TCP_HEADER_BASE_LENGTH, packet);
  tcp.SetFlags(flags, packet);
 
  //tcp.SetAckNum(a_mapping.state.GetLastRecvd(), packet); // <<old
  tcp.SetAckNum(ackNumber, packet); // <<new
 
  // >>>Removed this, moved the ACK and SEQ numbers to elsewhere so they are paremeters to be passed in.
  // This will likley help us in the long run, especially when we have to implement things like go back n, etc...
  //if (bCustomizedSeq) { // >>>DO WE NEED TO FIX THIS ONE TOO WITH SEQ #????? >>>>>
  //  tcp.SetSeqNum(a_mapping.state.GetLastSent() + my_seq, packet);
  //}
  //else {
  //  tcp.SetSeqNum(a_mapping.state.GetLastAcked(), packet);
  //}
  tcp.SetSeqNum(seqNumber, packet); //<<new
 
 
  tcp.SetWinSize(a_mapping.state.GetRwnd(), packet);
  tcp.SetUrgentPtr(0, packet);
  tcp.RecomputeChecksum(packet);
  //tcp.Print(cout);
  ERRLOG(5, ERR << "TCP HEADER>> ");
  tcp.Print(ERR);
  ERRLOG(5, ERR);
 
  // now we want to have the TCP header BEHIND the IP header - add to the packet
  packet.PushBackHeader(tcp);
 
} // void createIPPacket()
 
 
 
 
/* ======================================================================== */
/* ======================================================================== */
/*                           SECONDARY FUNCTIONS                            */
/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
// FUNCTION (group): Error Logging Utility Functions
// PARAMS: (see above for global parameter descriptions for
//    ERR,  need_pre_endl, LOG_TO_MONITOR_WINDOW, LOG_TO_TCP_MODULE_WINDOW
//    LOG_LVL, REMOTE_SECONDS_OFFSET & REMOTE_MICROSECONDS_OFFSET
// DESCRIPTION: How the date stamped error logger works:
//   Basic imlementation:
//   In any function, send a message to the error log with something like the following:
//      ERRLOG(5, ERR << "This is an error entry. The #5: " << 5 << " the float 12.34: " << 12.34);
//   The error will be sent to the monitor window, prepended with the datestamp, and appended with an endl.
//
//   Error Level:
//   The "5" represents the "individual error importance" from 0 to 5, where 0 means
//   a not very important error, and 5 is very important.  This value works with the
//   LOG_LVL module-level constant value (which is another value from 0 to 5) where if...
//        (LOG_LVL) + ("individual error importance") >= 5
//   ...then the error will be sen to the tcp monitor window  >>>Otherwise it will be printed<<<
//   By adjusting these two values, journaling can be "throttled" either individually, or on a module level.
//
//   Additional parameters and versions:
//   There are a few flavors of the ERRLOG() function.  Regardless of which one is called,
//   all of them simply wrappers to the main _ERRLOG(), with the only difference being that
//   the other versions exist only to make it easier to "code" the errors by defaulting some of the flags.
//   i.e. instead of:
//      _ERRLOG(5, ERR << "Some error", true, true)
//   you could simply do something like this:
//      ERRLOG(ERR << "Some error");
 
// ERRLOG FUNCTION PARAMETERS:
//  #1 = 0 to 5 log priority for this individual error
//  #2 = output stream you want to dump
//  #3 = (if parameter exists) t/f to print an endl at the end of the error (defaults to T)
//  #4 = (if parameter exists) t/f to print the timestamp (defaults to T)
/* ======================================================================== */
void ERRLOG  (const std::ostream &jEnt)
{  // usage: ERRLOG(ERR << "send this # to the journal " << 12.34);
  _ERRLOG(5, jEnt, true, true);  // default to log level 5, add endl, timestamp
}
void ERRLOG  (const int lgLvl, const std::ostream &jEnt)
{ // usage: ERRLOG(3, ERR << "send this # to the journal " << 12.34);
  _ERRLOG(lgLvl, jEnt, true, true); // default to adding an endl, and timestamp
}
void ERRLOG  (const int lgLvl, const std::ostream &jEnt, const bool useEndl)
{ // usage: ERRLOG(3, ERR << "send this # to the journal " << 12.34, true);
  _ERRLOG(lgLvl, jEnt, useEndl, true); // default to adding a timestamp
}
void ERRLOGnt(const int lgLvl, const std::ostream &jEnt, const bool useEndl)
{ // usage: ERRLOG(3, ERR << "send this # to the journal " << 12.34, true);
  _ERRLOG(lgLvl, jEnt, useEndl, false); // default to NOT adding a timestamp
}
void _ERRLOG (const int lgLvl, const std::ostream &jEnt, const bool useEndl, const bool dumpTime)
{ // usage: _ERRLOG(3, ERR << "send this # to the journal " << 12.34, true, false);
  std::stringstream ss;
  ss << jEnt.rdbuf();
 
  // If the global journal level, PLUS the calling functions reqested journal level
  // is at least 5, then we are to log the message.
  if ((LOG_LVL + lgLvl) >= 5)  // only log if the log if past a certain log level
  {
    // minet monitor logging
    if (LOG_TO_MONITOR_WINDOW)
      MinetSendToMonitor(MinetMonitoringEvent(ss.str())); // send error to minet monitor
 
    // cout logging
    if (LOG_TO_TCP_MODULE_WINDOW) {
      if (useEndl && need_pre_endl)
      {
        cout << endl; // caller wants an endl, and
        if (LOG_TO_FILE) fprintf(logFile, "\n");
      }
 
      // the previous call was NOT printing endl's, so dump and endl
      // before we start
      need_pre_endl = false; // no longer need it.
 
      if (dumpTime) {
        wiresharkTimestamp(); // dump the wireshark-style timestamp
        cout << " ";
        if (LOG_TO_FILE) fprintf(logFile, " ");
      }
      cout << ss.str(); // dump the message
      if (LOG_TO_FILE) fprintf(logFile, "%s", ss.str().c_str());
 
      if (useEndl)
      {
        cout << endl; // send the end of line character too...
        if (LOG_TO_FILE) fprintf(logFile, "\n");
      }
 
      if (!useEndl)
        need_pre_endl = true; // if caller does NOT want to print out an endl,
      // then setting this flag says that it MUST be done next time BEFORE printing
      // anything out for the next journal entry (as long as the next entry
      // WANTS to print out an endl, otherwise we wont do this)
      cout << flush;
      if (LOG_TO_FILE) fflush(logFile);
 
    } // if (LOG_TO_TCP_MODULE_WINDOW)
  } // only log if 5 or more
} // void _ERRLOG()
 
/* ======================================================================== */
// FUNCTION: wiresharkTimestamp()
// PARAMS: LOG_TO_FILE = true to send timestamp entro to the log file,
//        false to not
// DESCRIPTION: generate a timestamp that is identical to the "time of day" timestamp
//        in wireshark. To select this format, in wireshark, go to:
//        view > time display format > time of day
/* ======================================================================== */
void wiresharkTimestamp()
{
  struct timeval tv;
  time_t timeNow;
  struct tm *nowtm;
  char tmbuf[64];
 
  gettimeofday(&tv, NULL); // what time is it?
  //wiresharkTimestampsync(); // test sync of both systems
 
  // create any required offset to better match the remote netlab computers time
  tv.tv_sec += REMOTE_SECONDS_OFFSET;
  tv.tv_usec += REMOTE_MICROSECONDS_OFFSET;
  if (tv.tv_usec > 999999) // must roll over into seconds
  {
    tv.tv_usec -= 1000000; // add a second to microsecs
    tv.tv_sec++;
  }
  if (tv.tv_usec < 0) // must roll over into seconds
  {
    tv.tv_usec += 1000000; // subtract a second from microsecs
    tv.tv_sec--;
  }
 
  // now create the timestamp
  timeNow = tv.tv_sec;
 
  nowtm = localtime(&timeNow);
  strftime(tmbuf, sizeof tmbuf, "%H:%M:%S", nowtm);
  cout << "[" << tmbuf;
  cout << "." << tv.tv_usec << "]";
  if (LOG_TO_FILE) fprintf(logFile, "[%s.%d]", tmbuf, (int)tv.tv_usec);
} // wiresharkTimestamp()
 
 
/* ======================================================================== */
// FUNCTION: openLogFile()
// PARAMS: LOG_TO_FILE = true to send log entries to the log file, false to not
// DESCRIPTION: Open the log file required to output error log entires
//
/* ======================================================================== */
void openLogFile()
{
  if (!LOG_TO_FILE)
    return; // do not log to the journal
 
  logFile = fopen("tcpjournal.txt", "w");
  if (!logFile)
  {
    LOG_TO_FILE = false;
    // ERRLOG(10, ERR << "WARNING: Could not open TCPjournal.txt to journal error log entries!");
    cout << "WARNING: Could not open TCPjournal.txt to journal error log entries!\n";
    // <--better to send this directly using cout
  }
  else
  {
    cout << "OUTPUT FILE JOURNAL NOW OPEN: Sending messages to: tcpjournal.txt\n";
    fprintf(logFile, "TCP_MODULE Journal\n"); // make the journal look pretty
    fprintf(logFile, "------------------\n");
  }
} // openLogFile()
 
 
/* ======================================================================== */
// FUNCTION: closeLogFile()
// PARAMS: LOG_TO_FILE = true to send log entries to the log file, false to not
// DESCRIPTION: Close the log file required to output error log entires
//
/* ======================================================================== */
void closeLogFile()
{
  if (!LOG_TO_FILE)
    return; // we never opened the journal file
  fclose (logFile);
} // closeLogFile()
 
 
/* ======================================================================== */
// PURPOSE:        Helper function for the hexdump function
// Preconditions:  mem_start_ptr, mem_end_ptr = pounsigned inters to the data
//                      range to dump to the screen
//                 row_offsset =
//                     loop counter for the rows of the hexdump
//                 fst_byte = first byte to print
//                 col_overflow = # of characters before overflow
// postconditions: One row of a hexdump
// COMMENTS:
/* ======================================================================== */
void hex_dump_row(char * mem_start_ptr, char * mem_end_ptr, unsigned int row_offset, unsigned int fst_byte, unsigned int col_overflow)
{
  unsigned int col_offset;
  unsigned int cur_byte;
  unsigned char one_char;
  unsigned char *mem = (unsigned char*) mem_start_ptr;
 
  // print out the hex value of bytes.
  // ---------------------------------
  for (col_offset = 0; col_offset < 16; col_offset++) {
    cur_byte = fst_byte + row_offset + col_offset;
    if (((unsigned int) mem_start_ptr <= cur_byte) && (cur_byte <= (unsigned int) mem_end_ptr))
    {
      printf("%02x", mem[row_offset + col_offset - col_overflow]);
      if (LOG_TO_FILE)
        fprintf(logFile, "%02x", mem[row_offset + col_offset - col_overflow]);
    }
    else
    {
      printf("  "); // dump empty!
      if (LOG_TO_FILE)
        fprintf(logFile, "  ");
    }
 
    // handle space between units, or "-"
    if (col_offset == 7)
    {
      printf("-"); // use this to divide each half of 16
      if (LOG_TO_FILE)
        fprintf(logFile, "-");
    }
    else
    {
      printf(" "); // use for spaces between bytes
      if (LOG_TO_FILE)
        fprintf(logFile, " ");
    }
  } // for every col in the HEX dump range
 
  // print out the "printable" ascii value of bytes.
  // -----------------------------------------------
  for (col_offset = 0; col_offset < 16; col_offset++) {
    cur_byte = fst_byte + row_offset + col_offset;
    if (((unsigned int) mem_start_ptr <= cur_byte) && (cur_byte <= (unsigned int) mem_end_ptr)) {
      one_char = mem[row_offset + col_offset - col_overflow];
      if (one_char < 32)
        one_char = '.';
      if (126 < one_char)
        one_char = '.';
      printf("%c", one_char);
      if (LOG_TO_FILE)
        fprintf(logFile, "%c", one_char);
    }
    else
    {
      printf(" "); // dump space!
      if (LOG_TO_FILE)
        fprintf(logFile, " ");
    }
  } // for every col in the HEX dump range
  if (LOG_TO_FILE)
    fflush (logFile);
} // hex_dump_row
 
 
/* ======================================================================== */
// PURPOSE:        Perform a hex dump
// Preconditions:  mem_start_ptr, mem_end_ptr = pounsigned inters to the data
//                      range to dump to the screen
// postconditions: Hex dump of the memory range
// COMMENTS:
/* ======================================================================== */
void hexdump(char * mem_start_ptr, char * mem_end_ptr)
{
  unsigned int memlen; // how many bytes are in the range of memory to print
  unsigned int fst_byte; // holds the first byte to print
  unsigned int row_offset; // loop counter for the rows of the hexdump
  unsigned int col_overflow; // how many charcters before we overflow
 
  // prep
  // ----
  printf("Size: [%d]", mem_end_ptr - mem_start_ptr + 1);
  printf("  first_byte=[%02x]", *(unsigned char*) mem_start_ptr);
  printf("  last_byte=[%02x]\n", *(unsigned char*) mem_end_ptr);
  if (LOG_TO_FILE)
  {
    fprintf(logFile, "Size: [%d]", mem_end_ptr - mem_start_ptr + 1);
    fprintf(logFile, "  first_byte=[%02x]", *(unsigned char*) mem_start_ptr);
    fprintf(logFile, "  last_byte=[%02x]\n", *(unsigned char*) mem_end_ptr);
  }
  memlen = (unsigned int) (mem_end_ptr - mem_start_ptr) + 1;
  col_overflow = 0;
  //  there are i.e. use to print partial rows of bytes
  fst_byte = (unsigned int) mem_start_ptr; // FIRST byte to print
 
  // Loop to dump every row of the hex dump
  // --------------------------------------
  for (row_offset = 0; row_offset < memlen + 16; row_offset = row_offset + 16) {   // <<< for each 16byte block of memory
    // see if ANYTHING gets printed at all...
    if ((fst_byte + row_offset) <= (unsigned int) mem_end_ptr)
    {   // print address info
      //printf("%07X", (unsigned int)(fst_byte+row_offset)); //i.e: 0001fe:
      printf("(%03X) ", row_offset);  // i.e: print 0001fe:
      if (LOG_TO_FILE)
        fprintf(logFile, "(%03X) ", row_offset);
      // print out one row of the hexdump
      hex_dump_row(mem_start_ptr, mem_end_ptr, row_offset, fst_byte, col_overflow);
 
      printf("\n"); // advance to next row
      if (LOG_TO_FILE)
        fprintf(logFile, "\n");
    } // anything get printed at all?
  } // loop for every row
  if (LOG_TO_FILE)
    fflush (logFile);
} // hexdump
 
 
/* ======================================================================== */
// PURPOSE: DUMP OUT the memory at the specified pounsigned inter, PLUS info
//        about the the NODE that manages that memory location.
// predonditions: mem_to_dump_ptr = pounsigned inter to the memory node
//                mem_size = how much memory exists at that node
// Postconditions: dumped information
/* ======================================================================== */
void hexdump_by_size(char * mem_to_dump_ptr, unsigned int mem_size)
{
  printf("----------------------------------------------------------------------\n");
  if (LOG_TO_FILE)
    fprintf(logFile, "----------------------------------------------------------------------\n");
  hexdump(mem_to_dump_ptr, mem_to_dump_ptr + mem_size - 1);
  printf("----------------------------------------------------------------------\n");
  if (LOG_TO_FILE)
    fprintf(logFile, "----------------------------------------------------------------------\n");
  if (LOG_TO_FILE)
    fflush (logFile);
} // hexdump_by_size