/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#ifndef __difs_pkt_h__
#define __difs_pkt_h__

#include "packet.h"

#define HDR_DIFS(p)   ((struct hdr_difs*)hdr_difs::access(p))
#define HDR_DIFS_DATA(p) ((struct hdr_difs_data*)hdr_difs::access(p))

enum DiFSPktType {
	DIFS_PKT_DATA		// data
};

struct hdr_difs {
	DiFSPktType type_;
	
	static int offset_;
	inline static int& offset() {return offset_;}
	inline static hdr_difs* access(const Packet* p) {
		return (hdr_difs*) p->access(offset_);
	}
};

struct hdr_difs_data {
	DiFSPktType type_;					// the type of pkt
// Flow info
	int fid_;							// flow id given by user
	nsaddr_t src_;						// the source addr
	nsaddr_t dst_;						// the destination addr
	nsaddr_t sport_;					// the source port
	nsaddr_t dport_;					// the destination port
// Recommendation info
	nsaddr_t recore_;					// the recommended core
	nsaddr_t reaggr_;					// the recommended aggr
// Routing info
	nsaddr_t nexthop_;						// the next hop for this packet
	int content_size_;						// the content size
};

union hdr_all_difs {
	hdr_difs difsHdr;
	hdr_difs_data difsHdrData;
};

#endif
