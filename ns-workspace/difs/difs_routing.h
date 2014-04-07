/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#ifndef __difs_routing_h__
#define __difs_routing_h__

#include <map>
#include <list>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>

#include "difs_pkt.h"

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
#define FATTREE_K (4)
#define N_FLOW (FATTREE_K * FATTREE_K * FATTREE_K / 4 + 1)

#define IC3 "$temp install_connection [$cbr%s set fid_] %s %s %s %s %s"
#define IC5 "$temp install_connection [$cbr%s set fid_] %s %s %s %s %s %s %s"
#define IC7 "$temp install_connection [$cbr%s set fid_] %s %s %s %s %s %s %s %s %s"


typedef struct PATH {
	int length;					// src & dst included;
	nsaddr_t hops[7];
	nsaddr_t pair[2];
} path_t;

double delta(double a) {
	return (double)rand() / RAND_MAX * a - a / 2;
}

/*
 * This is the difs locator struct. 
 */
class DiFSLocator {
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

	static DiFSLocator addr2Locator(nsaddr_t addr);
	static nsaddr_t locator2Addr(DiFSLocator l);
	static nsaddr_t getEdgeHopOf(DiFSLocator);
	static nsaddr_t getEdgeHopOf(nsaddr_t);
	static nsaddr_t getCorrAggrIn(nsaddr_t, nsaddr_t);
};


/*
 * This is the difs routing agent. 
 */
class DiFSAgent : public Agent {
public:
	DiFSAgent();
	int command(int, const char*const*);
	void recv(Packet*, Handler*);
	inline void connect2(nsaddr_t nexthop) { 
		daddr() = nexthop; 
		dport() = agent_pool_[nexthop]->here_.port_;
	}

protected:
	Trace *logtarget_;						// for trace

private:
	// the node information	
	nsaddr_t addr_;							// addr
	DiFSLocator locator_;						// locator of this node based on the addr

	// std::map< int, double > access_;		// access time for each flow
	std::map< int, nsaddr_t > income_;		// last hop for each flow
	std::map< int, nsaddr_t > outgo_;		// next hop for each flow
	std::map< nsaddr_t, int > vi_;			// input Path State Vector
	std::map< nsaddr_t, int > vo_;			// output Path State Vector
	std::set<nsaddr_t> uphillNgbs_;			// uphill Neighbours

	static std::map< nsaddr_t, DiFSAgent * > agent_pool_;	// agent pool for lookup
	static path_t flow_path_[N_FLOW];			// the selected path for each flow
	void send2(nsaddr_t nexthop, int fid, nsaddr_t recore, nsaddr_t reaggr);
	void dumpPacket(Packet *p);
	void dumpStates();
	void init(nsaddr_t addr);
	void reflect();
	void sendEAR();
	void addFlow(int fid, nsaddr_t lasthop, nsaddr_t nexthop);
	void removeFlow(int fid);
	nsaddr_t palogic(int fid, nsaddr_t src, nsaddr_t dst);
	void move(int fid, path_t np);
	void exAdapt(int fid, nsaddr_t src, nsaddr_t dst, nsaddr_t recore, nsaddr_t reaggr);
	nsaddr_t pathAlloc(int fid, nsaddr_t src, nsaddr_t dst);
	static void assign(int fid, nsaddr_t src, nsaddr_t dst);
	static void eval_ic(nsaddr_t src, int fid, path_t p);
	static void eval_sched(nsaddr_t addr);

};

#endif
