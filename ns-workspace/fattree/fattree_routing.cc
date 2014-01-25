/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#include "fattree_routing.h"

using namespace std;

int hdr_fattree::offset_;

static class FattreeHeaderClass : public PacketHeaderClass{
public:
	FattreeHeaderClass() : PacketHeaderClass("PacketHeader/Fattree",
										  sizeof(hdr_all_fattree)){
										  bind_offset(&hdr_fattree::offset_);
	}
} class_fattreehdr;

static class FattreeAgentClass : public TclClass {
public:
	StaticRAgentClass() : TclClass("Agent/Fattree") {}
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
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "port-dmux") == 0) {
			port_dmux_ = (PortClassifier*)TclObject::lookup(argv[2]);
			if (port_dmux_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed \n", __FILE__, argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		} else if (strcmp(argv[1], "addr") == 0) {
			addr_ = Address::instance().str2addr(argv[2]);
			locator_ = Locator::addr2Locator(addr_);
			return TCL_OK;
		} else if (strcmp(argv[1], "log-target") == 0 || strcmp(argv[1], "tracetarget") == 0) {
			logtarget_ = (Trace*)TclObject::lookup(argv[2]);
			if (logtarget_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed \n", __FILE__, argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		} 
	}

	return Agent::command(argc, argv);
}

void FattreeAgent::recv(Packet* p, Handler* h) {
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	
	if (ih->saddr() == addr_) {
		// if there exists a loop and the packet is not DATA packet, drops the packet
		if (ch->num_forwards() > 0 && ch->ptype() == PT_FATTREE) {
			// loop
			drop(p, DROP_RTR_ROUTE_LOOP);
			return;	
		} else if (ch->num_forwards() == 0) {
			struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
		
			// else if this is a packet this node is originating
			// in reality, geographical routing does not use IP, so we do not add IP header
			ch->size() += hdr->size();
			
			hdr->type_ = FATTREE_PKT_DATA;
			hdr->src_ = addr_;
			hdr->dest_ = ih->daddr();
			
			ih->ttl_ = 32; // change ttl, the default ttl is 32
		}
	}

	// handling incoming packet
	if (ih->ttl_ == 0) {
		drop(p, DROP_RTR_TTL);
		return;
	}
	forwardData(p);
}

void FattreeAgent::forwardData(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);

	if (ch->direction() == hdr_cmn::UP &&
		((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == addr_)) {
		fprintf(stderr, "node %d received\n", addr_);
		port_dmux_->recv(p, 0);
		return;
	} else {
		/*
		  fattree routing
		*/
		struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
		Locator dest_loc = Locator::addr2Locator(hdr->dest_);
 		unsigned char k = FATTREE_K;

		if (locator_.isHost()) {
			Locator edge = locator_;
			edge.host = 0;
			ch->next_hop_ = Locator::locator2Addr(edge);
		} else if (locator_.isEdge()) {
			if (dest_loc.cpod == locator_.cpod && dest_loc.edge == locator_.edge) {
				ch->next_hop_ = hdr->dest_;
			} else {
				Locator aggr = locator_;
				aggr.edge = 0;
				aggr.aggr = rand() % (k / 2) + 1;
				ch->next_hop_ = Locator::locator2Addr(aggr);
			}
		} else if (locator_.isAggr()) {
			if (dest_loc.cpod == locator_.cpod) {
				Locator edge = dest_loc;
				edge.host = 0;
				ch->next_hop_ = Locator::locator2Addr(edge);
			} else {
				Locator core = locator_;
				core.aggr = 0;
				core.cpod = (locator_.aggr - 1) * (k / 2) + rand() % (k / 2) + 1;					// for default fat tree
				ch->next_hop_ = Locator::locator2Addr(core);
			}
		} else if (locator_.isCore()) {
			Locator aggr = locator_;
			aggr.cpod = dest_loc.cpod;
			aggr.aggr = (locator_.cpod - 1) / (k / 2) + 1;
			ch->next_hop_ = Locator::locator2Addr(aggr);
		} 

		ch->direction() = hdr_cmn::DOWN;
		ch->addr_type() = NS_AF_INET;
		ch->last_hop_ = addr_;
		ch->num_forwards_++;

		send(p, 0);
	}
}

void FattreeAgent::dumpPacket(Packet *p) {
	struct hdr_fattree_data *hdr = HDR_FATTREE_DATA(p);
	fprintf(stderr, "pkt -n %d -n %d -s %d -d %d -h %d",
			 addr_, HDR_CMN(p)->next_hop(), hdr->src_, hdr->dest_, (HDR_CMN(p))->num_forwards());
}