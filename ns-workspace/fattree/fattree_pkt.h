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
	FattreePktType type_;				// the type of pkt
	nsaddr_t src_;						// the src addr
	nsaddr_t dest_;						// the dest addr

	inline int size() {
		unsigned int s = 2 * sizeof(int) + 1;
		return s;
	}
};

union hdr_all_fattree {
	hdr_fattree fattreeHdr;
	hdr_fattree_data fattreeHdrData;
};

#endif
