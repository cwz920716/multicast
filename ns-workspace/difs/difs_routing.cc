/* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
#include "difs_routing.h"

using namespace std;

int hdr_difs::offset_;
std::map< nsaddr_t, DiFSAgent * > DiFSAgent::agent_pool_;
path_t DiFSAgent::flow_path_[N_FLOW];

static class DiFSHeaderClass : public PacketHeaderClass{
public:
	DiFSHeaderClass() : PacketHeaderClass("PacketHeader/DiFS",
										  sizeof(hdr_all_difs)){
										  bind_offset(&hdr_difs::offset_);
	}
} class_difshdr;

static class DiFSAgentClass : public TclClass {
public:
	DiFSAgentClass() : TclClass("Agent/DiFS") {}
	TclObject *create(int argc, const char*const* argv) {
		return (new DiFSAgent());
	}
} class_DiFSAgent;




/*************************************************************
 *
 *DiFS Section
 *
 *************************************************************/

DiFSAgent::DiFSAgent() : Agent(PT_DIFS), addr_(-1) {
	bind("packetSize_", &size_);
}

int DiFSAgent::command(int argc, const char*const* argv) {
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {
			// sched next reflect event
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-port") == 0) {
			fprintf(stderr, "%d \n", here_.port_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-addr") == 0) {
			fprintf(stderr, "%d \n", here_.addr_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-dst-port") == 0) {
			fprintf(stderr, "%d \n", dst_.port_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "show-dst-addr") == 0) {
			fprintf(stderr, "%d \n", dst_.addr_);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "dump-state") == 0) {
			dumpStates();
			return TCL_OK;
		} else if (strcasecmp(argv[1], "reflect") == 0) {
			if (locator_.isHost()) {
				fprintf(stderr, "no supported post on host node %d \n", addr_);
				return TCL_ERROR;
			}
			reflect();
			return TCL_OK;
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "addr") == 0) {
			init(Address::instance().str2addr(argv[2]));
			return TCL_OK;
		} else if (strcmp(argv[1], "remove-flow") == 0) {
			// unimplemented yet
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
		if (strcmp(argv[1], "assign-flow") == 0) {
			int fid = atoi(argv[2]);
			nsaddr_t src = Address::instance().str2addr(argv[3]);
			nsaddr_t dst = Address::instance().str2addr(argv[4]);
			assign(fid, src, dst);
			return TCL_OK;
		}
	}

	return Agent::command(argc, argv);
}

void DiFSAgent::recv(Packet* p, Handler* h) {
	struct hdr_difs_data *hdr = HDR_DIFS_DATA(p);
	int fid = hdr->fid_;
	nsaddr_t src = flow_path_[fid].pair[0];
	nsaddr_t dst = flow_path_[fid].pair[1];
	// struct hdr_ip* ih = HDR_IP(p);

	if (income_[fid] == 0) {
		Packet::free(p);
		return;
	}
	
	if (locator_.isHost()) {
		// host: discard
		Packet::free(p);
	} else if (locator_.isEdge()) {
		// edge: adapt or discard 
		if (hdr->reaggr_ != 0) {
			exAdapt(fid, src, dst, hdr->recore_, hdr->reaggr_);
		}
		
		Packet::free(p);
	} else if (locator_.isAggr()) {
		// aggr: adapt or forward
		if (hdr->recore_ == 0) {
			send2(income_[fid], fid, hdr->recore_, hdr->reaggr_);
		} else {
			exAdapt(fid, src, dst, hdr->recore_, hdr->reaggr_);
		}
		
		Packet::free(p);
	} else if (locator_.isCore()) {
		// core: SISO for all flow, forward this to the incoming port
		send2(income_[fid], fid, hdr->recore_, hdr->reaggr_);
		Packet::free(p);
	}

}

void DiFSAgent::dumpPacket(Packet *p) {
	struct hdr_difs_data *hdr = HDR_DIFS_DATA(p);
	fprintf(stderr, "pkt -n %d -n %d -s %d -d %d -h %d",
			 addr_, HDR_CMN(p)->next_hop(), hdr->fid_, hdr->recore_, (HDR_CMN(p))->num_forwards());
}

void DiFSAgent::dumpStates() { 
}

