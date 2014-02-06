/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#include "fattree_routing.h"

using namespace std;

int hdr_fattree::offset_;
std::map< nsaddr_t, FattreeAgent * > FattreeAgent::agent_pool_;
GroupController FattreeAgent::centralGC_;

static class FattreeHeaderClass : public PacketHeaderClass{
public:
	FattreeHeaderClass() : PacketHeaderClass("PacketHeader/Fattree",
										  sizeof(hdr_all_fattree)){
										  bind_offset(&hdr_fattree::offset_);
	}
} class_fattreehdr;

static class FattreeAgentClass : public TclClass {
public:
	FattreeAgentClass() : TclClass("Agent/Fattree") {}
	TclObject *create(int argc, const char*const* argv) {
		return (new FattreeAgent());
	}
} class_FattreeAgent;


/*************************************************************
 *
 *MState Section
 *
 *************************************************************/
const int MState::CAPACITY = 1000;


/*************************************************************
 *
 *Fattree Section
 *
 *************************************************************/

FattreeAgent::FattreeAgent() : Agent(PT_FATTREE), addr_(-1) {

}

int FattreeAgent::command(int argc, const char*const* argv) {
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-port") == 0) {
			fprintf(stderr, "%d \n", here_.port_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-addr") == 0) {
			fprintf(stderr, "%d \n", here_.addr_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-dst-port") == 0) {
			fprintf(stderr, "%d \n", dst_.port_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-dst-addr") == 0) {
			fprintf(stderr, "%d \n", dst_.addr_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "dump-mcast") == 0) {
			dumpMcastStates();
			return TCL_OK;
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "addr") == 0) {
			addr_ = Address::instance().str2addr(argv[2]);
			locator_ = Locator::addr2Locator(addr_);
			agent_pool_.insert(map< nsaddr_t, FattreeAgent * >::value_type(addr_, this));
			return TCL_OK;
		} else if (strcmp(argv[1], "subscribe") == 0) {
			if (!locator_.isHost()) {
				fprintf(stderr, "no supported sub on non-host node %d \n", addr_);
				return TCL_ERROR;
			}
			nsaddr_t group = Address::instance().str2addr(argv[2]);
			centralGC_.subscribe(locator_, group);
			return TCL_OK;
		} else if (strcmp(argv[1], "unsubscribe") == 0) {
			if (!locator_.isHost()) {
				fprintf(stderr, "no supported unsub on non-host node %d \n", addr_);
				return TCL_ERROR;
			}
			nsaddr_t group = Address::instance().str2addr(argv[2]);
			centralGC_.unsubscribe(locator_, group);
			return TCL_OK;
		} else if (strcmp(argv[1], "log-target") == 0 || strcmp(argv[1], "tracetarget") == 0) {
			logtarget_ = (Trace*)TclObject::lookup(argv[2]);
			if (logtarget_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed \n", __FILE__, argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		} 
	} else if (argc == 4) {
		if (strcmp(argv[1], "post") == 0) {
			if (!locator_.isHost()) {
				fprintf(stderr, "no supported post on non-host node %d \n", addr_);
				return TCL_ERROR;
			}
			post( Address::instance().str2addr(argv[2]), Address::instance().str2addr(argv[3]) );
			return TCL_OK;
		}
	}

	return Agent::command(argc, argv);
}

void FattreeAgent::recv(Packet* p, Handler* h) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	// struct hdr_ip* ih = HDR_IP(p);
	
	if (locator_.isHost()) {
		// I am a host, print the recv message
		fprintf(stderr, "node %d received %d bytes at %f.\n", addr_, hdr->content_size_, 
					(Scheduler::instance().clock()) * 1000);
		if (centralGC_.inGroup(locator_, hdr->group_))
			fprintf(stderr, "group msg %d delievered to node %d in fault.\n", addr_, hdr->group_);
		Packet::free(p);
		return; 
	} else if (locator_.isEdge()) {
		// I am an router agent
		/*
		  fattree routingsize, addr_, group
		*/
		fprintf(stderr, "node %d route %d bytes at %f.\n", addr_, hdr->content_size_, 
					(Scheduler::instance().clock()) * 1000);

		nsaddr_t group = hdr->group_;
		if (centralGC_.isElephant(group)) {
			mcast(p);
		} else {
			// multicast -> unicast tunnel
			m2u(p);
		}
		
		Packet::free(p);
	} else {
		// I am an router agent
		/*
		  fattree routingsize, addr_, group
		*/
		fprintf(stderr, "node %d route %d bytes at %f.\n", addr_, hdr->content_size_, 
					(Scheduler::instance().clock()) * 1000);

		nsaddr_t group = hdr->group_;
		if (centralGC_.isElephant(group)) {
			mcast(p);
		} else {
			// unicast a tunneled pkt
			ucast(p);
		}
		
		Packet::free(p);
	}

}

void FattreeAgent::m2u(Packet *p) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	nsaddr_t group = hdr->group_;

	std::map< nsaddr_t, std::list<nsaddr_t> >& c = centralGC_.gcs_[GroupController::indexOfControllersForEdge(addr_)];
	std::list<nsaddr_t>& hostlist = c[group];
	for (std::list<nsaddr_t>::iterator i = hostlist.begin(); i != hostlist.end(); ++i) {
		nsaddr_t nexthop = *i;
		send2(nexthop, hdr->content_size_, hdr->source_, hdr->group_, 
								true, addr_, nexthop);
	}

	if (!locator_.fromBelow(hdr->lasthop_))
		return;

	c = centralGC_.gcs_[GroupController::CINDEX];
	std::list<nsaddr_t>& edgelist = c[group];
	for (std::list<nsaddr_t>::iterator i = edgelist.begin(); i != edgelist.end(); ++i) {
		nsaddr_t tdest = *i;
		if (tdest == addr_)
			continue;

		send2(Locator::getNextHopFor(locator_, tdest), hdr->content_size_, hdr->source_, hdr->group_, 
								true, addr_, tdest);
	}
}

