#  Copyright (C) 2012 Kazuya Sakai. Allright Received.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License or any
#  later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but without any warranty, including any implied warranty for 
#  merchantability or fitness for a particular purpose. Under no
#  circumstances shall Kazuya Sakai be liable for any use of, misuse of,
#  or inability to use this software, including incidental and 
#  consequential damages.
# 
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, see <http://www.gnu.org/licenses/>
# 
#  Author: Kazuya Sakai, The Ohio State University
#  Date  : Dec. 14, 2012
#  
#  Greedy forwarding code for ns2 version 2.34
# 

# debug
set nam true

# Define options
set val(chan)	Channel/WirelessChannel		;# channel type
set val(prop)	Propagation/TwoRayGround	;# radio-propagation model
set val(ant)	Antenna/OmniAntenna			;# antenna type
set val(ll)		LL							;# link layer type
set val(ifq)	Queue/DropTail/PriQueue		;# interface queue type
set val(ifqlen)	50							;# max packet in ifq
set val(netif)	Phy/WirelessPhy				;# network interface type
set val(mac)	Mac							;# MAC protocol
set val(rp)		STATICR						;# ad-hoc routing protocol
set val(stop)	300.0						;# the time when the simulation stop

# src 
set val(src) topo1.txt
#set val(src) topo2.txt

set val(nn) 7
set val(x) 1000
set val(y) 1000
set val(radius) 250.0
set val(lr) 10

# the output file name
set val(outf) "result"

# create ns instancefor 
set ns_ [new Simulator]
$ns_ use-scheduler Heap
set tracefd [open $val(outf).tr w]
$ns_ trace-all $tracefd
if {$nam == "true"} {
	set namtrace [open $val(outf).nam w]
	$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)
}

# set up topography object
set topo       [new Topography]
$topo load_flatgrid $val(x) $val(y)

create-god $val(nn)

# configure the nodes
$ns_ node-config -adhocRouting $val(rp) \
		 -llType $val(ll) \
		 -macType $val(mac) \
		 -ifqType $val(ifq) \
		 -ifqLen $val(ifqlen) \
		 -antType $val(ant) \
		 -propType $val(prop) \
		 -phyType $val(netif) \
		 -topoInstance $topo \
		 -agentTrace ON \
		 -routerTrace ON \
		 -macTrace ON \
		 -movementTrace OFF \
		 -channel [new $val(chan)]

# setting

# scenario file
source $val(src)
for {set i 0} {$i < $val(nn)} {incr i} {
	$ns_ initial_node_pos $node_($i) 20
}

# end simulation
$ns_ at [expr $val(stop) + 0.101] "stop"
$ns_ at [expr $val(stop) + 0.102] "puts \"NS EXITING...\" ; $ns_ halt"

for {set i 0} {$i < $val(nn) } {incr i} {
	$ns_ at [expr $val(stop) + 0.101] "$node_($i) reset"
}

proc stop {} {
	global tracefd namtrace nam
	close $tracefd
	if {$nam == "true"} {close $namtrace}	

	exit 0
}

puts "Starting Simulation..."
$ns_ run
