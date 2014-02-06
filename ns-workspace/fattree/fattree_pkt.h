/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#ifndef __fattree_pkt_h__
#define __fattree_pkt_h__

#include "packet.h"

#define HDR_FATTREE(p)   ((struct hdr_fattree*)hdr_fattree::access(p))
#define HDR_FATTREE_DATA(p) ((struct hdr_fattree_data*)hdr_fattree::access(p))

enum FattreePktType {
	FATTREE_PKT_DATA		// data
};

struct hdr_fattree {
	FattreePktType type_;
	
	static int offset_;
	inline static int& offset() {return offset_;}
	inline static hdr_fattree* access(const Packet* p) {
		return (hdr_fattree*) p->access(offset_);
	}
};

struct hdr_fattree_data {
// App Layer
	FattreePktType type_;					// the type of pkt
	nsaddr_t source_;						// the source addr
	nsaddr_t group_;						// the group addr
// GRE/Tunnel Layer
	bool tunnelFg_;							// the tunnel Flag	
	nsaddr_t tsrc_;							// the tunnel source
	nsaddr_t tdest_;						// the tunnel destination
// Routing Layer
	nsaddr_t lasthop_;						// the last hop for this packet
	nsaddr_t nexthop_;						// the next hop for this packet
	int content_size_;						// the content size
};

union hdr_all_fattree {
	hdr_fattree fattreeHdr;
	hdr_fattree_data fattreeHdrData;
};

#endif
