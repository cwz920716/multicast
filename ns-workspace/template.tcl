#Create a simulator object
set ns [new Simulator]
set rtProto Static

#Define different colors for data flows
#$ns color 1 Blue
#$ns color 2 Red

#Open the nam trace file
# set nf [open out.nam w]
# $ns namtrace-all $nf

#Define a 'finish' procedure
proc finish {} {
        global ns # nf
        # $ns flush-trace
	#Close the trace file
        # close $nf
	#Execute nam on the trace file
        # exec nam out.nam &
        exit 0
}


# Returns the local link that leads to
# the next hop node with the passed
# node address parameter. If no link to
# the given node exists, the procedure
# returns -1.
Node instproc nexthop2link { nexthop } {
	#$self instvar link_
	set ns_ [Simulator instance]
	foreach {index link} [$ns_ array get link_] {
		set L [split $index :]
		set src [lindex $L 0]
		if {$src == [$self id]} {
			set dst [lindex $L 1]
			if {$dst == $nexthop} {	
				# Cost Debug
				#puts "Src:$src Dst:$dst Link:$link"
				#puts "[$link info class]"
				# End Cost Debug
				return $link
			}
		}
	}
	return -1
}

#
# This procedure is used to add explicitly
# routes to a node, overriding the routing
# policy used (e.g. shortest path routing).
# Tested currently with static ns2 routing.
# Essentially, it is used to add policy-routing
# entries in realistic network topologies.
#
# Parameters:
#
#	node: the ns2 node, to which the route
#	      entry is added to. This parameter
#	      is of type Node.
#	dst: the destination, to which the route
#	     entry refers to. This parameter
#	      is of type Node.
#	via: the next hope node, that the local node
#	     will use to access the destination node.
#	     This parameter is of type Node.
#
##########################################################
proc addExplicitRoute {node dst via } {
	set link2via [$node nexthop2link [$via node-addr]]
	if {$link2via != -1} {
		$node add-route [$dst node-addr] [$link2via head]
	} else {
		puts "Warning: No link exists between node [$node node-addr] and [$via node-addr]. Explicit route not added."
	}
}


--END

#Create four nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

set f0 [new Agent/Fattree]
set f1 [new Agent/Fattree]
set f2 [new Agent/Fattree]
set f3 [new Agent/Fattree]
set f4 [new Agent/Fattree]
set f5 [new Agent/Fattree]
$ns attach-agent $n0 $f0
$f0 init 0
$ns attach-agent $n1 $f1
$f1 init 1
$ns attach-agent $n2 $f2
$f2 init 2
$ns attach-agent $n3 $f3
$f3 init 3
$ns attach-agent $n4 $f4
$f4 init 4
$ns attach-agent $n5 $f5
$f5 init 5

#Create links between the nodes
$ns duplex-link $n0 $n5 1Kb 10ms DropTail
$ns duplex-link $n1 $n5 1Kb 10ms DropTail
$ns duplex-link $n2 $n5 1Kb 10ms DropTail
$ns duplex-link $n3 $n5 1Kb 10ms DropTail
$ns duplex-link $n4 $n5 1Kb 10ms DropTail

#Call the finish procedure after 5 seconds of simulation time
#$ns at 0.0 "$f1 post 3 10"
$ns at 0.0 "$f2 post 3 1000"
$ns at 0.0 "$f1 post 3 1000"
#$ns at 0.0 "$f3 post 4 100"
#$ns at 0.0 "$f4 post 1 50"
$ns at 100.0 "finish"

#Run the simulation
$ns run