void FattreeAgent::ucast(Packet *p) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);

	if (!hdr->tunnelFg_)
		fprintf(stderr, "ucast a non-tunneled pkt.\n");
	
	send2(Locator::getNextHopFor(locator_, hdr->tdest_), hdr->content_size_, hdr->source_, hdr->group_, 
												hdr->tunnelFg_, hdr->tsrc_, hdr->tdest_);
}

void FattreeAgent::mcast(Packet *p) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	nsaddr_t group = hdr->group_;

	std::list<nsaddr_t>& ports = mstates_.states_[group];
	for (std::list<nsaddr_t>::iterator i = ports.begin(); i != ports.end(); ++i) {
		nsaddr_t nexthop = *i;
		if (nexthop != hdr->lasthop_) {
			send2(nexthop, hdr->content_size_, hdr->source_, hdr->group_);
		}
	}
}

void FattreeAgent::dumpPacket(Packet *p) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	fprintf(stderr, "pkt -n %d -n %d -s %d -d %d -h %d",
			 addr_, HDR_CMN(p)->next_hop(), hdr->source_, hdr->group_, (HDR_CMN(p))->num_forwards());
}

void FattreeAgent::dumpMcastStates() {
	fprintf(stderr, "node -addr %d -len(mcast) %d \n", addr_, mstates_.len());
	for (std::map< nsaddr_t, std::list<nsaddr_t> >::iterator i = mstates_.states_.begin();
			i != mstates_.states_.end(); ++i) {
		fprintf(stderr, "entry -group %d [", (*i).first);
		std::list<nsaddr_t>& ports = (*i).second;
		for (std::list<nsaddr_t>::iterator j = ports.begin(); j != ports.end(); ++j)
			fprintf(stderr, "%d, ", *j);
		fprintf(stderr, "]\n");
	}
}

void FattreeAgent::post(nsaddr_t group, int size) {
	fprintf(stderr, "host %d post %d bytes at %f.\n", addr_, size, 
				(Scheduler::instance().clock()) * 1000);
	send2(Locator::getEdgeHopOf(locator_), size, addr_, group);
}

void FattreeAgent::send2(nsaddr_t nexthop, int size, nsaddr_t source, nsaddr_t group) {
	send2(nexthop, size, source, group, false, -1, -1);
}

void FattreeAgent::send2(nsaddr_t nexthop, int size, nsaddr_t source, nsaddr_t group, bool tunnelFg, nsaddr_t tsrc, nsaddr_t tdest) {
	// Create a new packet
	connect2(nexthop);
	Packet* p = allocpkt();
	// Access the Fattree header for the new packet:
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	hdr->source_ = source;
	hdr->group_ = group;
	hdr->tunnelFg_ = tunnelFg;
	hdr->tsrc_ = tsrc;
	hdr->tdest_ = tdest;
	hdr->nexthop_ = nexthop;
	hdr->lasthop_ = addr_;
	hdr->content_size_ = size;
	
	struct hdr_cmn* ch = HDR_CMN(p);
	ch->size() += hdr->content_size_;
	send(p, 0);
}

