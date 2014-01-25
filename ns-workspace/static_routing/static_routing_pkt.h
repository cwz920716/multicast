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

#ifndef __static_routing_pkt_h__
#define __static_routing_pkt_h__

#include "packet.h"

#define HDR_STATIC(p)   ((struct hdr_static*)hdr_static::access(p))
#define HDR_STATIC_DATA(p) ((struct hdr_static_data*)hdr_static::access(p))

enum StaticPktType {
	STATIC_PKT_DATA		// data
};

struct hdr_static {
	StaticPktType type_;
	
	static int offset_;
	inline static int& offset() {return offset_;}
	inline static hdr_static* access(const Packet* p) {
		return (hdr_static*) p->access(offset_);
	}
};

struct hdr_static_data {
	StaticPktType type_;				// the type of pkt
	nsaddr_t src_;						// the src addr
	nsaddr_t dest_;						// the dest addr

	inline int size() {
		unsigned int s = 2 * sizeof(int) + 1;
		return s;
	}
};

union hdr_all_static {
	hdr_static staticHdr;
	hdr_static_data staticHdrData;
};

#endif
