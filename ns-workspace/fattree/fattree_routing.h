/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#ifndef __fattree_routing_h__
#define __fattree_routing_h__

#include <map>
#include <list>
#include <iostream>
#include <algorithm>

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
	static nsaddr_t getNextHopFor(Locator, nsaddr_t);
	static nsaddr_t getEdgeHopOf(Locator);
};

/*
 * This is the multicast state. 
 */
class MState {
public:
	friend class FattreeAgent;

	MState::MState() : states_(), len_(0) {}
	inline void len() { return len_; }
	inline void clearAll() {  states_.clear(); len_ = 0; }
	inline void push2(nsaddr_t group, nsaddr_t porta) { states_[group].push_back(porta); len_++; }
	
	std::map< nsaddr_t, std::list<nsaddr_t> > states_;				/* multicast state for a given group */
	int len_;
	static const int CAPACITY;
};


/*
 * This is the membership controller. 
 */
class GroupController {
public:
	friend class FattreeAgent;

	void subscribe(Locator node, nsaddr_t group);
	inline void unsubscribe(Locator node, nsaddr_t group);

	inline bool inGroup(Locator node, nsaddr_t group) {
		std::map< nsaddr_t, std::list<nsaddr_t> > c = gcs_[indexOfControllers(node)];
		return c.find(group) != c.end() && 
					std::find(c[group].begin(), c[group].end(), node) != c[group].end();
	}

	/*
	 * For edges, the members are host nodes, it's index is addr - k * k * k / 4
	 * For central, the members are edge nodes, it's index is 0
	 * For others, it's undefined and shouldn't be accessed
	 */
	std::map< nsaddr_t, std::list<nsaddr_t> > gcs_[FATTREE_K * FATTREE_K / 2 + 1];

	static int indexOfControllers(Locator node);
	static const int CINDEX;									// Central index in gcs_

};

/*
 * This is the traffic controller. 
 */
class TrafficController {
public:
	friend class FattreeAgent;

	void posted(nsaddr_t group, int msg_len) { tfcmtx_[group] += msg_len; }
	void clear(nsaddr_t group) { tfcmtx_[group] = 0; }
	
	std::map< nsaddr_t, nsaddr_t > tfcmtx_;

};

class hashPIM {
public:
	static nsaddr_t chash(nsaddr_t group);							/* get the hashed core address for the group */
	static void buildSharedTree(nsaddr_t group);					
}

/*
 * This is the fattree routing agent. 
 */
class FattreeAgent : public Agent {
public:
	friend class hashPIM;

	FattreeAgent();
	int command(int, const char*const*);
	void recv(Packet*, Handler*);
	inline void connect2(nsaddr_t nexthop) { 
		dst_.addr_ = nexthop; 
		dst_.port_ = 0;
	}

	static GroupController centralGC_;
	static TrafficController centralTC_;

protected:
	Trace *logtarget_;						// for trace

private:
	// the node information	
	nsaddr_t addr_;							// addr
	Locator locator_;						// locator of this node based on the addr
	MState mstates_;							// multicast state of this node
	GroupController gc_;					// group controller

	static std::map< nsaddr_t, FattreeAgent * > agent_pool_;	// agent pool for lookup, not useful for now
	void dumpPacket(Packet*);
	void post(nsaddr_t, int);
	void send2(nsaddr_t, int, nsaddr_t, nsaddr_t);

};

#endif