/*************************************************************
 *
 *GroupController Section
 *
 *************************************************************/

const int GroupController::CINDEX = 0;
const unsigned long GroupController::THRESHOLD = 0;

int GroupController::indexOfControllers(Locator node) {
	int k = FATTREE_K;
	nsaddr_t edge = -1;
	edge = Locator::getEdgeHopOf(node);
	return edge - k * k * k / 4;
}

int GroupController::indexOfControllersForEdge(nsaddr_t edge) {
	int k = FATTREE_K;
	return edge - k * k * k / 4;
}

void GroupController::subscribe(Locator node, nsaddr_t group) {
	if (inGroup(node, group))
		return;

	nsaddr_t node_addr = Locator::locator2Addr(node);
	std::map< nsaddr_t, std::list<nsaddr_t> >& c = gcs_[indexOfControllers(node)];
	if (c[group].empty()) {					// first node subscribe to this group in its edge
		gcs_[CINDEX][group].push_back(Locator::getEdgeHopOf(node));
	}
	c[group].push_back(node_addr);

	if (isElephant(group)) {
		hashPIM::join(node, group);
	}
}

void GroupController::unsubscribe(Locator node, nsaddr_t group) {
	if (!inGroup(node, group))
		return;

	nsaddr_t node_addr = Locator::locator2Addr(node);
	std::map< nsaddr_t, std::list<nsaddr_t> >& c = gcs_[indexOfControllers(node)];
	c[group].remove(node_addr);
	if (c[group].empty()) {					// last node unsubscribe to this group in its edge
		gcs_[CINDEX][group].remove(Locator::getEdgeHopOf(node));
	}
	
	if (isElephant(group)) {
		hashPIM::leave(node, group);
	}
}

void GroupController::post(nsaddr_t group, int len) {
	if (preElephant(group, len))
		hashPIM::build(group);
	tfcmtx_[group] += len;
}

/*************************************************************
 *
 *hashPIM Section
 *
 *************************************************************/

nsaddr_t hashPIM::chash(nsaddr_t group) {
	int k = FATTREE_K;
	int hashed = (group * 2654435761) % (k * k / 4) + 1;

	return k * k * k / 4 + k * k + hashed;
}

void hashPIM::build(nsaddr_t group) {
	GroupController& gc = FattreeAgent::centralGC_;
	std::list<nsaddr_t>& edgelist = gc.gcs_[GroupController::CINDEX][group];
	for (std::list<nsaddr_t>::const_iterator i = edgelist.begin(); i != edgelist.end(); ++i ) {
		std::list<nsaddr_t>& hostlist = gc.gcs_[GroupController::indexOfControllersForEdge(*i)][group];
		for (std::list<nsaddr_t>::const_iterator j = hostlist.begin(); j != hostlist.end(); ++j ) {
			join(Locator::addr2Locator(*j), group);
		}
	}
}

void hashPIM::join(Locator node, nsaddr_t group) {
	// install mcast entry at the edge
	nsaddr_t host = Locator::locator2Addr(node);
	nsaddr_t edge = Locator::getEdgeHopOf(node);
	nsaddr_t core = chash(group);
	nsaddr_t aggr = Locator::getAggrHopOf(node, core);
	MState& edge_mcast = FattreeAgent::agent_pool_[edge]->mstates_;
	MState& aggr_mcast = FattreeAgent::agent_pool_[aggr]->mstates_;
	MState& core_mcast = FattreeAgent::agent_pool_[core]->mstates_;

	if (edge_mcast.states_[group].empty()) {
		// first entry in the edge
		// push aggr in edge.mcast & push edge in aggr.mcast
		edge_mcast.push2(group, aggr);
		if (aggr_mcast.states_[group].empty()) {
			// first entry in the aggr
			aggr_mcast.push2(group, core);
			core_mcast.push2(group, aggr);
		}
		aggr_mcast.push2(group, edge);
	}
	edge_mcast.push2(group, host);
}

