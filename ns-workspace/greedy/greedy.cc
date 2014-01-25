//* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
/* 
 * Copyright (C) 2011 Kazuya Sakai, Allright Received.
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
 * 
 * Greedy forwarding for ns2 version 2.34
 */
 
#include <vector>

#include "greedy.h"

using namespace std;

int hdr_greedy::offset_;

static class GreedyHeaderClass : public PacketHeaderClass{
public:
	GreedyHeaderClass() : PacketHeaderClass("PacketHeader/Greedy",
										  sizeof(hdr_all_greedy)){
										  bind_offset(&hdr_greedy::offset_);
	}
} class_greedyhdr;

static class GreedyAgentClass : public TclClass {
public:
	GreedyAgentClass() : TclClass("Agent/Greedy") {}
	TclObject *create(int argc, const char*const* argv) {
		return (new GreedyAgent());
	}
} class_GreedyAgent;

GreedyAgent::GreedyAgent() : Agent(PT_GREEDY), addr_(-1) , x_(0.0), y_(0.0) {

}

int GreedyAgent::command(int argc, const char*const* argv) {
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {
			return TCL_OK;
		} else if (strcasecmp(argv[1], "set-location") == 0){
			setLocation();
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
	} else if (argc == 5) {
		if (strcmp(argv[1], "set-nbr") == 0) {
			addGreedyNbr(Address::instance().str2addr(argv[2]), atof(argv[3]), atof(argv[4]));
			return TCL_OK;
		}
		if (strcmp(argv[1], "set-flow-data") == 0) {
			addFlow(Address::instance().str2addr(argv[2]) , atof(argv[3]), atof(argv[4]));
			return TCL_OK;
		}
	} 
	return Agent::command(argc, argv);
}

void GreedyAgent::recv(Packet* p, Handler* h) {
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	
	if (ih->saddr() == addr_) {
		// if there exists a loop and the packet is not DATA packet, drops the packet
		if (ch->num_forwards() > 0 && ch->ptype() == PT_GREEDY) {
			// loop
			drop(p, DROP_RTR_ROUTE_LOOP);
			return;	
		} else if (ch->num_forwards() == 0) {
			struct hdr_greedy_data *hdr = HDR_GREEDY_DATA(p);
		
			// else if this is a packet this node is originating
			// in reality, geographical routing does not use IP, so we do not add IP header
			ch->size() += hdr->size();
			
			GreedyFlow *td = getFlow(ih->daddr());

			hdr->type_ = GREEDY_PKT_DATA;
			hdr->src_ = addr_;
			hdr->dest_ = ih->daddr();
			hdr->destX_ = td->destX_;
			hdr->destY_ = td->destY_;
			
			ih->ttl_ = 100; // change ttl, the default ttl is 32
		}
	}

	// handling incoming packet
	if (ch->ptype() == PT_GREEDY) {
		struct hdr_greedy* hdr = HDR_GREEDY(p);
		switch (hdr->type_) {
			default:
				printf("Error with Greedy packet type. \n");
				exit(1);
		}
	} else {
		//ih->ttl_--;
		if (ih->ttl_ == 0) {
			drop(p, DROP_RTR_TTL);
			return;
		}
		forwardData(p);
	}
}

void GreedyAgent::forwardData(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);

	if (ch->direction() == hdr_cmn::UP &&
		((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == addr_)) {
		port_dmux_->recv(p, 0);
		return;
	} else {
		/*
		/ geedy fowarding
		*/		
		nsaddr_t target = getNextNode(p);
		if (target == -1) {
			Packet::free(p);
			return;
		}

		// handling forward the packet
		ch->direction() = hdr_cmn::DOWN;
		ch->addr_type() = NS_AF_INET;
		ch->next_hop_ = target;
		ch->last_hop_ = addr_;
		ch->num_forwards_++;
		
		send(p, 0);
	}
}

nsaddr_t GreedyAgent::getNextNode(Packet *p) {
	struct hdr_greedy_data *hdr = HDR_GREEDY_DATA(p);
	
	float minDist = sqrt(pow(hdr->destX_ - x_, 2) + pow(hdr->destY_ - y_, 2));
	nsaddr_t target = -1;
	for (std::vector< GreedyNbr* >::iterator it = nbr_.begin(); it != nbr_.end(); it++) {
		if (hdr->dest_ == (*it)->addr_) return (*it)->addr_;
		float d = sqrt(pow(hdr->destX_ - (*it)->x_, 2) + pow(hdr->destY_ - (*it)->y_, 2));
		if (d < minDist) target = (*it)->addr_;
	}

	return target;
}

void GreedyAgent::setLocation() {
	x_ = (float)node_->X();
	y_ = (float)node_->Y();
}

bool GreedyAgent::addGreedyNbr(nsaddr_t id, float x, float y) {	
	// search exists nbrs
	for (std::vector< GreedyNbr* >::iterator it = nbr_.begin(); it != nbr_.end(); it++) {
		if ((*it)->addr_ == id) {
			(*it)->x_ = x;
			(*it)->y_ = y;
			return false;
		}
	}
	
	// add new nbr
	GreedyNbr *nn = new GreedyNbr(id, x, y);
	nbr_.push_back(nn);
	return true;
}

void GreedyAgent::addFlow(nsaddr_t d, float x, float y) {
	GreedyFlow *t = new GreedyFlow(d, x, y);
	flow_.push_back(t);
}

GreedyFlow* GreedyAgent::getFlow(nsaddr_t d) {
	// return flow data when this code finds destinaiton d.	
	for (std::vector< GreedyFlow* >::iterator it = flow_.begin(); it != flow_.end(); it++) {
		if ((*it)->dest_ == d) return *it;
	}

	// if code reaches here, it is wrong.
	fprintf(stderr, "Invalid flow flow (%d, %d). \n", addr_, d);
	exit(1);
}

void GreedyAgent::dumpPacket(Packet *p) {
	struct hdr_greedy_data *hdr = HDR_GREEDY_DATA(p);
	fprintf(stderr, "pkt -n %d -n %d -s %d -d %d -h %d",
			 addr_, HDR_CMN(p)->next_hop(), hdr->src_, hdr->dest_, (HDR_CMN(p))->num_forwards());
}