void DiFSAgent::init(nsaddr_t addr) { 
	addr_ = addr;
	locator_ = DiFSLocator::addr2Locator(addr_);
	agent_pool_.insert(map< nsaddr_t, DiFSAgent * >::value_type(addr_, this));

	int k = FATTREE_K;
	DiFSLocator l;
	if (locator_.isAggr()) {
		l.aggr = l.edge = l.host = 0;
		int base = (locator_.aggr - 1) * (k/2);
		for (int i = 1; i <= k/2; i++) {
			l.cpod = base + i;
			uphillNgbs_.insert(DiFSLocator::locator2Addr(l));		
		}
	} else if (locator_.isEdge()) {
		l.cpod = locator_.cpod;
		l.edge = l.host = 0;
		for (int i = 1; i <= k/2; i++) {
			l.aggr = i;
			uphillNgbs_.insert(DiFSLocator::locator2Addr(l));		
		}
	}

	// Tcl schedule another reflect in future
	eval_sched(addr_);
}

void DiFSAgent::reflect() { 
	// remove flows: unneccssary
	// rebalance: unimplemented yet
	// imbalance detection
	fprintf(stderr, "reflect");
	sendEAR();

	// Tcl schedule another reflect in future
	eval_sched(addr_);
}

void DiFSAgent::sendEAR() { 
	std::vector<int> S;
	int min = -1;
	for (std::set<nsaddr_t>::const_iterator i = uphillNgbs_.begin(); i != uphillNgbs_.end(); ++i) {
		if (min < 0 || vi_[*i] < min)
			min = vi_[*i];
	}

	for(std::map< int, nsaddr_t >::iterator i = income_.begin(); i != income_.end(); ++i) {
		if (vi_[ income_[i->first] ] - min > 1) {
			S.push_back(i->first);
		}	
	}

	int fid = S[rand() % S.size()];
	nsaddr_t recore = 0, reaggr = 0;
	if (locator_.isAggr()) {
		recore = income_[fid];
	} else if (locator_.isEdge()) {
		reaggr = income_[fid];
	}
	send2(income_[fid], fid, recore, reaggr);
}

void DiFSAgent::addFlow(int fid, nsaddr_t lasthop, nsaddr_t nexthop) {
	if (locator_.isHost())
		return;

	income_[fid] = lasthop;
	outgo_[fid] = nexthop;
	vi_[lasthop]++;
	vo_[nexthop]++;
}

void DiFSAgent::removeFlow(int fid) {
	if (locator_.isHost() || income_[fid] == 0)
		return;

	nsaddr_t lasthop = income_[fid];
	nsaddr_t nexthop = outgo_[fid];
	vi_[lasthop]--;
	vo_[nexthop]--;
	income_[fid] = 0;
	outgo_[fid] = 0;
}

nsaddr_t DiFSAgent::palogic(int fid, nsaddr_t src, nsaddr_t dst) {		
	std::vector<nsaddr_t> S;
	int min = -1;
	for (std::set<nsaddr_t>::const_iterator i = uphillNgbs_.begin(); i != uphillNgbs_.end(); ++i) {
		if (min < 0 || vo_[*i] < min)
			min = vo_[*i];
	} 
	
	for (std::set<nsaddr_t>::const_iterator i = uphillNgbs_.begin(); i != uphillNgbs_.end(); ++i) {
		if (vo_[*i] == min)
			S.push_back(*i);
	} 

	int r = rand() % S.size();
	return S[r];
}

void DiFSAgent::move(int fid, path_t np) {
	if (flow_path_[fid].length == np.length) {
		for (int i = 0; i < np.length; i++) {
			agent_pool_[flow_path_[fid].hops[i]]->removeFlow(fid);
		}
		for (int i = 1; i < np.length - 1; i++) {
			agent_pool_[np.hops[i]]->addFlow(fid, np.hops[i - 1], np.hops[i + 1]);
		}
		flow_path_[fid] = np;

		// eval tcl script to establish connection
		eval_ic(np.pair[0], fid, np);
	} else {
	}
}	

