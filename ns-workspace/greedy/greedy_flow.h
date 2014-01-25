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
 
#ifndef __greedy_flow_h__
#define __greedy_flow_h__

#include "config.h"

/*
 * This is the flow class for source nodes to learn destination location.
 * This is because that geographical routing protocols assume
 * a source node knows the location of its destination.
 */
class GreedyFlow {
public:
	nsaddr_t dest_;		// dest addr
	float destX_;		// dest location
	float destY_;
	
	GreedyFlow(nsaddr_t, float, float);
};

#endif
