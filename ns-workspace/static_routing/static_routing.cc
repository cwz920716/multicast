/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
/* 
 * Copyright (C) 2012 Kazuya Sakai, Allright Received.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty, including any implied warranty for 
 * merchantability or fitness for a particular purpose. Under no
 * circumstances shall Kazuya Sakai be liable for any use of, 
 * misuse of, or inability to use this software, including incidental 
 * and consequential damages.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 * Author: Kazuya Sakai, The Ohio State University
 * Date  : Dec. 14, 2012
 * 
 * Greedy forwarding for ns2 version 2.34
 */
 
#include <map>

#include "static_routing.h"

using namespace std;

int hdr_static::offset_;

static class StaticRHeaderClass : public PacketHeaderClass{
public:
	StaticRHeaderClass() : PacketHeaderClass("PacketHeader/StaticR",
										  sizeof(hdr_all_static)){
										  bind_offset(&hdr_static::offset_);
	}
} class_greedyhdr;

static class StaticRAgentClass : public TclClass {
public:
	StaticRAgentClass() : TclClass("Agent/StaticR") {}
	TclObject *create(int argc, const char*const* argv) {
		return (new StaticRAgent());
	}
} class_StaticRAgent;

StaticRAgent::StaticRAgent() : Agent(PT_STATICR), addr_(-1) {

}

int StaticRAgent::command(int argc, const char*const* argv) {
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
		} else if (strcmp(argv[1], "node") == 0) {
			node_ = (MobileNode*)TclObject::lookup(argv[2]);
			if (node_ == 0) {
				fprintf(stderr,  "%s: %s lookup of %s failed \n", __FILE__, argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		} else if (strcmp(argv[1], "addr") == 0) {
			addr_ = Address::instance().str2addr(argv[2]);
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
		if (strcmp(argv[1], "set-rtable-entry") == 0) {
			addRTableEntry(atoi(argv[2]), atoi(argv[3]));
			return TCL_OK;
		}
	}
	return Agent::command(argc, argv);
}

void StaticRAgent::recv(Packet* p, Handler* h) {
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	
	if (ih->saddr() == addr_) {
		// if there exists a loop and the packet is not DATA packet, drops the packet
		if (ch->num_forwards() > 0 && ch->ptype() == PT_STATICR) {
			// loop
			drop(p, DROP_RTR_ROUTE_LOOP);
			return;	
		} else if (ch->num_forwards() == 0) {
			struct hdr_static_data *hdr = HDR_STATIC_DATA(p);
		
			// else if this is a packet this node is originating
			// in reality, geographical routing does not use IP, so we do not add IP header
			ch->size() += hdr->size();
			
			hdr->type_ = STATIC_PKT_DATA;
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

void StaticRAgent::forwardData(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);

	if (ch->direction() == hdr_cmn::UP &&
		((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == addr_)) {
		fprintf(stderr, "node %d received\n", addr_);
		port_dmux_->recv(p, 0);
		return;
	} else {
		/*
		  static routing
		*/
		struct hdr_static_data *hdr = HDR_STATIC_DATA(p);
		if (rtable_.find(hdr->dest_) == rtable_.end()) {
			// there is no entry for the destination.
			drop(p, 0);
			fprintf(stderr, "Error: node %d does not have an entry for the destination %d\n", addr_, hdr->dest_);
		} else {
			
			ch->direction() = hdr_cmn::DOWN;
			ch->addr_type() = NS_AF_INET;
			ch->next_hop_ = rtable_[hdr->dest_];
			ch->last_hop_ = addr_;
			ch->num_forwards_++;

			send(p, 0);
		}
	}
}


bool StaticRAgent::addRTableEntry(nsaddr_t d,nsaddr_t n) {
	rtable_.insert(map< nsaddr_t, nsaddr_t >::value_type(d, n));	

	return true;
}

void StaticRAgent::dumpPacket(Packet *p) {
	struct hdr_static_data *hdr = HDR_STATIC_DATA(p);
	fprintf(stderr, "pkt -n %d -n %d -s %d -d %d -h %d",
			 addr_, HDR_CMN(p)->next_hop(), hdr->src_, hdr->dest_, (HDR_CMN(p))->num_forwards());
}