void DiFSAgent::exAdapt(int fid, nsaddr_t src, nsaddr_t dst, nsaddr_t recore, nsaddr_t reaggr) {
	DiFSLocator src_loc = DiFSLocator::addr2Locator(src);
	nsaddr_t next;
	std::vector<nsaddr_t> S;
	path_t np, np2;
	int f2;

	if (recore == 0 && reaggr != 0) {
		// recommend a aggr
		if (locator_.isEdge() && locator_.cpod == src_loc.cpod && locator_.edge == src_loc.edge) {
			np = flow_path_[fid];
			// pre-cond: np.length == 5
			np.hops[2] = reaggr;
			if (flow_path_[fid].length == 7) {
				np.hops[3] = agent_pool_[np.hops[2]]->pathAlloc(fid, np.pair[0], np.pair[1]);
				np.hops[4] = DiFSLocator::getCorrAggrIn(np.hops[2], DiFSLocator::addr2Locator(np.pair[1]).cpod);
			}
			next = reaggr;
			// pre-cond: recore in uphillNgbs_
			if (vo_[next] > vo_[outgo_[fid]]) {
				for(std::map< int, nsaddr_t >::iterator i = outgo_.begin(); i != outgo_.end(); ++i) {
					if (outgo_[i->first] == next) {
						S.push_back(i->first);
					}	
				}
				f2 = S[rand() % S.size()];
				np2 = flow_path_[f2];
				// pre-cond: np2.length == ?(5, 7)
				np2.hops[2] = outgo_[fid];
				if (flow_path_[f2].length == 7) {
					np2.hops[3] = agent_pool_[np2.hops[2]]->pathAlloc(f2, np2.pair[0], np2.pair[1]);
					np2.hops[4] = DiFSLocator::getCorrAggrIn(np2.hops[2], DiFSLocator::addr2Locator(np2.pair[1]).cpod);
				}
				move(f2, np2);
			}
			move(fid, np);
		}
		
	} else if (reaggr == 0 && recore != 0) {
		// recommend a core
		// pre-cond: pod(src) <> pod(dst)
		if (locator_.isAggr() && locator_.cpod == src_loc.cpod) {
			// compute a new path for fid
			np = flow_path_[fid];
			// pre-cond: np.length == 7
			np.hops[3] = recore;
			next = recore;
			// pre-cond: recore in uphillNgbs_
			if (vo_[next] > vo_[outgo_[fid]]) {
				for(std::map< int, nsaddr_t >::iterator i = outgo_.begin(); i != outgo_.end(); ++i) {
					if (outgo_[i->first] == next) {
						S.push_back(i->first);
					}	
				}
				f2 = S[rand() % S.size()];
				np2 = flow_path_[f2];
				// pre-cond: pod(np2.src) <> pod(np2.dst)
				// pre-cond: np2.length == 7
				np2.hops[3] = outgo_[fid];
				move(f2, np2);
			}
			move(fid, np);
		}
	}
}

nsaddr_t DiFSAgent::pathAlloc(int fid, nsaddr_t src, nsaddr_t dst) {
	DiFSLocator dst_loc = DiFSLocator::addr2Locator(dst);
 	unsigned char k = FATTREE_K;

	if (locator_.isHost()) {
		if (addr_ == dst)
			return 0;
		else if (addr_ == src)
			return DiFSLocator::getEdgeHopOf(src);
	} else if (locator_.isCore()) {
		DiFSLocator aggr = locator_;
		aggr.cpod = dst_loc.cpod;
		aggr.aggr = (locator_.cpod - 1) / (k / 2) + 1;											// for default fat tree
		return DiFSLocator::locator2Addr(aggr);
	} else if (locator_.isAggr()) {
		if (dst_loc.cpod == locator_.cpod) {
			return DiFSLocator::getEdgeHopOf(dst);
		} else {
			// path allocation logic	
			return palogic(fid, src, dst);
		}
	} else if (locator_.isEdge()) {
		if (dst_loc.cpod == locator_.cpod && dst_loc.edge == locator_.edge) {
			return dst;
		} else {
			// path allocation logic
			return palogic(fid, src, dst);
		}
	}

	// no match; shouldn't happen 
	return 0;
}

void DiFSAgent::eval_ic(nsaddr_t src, int fid, path_t np) {
	Tcl& tcl = Tcl::instance();
	tcl.evalf ("set temp [$n%s set src_agent_]", src);
	if (np.length == 3)
		tcl.evalf (IC3, fid, src, np.pair[1], np.hops[0], np.hops[1], np.hops[2]);
	else if (np.length == 5)
		tcl.evalf (IC5, fid, src, np.pair[1], np.hops[0], np.hops[1], np.hops[2], np.hops[3], np.hops[4]);
	else if (np.length == 7)
		tcl.evalf (IC7, fid, src, np.pair[1], np.hops[0], np.hops[1], np.hops[2], np.hops[3], np.hops[4], np.hops[5], np.hops[6]);
}

void DiFSAgent::eval_sched(nsaddr_t addr) {
	Tcl& tcl = Tcl::instance();
	tcl.evalf ("set ns [Simulator instance]");
	tcl.evalf ("ns at %lf \"n%s reflect\"", Scheduler::instance().clock() * 1000 + 0.1 + delta(0.05), addr);
}

