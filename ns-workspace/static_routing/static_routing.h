//* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
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
 * Greedy forwarding for ns2 version 2.35
 */

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

#ifndef __static_routing_h__
#define __static_routing_h__

#include <map>

#include "static_routing_pkt.h"

#include "agent.h"
#include "packet.h"
#include "trace.h"
#include "timer-handler.h"
#include "mobilenode.h"
#include "classifier-port.h"
#include "cmu-trace.h"

class GreedyAgent;

/*
 * This is the static routing agent. 
 */
class StaticRAgent : public Agent {
public:
	StaticRAgent();
	int command(int, const char*const*);
	void recv(Packet*, Handler*);

protected:
	PortClassifier *port_dmux_;				// for passing packets up to agents
	Trace *logtarget_;						// for trace

	// forward a packet toward the destination
	void forwardData(Packet*);				// forward packets

private:
	// the node information
	MobileNode *node_;						// the attached mobile node	
	nsaddr_t addr_;							// addr
	std::map< nsaddr_t, nsaddr_t > rtable_;	// routing table <dest, next> pair
	bool addRTableEntry(nsaddr_t, nsaddr_t);// add neighbors

	// dump
	void dumpPacket(Packet*);

};

#endif
