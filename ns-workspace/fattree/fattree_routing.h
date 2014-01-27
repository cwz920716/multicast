/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#ifndef __fattree_routing_h__
#define __fattree_routing_h__

#include <map>

#include "fattree_pkt.h"

#include "agent.h"
#include "packet.h"
#include "tclcl.h"
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
class Locator {
public:
	unsigned char host;
	unsigned char edge;
	unsigned char aggr;
	unsigned char cpod;			// core or pod
	inline bool isHost() {
		return host != 0 && edge != 0 && aggr == 0 && cpod != 0 && cpod != 2;
	}
	inline bool isEdge() {
		return host == 0 && edge != 0 && aggr == 0 && cpod != 0;
	}
	inline bool isAggr() {
		return host == 0 && edge == 0 && aggr != 0 && cpod != 0;
	}
	inline bool isCore() {
		return host == 0 && edge == 0 && aggr == 0 && cpod != 0;
	}

	static Locator addr2Locator(nsaddr_t addr);
	static nsaddr_t locator2Addr(Locator l);
};

Locator Locator::addr2Locator(nsaddr_t addr) {
	Locator l;
 	unsigned char k = FATTREE_K;
	if (1 <= addr && addr <= k * k *k / 4) {
		// host addr
		l.aggr = 0;
		l.cpod = (addr - 1) / (k * k / 4) + 1;
		l.edge = (addr - (l.cpod - 1) * (k * k / 4) - 1) / (k / 2) + 1;
		l.host = (addr - 1) / (k / 2) + 1; 
	} else if (k * k * k / 4 + 1 <= addr && addr <= k * k * k / 4 + k * k / 2) {
		// edge addr
		nsaddr_t offset = k * k * k / 4 + 1;
		l.aggr = 0;
		l.host = 0;
		l.cpod = (addr - offset) / (k / 2) + 1;
		l.edge = (addr - offset) % (k / 2) + 1;
	} else if (k * k * k / 4 + k * k / 2 + 1 <= addr && addr <= k * k * k / 4 + k * k) {
		// aggr addr
		nsaddr_t offset = k * k * k / 4 + k * k / 2 + 1;
		l.edge = 0;
		l.host = 0;
		l.cpod = (addr - offset) / (k / 2) + 1;
		l.aggr = (addr - offset) % (k / 2) + 1;
	} else if (k * k * k / 4 + k * k + 1 <= addr && addr <= k * k * k / 4 + k * k + k * k / 4) {
		// core addr
		nsaddr_t offset = k * k * k / 4 + k * k + 1;
		l.edge = 0;
		l.host = 0;
		l.cpod = addr - offset;
		l.aggr = 0;
	}

	return l;
}

nsaddr_t Locator::locator2Addr(Locator l) {
	nsaddr_t addr = 0;
 	unsigned char k = FATTREE_K;
	if (l.isHost()) {
		addr = (l.cpod - 1) * (k * k / 4) + (l.edge - 1) * (k / 2) + l.host;
	} else if (l.isEdge()) {
		nsaddr_t base = k * k * k / 4;
		addr = (l.cpod - 1) * (k / 2) + l.edge + base;
	} else if (l.isAggr()) {
		nsaddr_t base = k * k * k / 4 + k * k / 2;
		addr = (l.cpod - 1) * (k / 2) + l.aggr + base;
	} else if (l.isCore()) {
		nsaddr_t base = k * k * k / 4 + k * k;
		addr = l.cpod + base;
	}

	return addr;
}


/*
 * This is the fattree routing agent. 
 */
class FattreeAgent : public Agent {
public:
	FattreeAgent();
	int command(int, const char*const*);
	void recv(Packet*, Handler*);
	inline void send_config(nsaddr_t nexthop, int size) { 
		dst_.addr_ = nexthop; 
		dst_.port_ = 0;
		size_ = size; 
	}

protected:
	Trace *logtarget_;						// for trace

private:
	// the node information	
	nsaddr_t addr_;							// addr
	Locator locator_;						// locator of this node based on the addr
	static std::map< nsaddr_t, FattreeAgent * > agent_pool_;	// routing table <dest, next> pair

	// dump
	void dumpPacket(Packet*);

	// post
	void post(nsaddr_t, int);

	nsaddr_t getNextHopFor(nsaddr_t);

};

#endif
