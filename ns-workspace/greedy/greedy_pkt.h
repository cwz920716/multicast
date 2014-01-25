/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
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

#ifndef __greedy_pkt_h__
#define __greedy_pkt_h__

#include "packet.h"

#define HDR_GREEDY(p)   ((struct hdr_greedy*)hdr_greedy::access(p))
#define HDR_GREEDY_DATA(p) ((struct hdr_greedy_data*)hdr_greedy::access(p))

enum GreedyPktType {
	GREEDY_PKT_DATA		// data
};

struct hdr_greedy {
	GreedyPktType type_;
	
	static int offset_;
	inline static int& offset() {return offset_;}
	inline static hdr_greedy* access(const Packet* p) {
		return (hdr_greedy*) p->access(offset_);
	}
};

struct hdr_greedy_data {
	GreedyPktType type_;				// the type of pkt
	nsaddr_t src_;						// the src addr
	nsaddr_t dest_;						// the dest addr
	float destX_;						// the location X of the dest
	float destY_;						// the location Y of the dest

	inline int size() {
		unsigned int s = 2 * sizeof(int) + 1 + 2 * sizeof(float);
		return s;
	}
};

union hdr_all_greedy {
	hdr_greedy greedyHdr;
	hdr_greedy_data greedyHdrData;
};

#endif
