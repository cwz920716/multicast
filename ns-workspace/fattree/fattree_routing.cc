/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#include "fattree_routing.h"

using namespace std;

int hdr_fattree::offset_;
std::map< nsaddr_t, FattreeAgent * > FattreeAgent::agent_pool_;
GroupController FattreeAgent::centralGC_;
TrafficController FattreeAgent::centralTC_;

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
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "addr") == 0) {
			addr_ = Address::instance().str2addr(argv[2]);
			locator_ = Locator::addr2Locator(addr_);
			agent_pool_.insert(map< nsaddr_t, FattreeAgent * >::value_type(addr_, this));
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
			post( atoi(argv[2]), atoi(argv[3]) );
			return TCL_OK;
		}
	}

	return Agent::command(argc, argv);
}

void FattreeAgent::recv(Packet* p, Handler* h) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	// struct hdr_ip* ih = HDR_IP(p);
	
	if (hdr->dest_ == addr_) {
		// I am the destination, print the recv message
		fprintf(stderr, "node %d received %d bytes at %f.\n", addr_, hdr->content_size_, 
					(Scheduler::instance().clock()) * 1000);
		Packet::free(p);
		return; 
	} else {
		// I am an router agent
		/*
		  fattree routing
		*/
		fprintf(stderr, "node %d route %d bytes at %f.\n", addr_, hdr->content_size_, 
					(Scheduler::instance().clock()) * 1000);

		send2(Locator::getNextHopFor(locator_, hdr->dest_), hdr->content_size_, hdr->src_, hdr->dest_);
		Packet::free(p);
	}

}


void FattreeAgent::dumpPacket(Packet *p) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	fprintf(stderr, "pkt -n %d -n %d -s %d -d %d -h %d",
			 addr_, HDR_CMN(p)->next_hop(), hdr->src_, hdr->dest_, (HDR_CMN(p))->num_forwards());
}

void FattreeAgent::post(nsaddr_t dest, int size) {
	fprintf(stderr, "node %d post %d bytes at %f.\n", addr_, size, 
				(Scheduler::instance().clock()) * 1000);
	send2(Locator::getNextHopFor(locator_, dest), size, addr_, dest);
}

void FattreeAgent::send2(nsaddr_t nexthop, int size, nsaddr_t src, nsaddr_t dest) {
	// Create a new packet
	connect2(nexthop);
	Packet* p = allocpkt();
	// Access the Fattree header for the new packet:
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	hdr->src_ = src;
	hdr->dest_ = dest;
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

int GroupController::indexOfControllers(Locator node) {
	int k = FATTREE_K;
	nsaddr_t edge = Locator::getEdgeHopOf(node);
	return edge - k * k * k / 4;
}

void GroupController::subscribe(Locator node, nsaddr_t group) {
	if (inGroup(node, group))
		return;

	std::map< nsaddr_t, std::list<nsaddr_t> > c = gcs_[indexOfControllers(node)];
	if (c[group].empty()) {					// first node subscribe to this group in its edge
		gcs_[CINDEX][group].push_back(getEdgeHopOf(node));
	}
	c[group].push_back(node);
	
}

void GroupController::unsubscribe(Locator node, nsaddr_t group) {
	if (!inGroup(node, group))
		return;

	std::map< nsaddr_t, std::list<nsaddr_t> > c = gcs_[indexOfControllers(node)];
	c[group].remove(node);
	if (c[group].empty()) {					// last node unsubscribe to this group in its edge
		gcs_[CINDEX][group].remove(getEdgeHopOf(node));
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

nsaddr_t Locator::getNextHopFor(Locator cur_loc, nsaddr_t dest) {
	Locator dest_loc = addr2Locator(dest);
	unsigned char k = FATTREE_K;
	nsaddr_t nexthop = -1;

	if (cur_loc.isHost()) {
		Locator edge = cur_loc;
		edge.host = 0;
		nexthop = locator2Addr(edge);
	} else if (cur_loc.isEdge()) {
		if (dest_loc.cpod == cur_loc.cpod && dest_loc.edge == cur_loc.edge) {
			nexthop = dest;
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

nsaddr_t Locator::getEdgeHopOf(Locator cur_loc) {
	unsigned char k = FATTREE_K;
	nsaddr_t edgehop = -1;

	if (cur_loc.isHost()) {
		Locator edge = cur_loc;
		edge.host = 0;
		edgehop = locator2Addr(edge);
	} else {
		fprintf(stderr, "no supported getEdgeHopOf() on non-host node %d.\n", locator2Addr(cur_loc));
	}

	return edgehop;
}
