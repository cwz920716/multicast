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

#ifndef __greedy_h__
#define __greedy_h__

#include <vector>

#include "greedy_pkt.h"
#include "greedy_nbr.h"
#include "greedy_flow.h"

#include "agent.h"
#include "packet.h"
#include "trace.h"
#include "timer-handler.h"
#include "mobilenode.h"
#include "classifier-port.h"
#include "cmu-trace.h"

class GreedyAgent;

/*
 * This is the Greedy Forwarding agent. 
 */
class GreedyAgent : public Agent {
public:
	GreedyAgent();
	int command(int, const char*const*);
	void recv(Packet*, Handler*);

protected:
	PortClassifier *port_dmux_;				// for passing packets up to agents
	Trace *logtarget_;						// for trace

	// for Greedy routing
	void forwardData(Packet*);				// forward packets
	nsaddr_t getNextNode(Packet*);			// return the next candidate node

private:
	// the node information
	MobileNode *node_;						// the attached mobile node	
	nsaddr_t addr_;							// addr
	double x_;								// the location X
	double y_;								// the location Y
	std::vector< GreedyNbr* > nbr_;			// the neighbors
	std::vector< GreedyFlow* > flow_;		// the flow

	void setLocation();								// set the location of this agent
	bool addGreedyNbr(nsaddr_t, float, float);		// add neighbors

	// for flow data
	void addFlow(nsaddr_t, float, float);		// add flow data
	GreedyFlow* getFlow(nsaddr_t);					// return flow data

	// dump
	void dumpPacket(Packet*);

};

#endif