void DiFSAgent::assign(int fid, nsaddr_t src, nsaddr_t dst) { 
	path_t p;
	p.pair[0] = src;
	p.pair[1] = dst;
	nsaddr_t cur = src, next = 0, prev = 0;
	int len = 0;

	if (src == dst)
		return;

	while (cur != 0) {
		p.hops[len] = cur;
		len++;
		next = agent_pool_[cur]->pathAlloc(fid, src, dst);

		// update switch state
		agent_pool_[cur]->addFlow(fid, prev, next);
		prev = cur;
		cur = next;
	}
	p.length = len;

	// eval tcl script to establish connection
	eval_ic(src, fid, p);
}

void DiFSAgent::send2(nsaddr_t nexthop, int fid, nsaddr_t recore, nsaddr_t reaggr) {
	// Create a new packet
	connect2(nexthop);
	Packet* p = allocpkt();
	// Access the Fattree header for the new packet:
	struct hdr_difs_data *hdr = HDR_DIFS_DATA(p);
	hdr->fid_ = fid;
	hdr->recore_ = recore;
	hdr->reaggr_ = reaggr;
	hdr->nexthop_ = nexthop;
	
	send(p, 0);
}

/*************************************************************
 *
 *DiFSLocator Section
 *
 *************************************************************/

DiFSLocator DiFSLocator::addr2Locator(nsaddr_t addr) {
	DiFSLocator l;
 	unsigned char k = FATTREE_K;
	if (1 <= addr && addr <= k * k * k / 4) {
		// host addr
		l.aggr = 0;
		l.cpod = (addr - 1) / (k * k / 4) + 1;
		l.edge = (addr - (l.cpod - 1) * (k * k / 4) - 1) / (k / 2) + 1;
		l.host = (addr - 1) % (k / 2) + 1; 
	} else if (k * k * k / 4 + 1 <= addr && addr <= k * k * k / 4 + k * k / 2) {
		// edge addr
		nsaddr_t offset = k * k * k / 4 + 1;
		l.aggr = 0;
		l.host = 0;
		l.cpod = (addr - offset) / (k / 2) + 1;
		l.edge = (addr - offset) % (k / 2) + 1;
	} else if (k * k * k / 4 + k * k / 2 + 1 <= addr && addr <= k * k * k / 4 + k * k) {
		// aggr addr
		nsaddr_t offset = k * k * k / 4 + k * k / 2 + 1;
		l.edge = 0;
		l.host = 0;
		l.cpod = (addr - offset) / (k / 2) + 1;
		l.aggr = (addr - offset) % (k / 2) + 1;
	} else if (k * k * k / 4 + k * k + 1 <= addr && addr <= k * k * k / 4 + k * k + k * k / 4) {
		// core addr
		nsaddr_t offset = k * k * k / 4 + k * k;
		l.edge = 0;
		l.host = 0;
		l.cpod = addr - offset;
		l.aggr = 0;
	}

	return l;
}

nsaddr_t DiFSLocator::locator2Addr(DiFSLocator l) {
	nsaddr_t addr = 0;
 	unsigned char k = FATTREE_K;
	if (l.isHost()) {
		addr = (l.cpod - 1) * (k * k / 4) + (l.edge - 1) * (k / 2) + l.host;
	} else if (l.isEdge()) {
		nsaddr_t base = k * k * k / 4;
		addr = (l.cpod - 1) * (k / 2) + l.edge + base;
	} else if (l.isAggr()) {
		nsaddr_t base = k * k * k / 4 + k * k / 2;
		addr = (l.cpod - 1) * (k / 2) + l.aggr + base;
	} else if (l.isCore()) {
		nsaddr_t base = k * k * k / 4 + k * k;
		addr = l.cpod + base;
	}

	return addr;
}

nsaddr_t DiFSLocator::getCorrAggrIn(nsaddr_t aggr, nsaddr_t cpod) {
	DiFSLocator l = addr2Locator(aggr);
	l.cpod = cpod;
	return locator2Addr(l);
}

nsaddr_t DiFSLocator::getEdgeHopOf(nsaddr_t host) {
	return getEdgeHopOf(addr2Locator(host));
}

nsaddr_t DiFSLocator::getEdgeHopOf(DiFSLocator host_loc) {
	nsaddr_t edgehop = -1;

	if (host_loc.isHost()) {
		DiFSLocator edge = host_loc;
		edge.host = 0;
		edgehop = locator2Addr(edge);
	} else {
		fprintf(stderr, "no supported getEdgeHopOf() on non-host node %d.\n", locator2Addr(host_loc));
	}

	return edgehop;
}
