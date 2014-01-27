/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#include "fattree_routing.h"

using namespace std;

int hdr_fattree::offset_;
std::map< nsaddr_t, FattreeAgent * > FattreeAgent::agent_pool_;

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
		
		send_config(getNextHopFor(hdr->dest_), hdr->content_size_);
		Packet* p2 = allocpkt();
		// Copy the Fattree header for the new packet:
		struct hdr_fattree_data *hdr2 = HDR_FATTREE_DATA(p2);
		hdr2->src_ = hdr->src_;
		hdr2->dest_ = hdr->dest_;
		hdr2->content_size_ = hdr->content_size_;
		hdr2->send_time_ = hdr2->send_time_;
		send(p2, 0);
		Packet::free(p);
	}

}


void FattreeAgent::dumpPacket(Packet *p) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	fprintf(stderr, "pkt -n %d -n %d -s %d -d %d -h %d",
			 addr_, HDR_CMN(p)->next_hop(), hdr->src_, hdr->dest_, (HDR_CMN(p))->num_forwards());
}

nsaddr_t FattreeAgent::getNextHopFor(nsaddr_t dest) {
	Locator dest_loc = Locator::addr2Locator(dest);
	unsigned char k = FATTREE_K;
	nsaddr_t nexthop;

/*
	if (locator_.isHost()) {
		Locator edge = locator_;
		edge.host = 0;
		nexthop = Locator::locator2Addr(edge);
	} else if (locator_.isEdge()) {
		if (dest_loc.cpod == locator_.cpod && dest_loc.edge == locator_.edge) {
			nexthop = dest;
		} else {
			Locator aggr = locator_;
			aggr.edge = 0;
			aggr.aggr = rand() % (k / 2) + 1;
			nexthop = Locator::locator2Addr(aggr);
		}
	} else if (locator_.isAggr()) {
		if (dest_loc.cpod == locator_.cpod) {
			Locator edge = dest_loc;
			edge.host = 0;
			nexthop = Locator::locator2Addr(edge);
		} else {
			Locator core = locator_;
			core.aggr = 0;
			core.cpod = (locator_.aggr - 1) * (k / 2) + rand() % (k / 2) + 1;					// for default fat tree
			nexthop = Locator::locator2Addr(core);
		}
	} else if (locator_.isCore()) {
		Locator aggr = locator_;
		aggr.cpod = dest_loc.cpod;
		aggr.aggr = (locator_.cpod - 1) / (k / 2) + 1;
		nexthop = Locator::locator2Addr(aggr);
	} 
*/

	if (locator_.isHost()) {
		nexthop = k + 1;
	} else {
		nexthop = dest;
	}

	return nexthop;
}

void FattreeAgent::post(nsaddr_t dest, int size) {
	// Create a new packet
	send_config(getNextHopFor(dest), size);
	Packet* p = allocpkt();
	// Access the Fattree header for the new packet:
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	hdr->src_ = addr_;
	hdr->dest_ = dest;
	hdr->content_size_ = size;
	hdr->send_time_ = Scheduler::instance().clock();
	send(p, 0);
}
