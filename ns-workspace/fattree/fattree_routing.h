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
		return host != 0 && edge != 0 && aggr == 0 && cpod != 0;
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


/*
 * This is the fattree routing agent. 
 */
class FattreeAgent : public Agent {
public:
	FattreeAgent();
	int command(int, const char*const*);
	void recv(Packet*, Handler*);
	inline void connect2(nsaddr_t nexthop) { 
		dst_.addr_ = nexthop; 
		dst_.port_ = 0;
	}

protected:
	Trace *logtarget_;						// for trace

private:
	// the node information	
	nsaddr_t addr_;							// addr
	Locator locator_;						// locator of this node based on the addr
	static std::map< nsaddr_t, FattreeAgent * > agent_pool_;	// agent pool for lookup, not useful for now

	void dumpPacket(Packet*);

	void post(nsaddr_t, int);

	nsaddr_t getNextHopFor(nsaddr_t);

	void send2(nsaddr_t, int, nsaddr_t, nsaddr_t);

};

#endif
