#ifndef __fattree_routing_h__
#define __fattree_routing_h__

#include <map>

#include "fattree_pkt.h"

#include "agent.h"
#include "packet.h"
#include "trace.h"
#include "timer-handler.h"
#include "mobilenode.h"
#include "classifier-port.h"
#include "cmu-trace.h"

/*
 * This is the fattree central information.
 * the nsaddr to Locator map
 * rule:
 * node 1 - k * k * k / 4 host
 * node k * k * k / 4 + [1 - k * k / 2] edge
 * node k * k * k / 4 + k * k / 2 + [1 - k * k / 2] aggr
 * node k * k * k / 4 + k * k + [1 - k * k / 4] core
 */
// std::map< nsaddr_t, nsaddr_t > rtable_;	// routing table <dest, next> pair
#define FATTREE_K 4


/*
 * This is the fattree locator struct. 
 */
struct Locator {
	unsigned char host;
	unsigned char edge;
	unsigned char aggr;
	unsigned char cpod;			// core or pod
};

Locator addr2Locator(nsaddr_t addr) {
	Locator l;
 	unsigned char k = FATTREE_K;
	if (1 <= addr && addr <= k * k *k / 4) {
		// host addr
		l.aggr = 0;
		l.cpod = (addr - 1) / (k * k / 4) + 1;
		l.edge = (addr - (l.cpod - 1) * (k * k / 4) - 1) / (k / 2) + 1;
		l.host = (addr - 1) / (k / 2) + 1; 
	} else if (k * k *k / 4 + 1 <= addr && addr <= k * k *k / 4 + k * k / 2) {
		// edge addr
	} else if (k * k *k / 4 + k * k / 2 + 1 <= addr && addr <= k * k * k / 4 + k * k) {
		// aggr addr
	} else if (k * k * k / 4 + k * k + 1 <= addr && addr <= k * k * k / 4 + k * k + k * k / 4) {
		// core addr
	}

	return l;
}


/*
 * This is the fattree routing agent. 
 */
class FattreeAgent : public Agent {
public:
	FattreeAgent();
	int command(int, const char*const*);
	void recv(Packet*, Handler*);

protected:
	PortClassifier *port_dmux_;				// for passing packets up to agents
	Trace *logtarget_;						// for trace

	// forward a packet toward the destination
	void forwardData(Packet*);				// forward packets

private:
	// the node information	
	nsaddr_t addr_;							// addr
	Locator locator_;						// locator of this node based on the addr

	// dump
	void dumpPacket(Packet*);

};

#endif