void hashPIM::leave(Locator node, nsaddr_t group) {
	// uninstall mcast entry at the edge
	nsaddr_t host = Locator::locator2Addr(node);
	nsaddr_t edge = Locator::getEdgeHopOf(node);
	nsaddr_t core = chash(group);
	nsaddr_t aggr = Locator::getAggrHopOf(node, core);
	MState& edge_mcast = FattreeAgent::agent_pool_[edge]->mstates_;
	MState& aggr_mcast = FattreeAgent::agent_pool_[aggr]->mstates_;
	MState& core_mcast = FattreeAgent::agent_pool_[core]->mstates_;

	edge_mcast.remove2(group, host);
	if (edge_mcast.states_[group].size() == 1) {
		// last entry in the edge
		// pop aggr in edge.mcast & pop edge in aggr.mcast
		edge_mcast.remove2(group, aggr);
		aggr_mcast.remove2(group, edge);
		if (aggr_mcast.states_[group].size() == 1) {
			// first entry in the aggr
			aggr_mcast.remove2(group, core);
			core_mcast.remove2(group, aggr);
		}
	}
}



/*************************************************************
 *
 *Locator Section
 *
 *************************************************************/

Locator Locator::addr2Locator(nsaddr_t addr) {
	Locator l;
 	unsigned char k = FATTREE_K;
	if (1 <= addr && addr <= k * k * k / 4) {
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
		nsaddr_t offset = k * k * k / 4 + k * k;
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
 * only for unicast tunnel between edges
 */
nsaddr_t Locator::getNextHopFor(Locator cur_loc, nsaddr_t dest) {
	Locator dest_loc = addr2Locator(dest);
	unsigned char k = FATTREE_K;
	nsaddr_t nexthop = -1;

	if (!dest_loc.isEdge() || cur_loc.isHost()) {
		fprintf(stderr, "only for unicast tunnel between edges.\n");
		return -1;
	}

/*	if (cur_loc.isHost()) {
		Locator edge = cur_loc;
		edge.host = 0;
		nexthop = locator2Addr(edge);
	} else
*/ 
	if (cur_loc.isEdge()) {
		if (dest_loc.cpod == cur_loc.cpod && dest_loc.edge == cur_loc.edge) {
			fprintf(stderr, "self loop.\n");
			return -1;
		} else {
			Locator aggr = cur_loc;
			aggr.edge = 0;
			aggr.aggr = rand() % (k / 2) + 1;
			nexthop = locator2Addr(aggr);
		}
	} else if (cur_loc.isAggr()) {
		if (dest_loc.cpod == cur_loc.cpod) {
			Locator edge = dest_loc;
			edge.host = 0;
			nexthop = locator2Addr(edge);
		} else {
			Locator core = cur_loc;
			core.aggr = 0;
			core.cpod = (cur_loc.aggr - 1) * (k / 2) + rand() % (k / 2) + 1;					// for default fat tree
			nexthop = locator2Addr(core);
		}
	} else if (cur_loc.isCore()) {
		Locator aggr = cur_loc;
		aggr.cpod = dest_loc.cpod;
		aggr.aggr = (cur_loc.cpod - 1) / (k / 2) + 1;											// for default fat tree
		nexthop = locator2Addr(aggr);
	}

	return nexthop;
}

nsaddr_t Locator::getEdgeHopOf(Locator host_loc) {
	nsaddr_t edgehop = -1;

	if (host_loc.isHost()) {
		Locator edge = host_loc;
		edge.host = 0;
		edgehop = locator2Addr(edge);
	} else {
		fprintf(stderr, "no supported getEdgeHopOf() on non-host node %d.\n", locator2Addr(host_loc));
	}

	return edgehop;
}

nsaddr_t Locator::getAggrHopOf(Locator host_loc, nsaddr_t core) {
	nsaddr_t aggrhop = -1;
	unsigned char k = FATTREE_K;
	
	if (host_loc.isHost() || host_loc.isEdge()) {
		// for default fat tree
		// unsigned char pod = host_loc.cpod;
		Locator aggr = host_loc;
		aggr.host = aggr.edge = 0;
		aggr.aggr = (core - (k * k * k / 4 + k * k + 1)) / (k / 2) + 1;
		aggrhop = locator2Addr(aggr);
	} else {
		fprintf(stderr, "no supported getAggrHopOf() on non-host(edge) node %d.\n", locator2Addr(host_loc));
	}

	return aggrhop;
}
