#Create a simulator object
set ns [new Simulator]
set rtProto Static

#Define different colors for data flows
#$ns color 1 Blue
#$ns color 2 Red

#Open the nam trace file
set nf [open out.nam w]
$ns namtrace-all $nf

#Define a 'finish' procedure
proc finish {} {
        global ns nf
        $ns flush-trace
	#Close the trace file
        close $nf
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


set n0 [$ns node]
# Create [1 - k * k * k / 4] host nodes
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]
set n6 [$ns node]
set n7 [$ns node]
set n8 [$ns node]
set n9 [$ns node]
set n10 [$ns node]
set n11 [$ns node]
set n12 [$ns node]
set n13 [$ns node]
set n14 [$ns node]
set n15 [$ns node]
set n16 [$ns node]
# Create k * k * k / 4 + [1 - k * k / 2] edge swicthes
set n17 [$ns node]
set n18 [$ns node]
set n19 [$ns node]
set n20 [$ns node]
set n21 [$ns node]
set n22 [$ns node]
set n23 [$ns node]
set n24 [$ns node]
# Create k * k * k / 4 + k * k / 2 + [1 - k * k / 2] aggr swicthes
set n25 [$ns node]
set n26 [$ns node]
set n27 [$ns node]
set n28 [$ns node]
set n29 [$ns node]
set n30 [$ns node]
set n31 [$ns node]
set n32 [$ns node]
# Create k * k * k / 4 + k * k + [1 - k * k / 4] edge swicthes
set n33 [$ns node]
set n34 [$ns node]
set n35 [$ns node]
set n36 [$ns node]

# Attach agent to node and call init
set f1 [new Agent/Fattree]
$ns attach-agent $n1 $f1
$f1 addr 1
set f2 [new Agent/Fattree]
$ns attach-agent $n2 $f2
$f2 addr 2
set f3 [new Agent/Fattree]
$ns attach-agent $n3 $f3
$f3 addr 3
set f4 [new Agent/Fattree]
$ns attach-agent $n4 $f4
$f4 addr 4
set f5 [new Agent/Fattree]
$ns attach-agent $n5 $f5
$f5 addr 5
set f6 [new Agent/Fattree]
$ns attach-agent $n6 $f6
$f6 addr 6
set f7 [new Agent/Fattree]
$ns attach-agent $n7 $f7
$f7 addr 7
set f8 [new Agent/Fattree]
$ns attach-agent $n8 $f8
$f8 addr 8
set f9 [new Agent/Fattree]
$ns attach-agent $n9 $f9
$f9 addr 9
set f10 [new Agent/Fattree]
$ns attach-agent $n10 $f10
$f10 addr 10
set f11 [new Agent/Fattree]
$ns attach-agent $n11 $f11
$f11 addr 11
set f12 [new Agent/Fattree]
$ns attach-agent $n12 $f12
$f12 addr 12
set f13 [new Agent/Fattree]
$ns attach-agent $n13 $f13
$f13 addr 13
set f14 [new Agent/Fattree]
$ns attach-agent $n14 $f14
$f14 addr 14
set f15 [new Agent/Fattree]
$ns attach-agent $n15 $f15
$f15 addr 15
set f16 [new Agent/Fattree]
$ns attach-agent $n16 $f16
$f16 addr 16
set f17 [new Agent/Fattree]
$ns attach-agent $n17 $f17
$f17 addr 17
set f18 [new Agent/Fattree]
$ns attach-agent $n18 $f18
$f18 addr 18
set f19 [new Agent/Fattree]
$ns attach-agent $n19 $f19
$f19 addr 19
set f20 [new Agent/Fattree]
$ns attach-agent $n20 $f20
$f20 addr 20
set f21 [new Agent/Fattree]
$ns attach-agent $n21 $f21
$f21 addr 21
set f22 [new Agent/Fattree]
$ns attach-agent $n22 $f22
$f22 addr 22
set f23 [new Agent/Fattree]
$ns attach-agent $n23 $f23
$f23 addr 23
set f24 [new Agent/Fattree]
$ns attach-agent $n24 $f24
$f24 addr 24
set f25 [new Agent/Fattree]
$ns attach-agent $n25 $f25
$f25 addr 25
set f26 [new Agent/Fattree]
$ns attach-agent $n26 $f26
$f26 addr 26
set f27 [new Agent/Fattree]
$ns attach-agent $n27 $f27
$f27 addr 27
set f28 [new Agent/Fattree]
$ns attach-agent $n28 $f28
$f28 addr 28
set f29 [new Agent/Fattree]
$ns attach-agent $n29 $f29
$f29 addr 29
set f30 [new Agent/Fattree]
$ns attach-agent $n30 $f30
$f30 addr 30
set f31 [new Agent/Fattree]
$ns attach-agent $n31 $f31
$f31 addr 31
set f32 [new Agent/Fattree]
$ns attach-agent $n32 $f32
$f32 addr 32
set f33 [new Agent/Fattree]
$ns attach-agent $n33 $f33
$f33 addr 33
set f34 [new Agent/Fattree]
$ns attach-agent $n34 $f34
$f34 addr 34
set f35 [new Agent/Fattree]
$ns attach-agent $n35 $f35
$f35 addr 35
set f36 [new Agent/Fattree]
$ns attach-agent $n36 $f36
$f36 addr 36

#Create links between the edge and host
$ns duplex-link $n1 $n17 1Gb 0ms DropTail
$ns duplex-link $n2 $n17 1Gb 0ms DropTail
$ns duplex-link $n3 $n18 1Gb 0ms DropTail
$ns duplex-link $n4 $n18 1Gb 0ms DropTail
$ns duplex-link $n5 $n19 1Gb 0ms DropTail
$ns duplex-link $n6 $n19 1Gb 0ms DropTail
$ns duplex-link $n7 $n20 1Gb 0ms DropTail
$ns duplex-link $n8 $n20 1Gb 0ms DropTail
$ns duplex-link $n9 $n21 1Gb 0ms DropTail
$ns duplex-link $n10 $n21 1Gb 0ms DropTail
$ns duplex-link $n11 $n22 1Gb 0ms DropTail
$ns duplex-link $n12 $n22 1Gb 0ms DropTail
$ns duplex-link $n13 $n23 1Gb 0ms DropTail
$ns duplex-link $n14 $n23 1Gb 0ms DropTail
$ns duplex-link $n15 $n24 1Gb 0ms DropTail
$ns duplex-link $n16 $n24 1Gb 0ms DropTail
#Create links between the aggr and edge
$ns duplex-link $n17 $n25 1Gb 0ms DropTail
$ns duplex-link $n17 $n26 1Gb 0ms DropTail
$ns duplex-link $n18 $n25 1Gb 0ms DropTail
$ns duplex-link $n18 $n26 1Gb 0ms DropTail
$ns duplex-link $n19 $n27 1Gb 0ms DropTail
$ns duplex-link $n19 $n28 1Gb 0ms DropTail
$ns duplex-link $n20 $n27 1Gb 0ms DropTail
$ns duplex-link $n20 $n28 1Gb 0ms DropTail
$ns duplex-link $n21 $n29 1Gb 0ms DropTail
$ns duplex-link $n21 $n30 1Gb 0ms DropTail
$ns duplex-link $n22 $n29 1Gb 0ms DropTail
$ns duplex-link $n22 $n30 1Gb 0ms DropTail
$ns duplex-link $n23 $n31 1Gb 0ms DropTail
$ns duplex-link $n23 $n32 1Gb 0ms DropTail
$ns duplex-link $n24 $n31 1Gb 0ms DropTail
$ns duplex-link $n24 $n32 1Gb 0ms DropTail
#Create links between the core and aggr, classic fattree
$ns duplex-link $n25 $n33 1Gb 0ms DropTail
$ns duplex-link $n25 $n34 1Gb 0ms DropTail
$ns duplex-link $n26 $n35 1Gb 0ms DropTail
$ns duplex-link $n26 $n36 1Gb 0ms DropTail
$ns duplex-link $n27 $n33 1Gb 0ms DropTail
$ns duplex-link $n27 $n34 1Gb 0ms DropTail
$ns duplex-link $n28 $n35 1Gb 0ms DropTail
$ns duplex-link $n28 $n36 1Gb 0ms DropTail
$ns duplex-link $n29 $n33 1Gb 0ms DropTail
$ns duplex-link $n29 $n34 1Gb 0ms DropTail
$ns duplex-link $n30 $n35 1Gb 0ms DropTail
$ns duplex-link $n30 $n36 1Gb 0ms DropTail
$ns duplex-link $n31 $n33 1Gb 0ms DropTail
$ns duplex-link $n31 $n34 1Gb 0ms DropTail
$ns duplex-link $n32 $n35 1Gb 0ms DropTail
$ns duplex-link $n32 $n36 1Gb 0ms DropTail

$ns at 0.0 "$f1 post 2 1000"
$ns at 1.0 "$f1 post 3 1000"
$ns at 2.0 "$f1 post 4 1000"
$ns at 3.0 "$f1 post 5 1000"
$ns at 4.0 "$f1 post 6 1000"
$ns at 5.0 "$f1 post 7 1000"
$ns at 6.0 "$f1 post 8 1000"
$ns at 7.0 "$f1 post 9 1000"
$ns at 8.0 "$f1 post 10 1000"
$ns at 9.0 "$f1 post 11 1000"
$ns at 10.0 "$f1 post 12 1000"
$ns at 11.0 "$f1 post 13 1000"
$ns at 12.0 "$f1 post 14 1000"
$ns at 13.0 "$f1 post 15 1000"
$ns at 14.0 "$f1 post 16 1000"
$ns at 15.0 "$f2 post 1 1000"
$ns at 16.0 "$f2 post 3 1000"
$ns at 17.0 "$f2 post 4 1000"
$ns at 18.0 "$f2 post 5 1000"
$ns at 19.0 "$f2 post 6 1000"
$ns at 20.0 "$f2 post 7 1000"
$ns at 21.0 "$f2 post 8 1000"
$ns at 22.0 "$f2 post 9 1000"
$ns at 23.0 "$f2 post 10 1000"
$ns at 24.0 "$f2 post 11 1000"
$ns at 25.0 "$f2 post 12 1000"
$ns at 26.0 "$f2 post 13 1000"
$ns at 27.0 "$f2 post 14 1000"
$ns at 28.0 "$f2 post 15 1000"
$ns at 29.0 "$f2 post 16 1000"
$ns at 30.0 "$f3 post 1 1000"
$ns at 31.0 "$f3 post 2 1000"
$ns at 32.0 "$f3 post 4 1000"
$ns at 33.0 "$f3 post 5 1000"
$ns at 34.0 "$f3 post 6 1000"
$ns at 35.0 "$f3 post 7 1000"
$ns at 36.0 "$f3 post 8 1000"
$ns at 37.0 "$f3 post 9 1000"
$ns at 38.0 "$f3 post 10 1000"
$ns at 39.0 "$f3 post 11 1000"
$ns at 40.0 "$f3 post 12 1000"
$ns at 41.0 "$f3 post 13 1000"
$ns at 42.0 "$f3 post 14 1000"
$ns at 43.0 "$f3 post 15 1000"
$ns at 44.0 "$f3 post 16 1000"
$ns at 45.0 "$f4 post 1 1000"
$ns at 46.0 "$f4 post 2 1000"
$ns at 47.0 "$f4 post 3 1000"
$ns at 48.0 "$f4 post 5 1000"
$ns at 49.0 "$f4 post 6 1000"
$ns at 50.0 "$f4 post 7 1000"
$ns at 51.0 "$f4 post 8 1000"
$ns at 52.0 "$f4 post 9 1000"
$ns at 53.0 "$f4 post 10 1000"
$ns at 54.0 "$f4 post 11 1000"
$ns at 55.0 "$f4 post 12 1000"
$ns at 56.0 "$f4 post 13 1000"
$ns at 57.0 "$f4 post 14 1000"
$ns at 58.0 "$f4 post 15 1000"
$ns at 59.0 "$f4 post 16 1000"
$ns at 60.0 "$f5 post 1 1000"
$ns at 61.0 "$f5 post 2 1000"
$ns at 62.0 "$f5 post 3 1000"
$ns at 63.0 "$f5 post 4 1000"
$ns at 64.0 "$f5 post 6 1000"
$ns at 65.0 "$f5 post 7 1000"
$ns at 66.0 "$f5 post 8 1000"
$ns at 67.0 "$f5 post 9 1000"
$ns at 68.0 "$f5 post 10 1000"
$ns at 69.0 "$f5 post 11 1000"
$ns at 70.0 "$f5 post 12 1000"
$ns at 71.0 "$f5 post 13 1000"
$ns at 72.0 "$f5 post 14 1000"
$ns at 73.0 "$f5 post 15 1000"
$ns at 74.0 "$f5 post 16 1000"
$ns at 75.0 "$f6 post 1 1000"
$ns at 76.0 "$f6 post 2 1000"
$ns at 77.0 "$f6 post 3 1000"
$ns at 78.0 "$f6 post 4 1000"
$ns at 79.0 "$f6 post 5 1000"
$ns at 80.0 "$f6 post 7 1000"
$ns at 81.0 "$f6 post 8 1000"
$ns at 82.0 "$f6 post 9 1000"
$ns at 83.0 "$f6 post 10 1000"
$ns at 84.0 "$f6 post 11 1000"
$ns at 85.0 "$f6 post 12 1000"
$ns at 86.0 "$f6 post 13 1000"
$ns at 87.0 "$f6 post 14 1000"
$ns at 88.0 "$f6 post 15 1000"
$ns at 89.0 "$f6 post 16 1000"
$ns at 90.0 "$f7 post 1 1000"
$ns at 91.0 "$f7 post 2 1000"
$ns at 92.0 "$f7 post 3 1000"
$ns at 93.0 "$f7 post 4 1000"
$ns at 94.0 "$f7 post 5 1000"
$ns at 95.0 "$f7 post 6 1000"
$ns at 96.0 "$f7 post 8 1000"
$ns at 97.0 "$f7 post 9 1000"
$ns at 98.0 "$f7 post 10 1000"
$ns at 99.0 "$f7 post 11 1000"
$ns at 100.0 "$f7 post 12 1000"
$ns at 101.0 "$f7 post 13 1000"
$ns at 102.0 "$f7 post 14 1000"
$ns at 103.0 "$f7 post 15 1000"
$ns at 104.0 "$f7 post 16 1000"
$ns at 105.0 "$f8 post 1 1000"
$ns at 106.0 "$f8 post 2 1000"
$ns at 107.0 "$f8 post 3 1000"
$ns at 108.0 "$f8 post 4 1000"
$ns at 109.0 "$f8 post 5 1000"
$ns at 110.0 "$f8 post 6 1000"
$ns at 111.0 "$f8 post 7 1000"
$ns at 112.0 "$f8 post 9 1000"
$ns at 113.0 "$f8 post 10 1000"
$ns at 114.0 "$f8 post 11 1000"
$ns at 115.0 "$f8 post 12 1000"
$ns at 116.0 "$f8 post 13 1000"
$ns at 117.0 "$f8 post 14 1000"
$ns at 118.0 "$f8 post 15 1000"
$ns at 119.0 "$f8 post 16 1000"
$ns at 120.0 "$f9 post 1 1000"
$ns at 121.0 "$f9 post 2 1000"
$ns at 122.0 "$f9 post 3 1000"
$ns at 123.0 "$f9 post 4 1000"
$ns at 124.0 "$f9 post 5 1000"
$ns at 125.0 "$f9 post 6 1000"
$ns at 126.0 "$f9 post 7 1000"
$ns at 127.0 "$f9 post 8 1000"
$ns at 128.0 "$f9 post 10 1000"
$ns at 129.0 "$f9 post 11 1000"
$ns at 130.0 "$f9 post 12 1000"
$ns at 131.0 "$f9 post 13 1000"
$ns at 132.0 "$f9 post 14 1000"
$ns at 133.0 "$f9 post 15 1000"
$ns at 134.0 "$f9 post 16 1000"
$ns at 135.0 "$f10 post 1 1000"
$ns at 136.0 "$f10 post 2 1000"
$ns at 137.0 "$f10 post 3 1000"
$ns at 138.0 "$f10 post 4 1000"
$ns at 139.0 "$f10 post 5 1000"
$ns at 140.0 "$f10 post 6 1000"
$ns at 141.0 "$f10 post 7 1000"
$ns at 142.0 "$f10 post 8 1000"
$ns at 143.0 "$f10 post 9 1000"
$ns at 144.0 "$f10 post 11 1000"
$ns at 145.0 "$f10 post 12 1000"
$ns at 146.0 "$f10 post 13 1000"
$ns at 147.0 "$f10 post 14 1000"
$ns at 148.0 "$f10 post 15 1000"
$ns at 149.0 "$f10 post 16 1000"
$ns at 150.0 "$f11 post 1 1000"
$ns at 151.0 "$f11 post 2 1000"
$ns at 152.0 "$f11 post 3 1000"
$ns at 153.0 "$f11 post 4 1000"
$ns at 154.0 "$f11 post 5 1000"
$ns at 155.0 "$f11 post 6 1000"
$ns at 156.0 "$f11 post 7 1000"
$ns at 157.0 "$f11 post 8 1000"
$ns at 158.0 "$f11 post 9 1000"
$ns at 159.0 "$f11 post 10 1000"
$ns at 160.0 "$f11 post 12 1000"
$ns at 161.0 "$f11 post 13 1000"
$ns at 162.0 "$f11 post 14 1000"
$ns at 163.0 "$f11 post 15 1000"
$ns at 164.0 "$f11 post 16 1000"
$ns at 165.0 "$f12 post 1 1000"
$ns at 166.0 "$f12 post 2 1000"
$ns at 167.0 "$f12 post 3 1000"
$ns at 168.0 "$f12 post 4 1000"
$ns at 169.0 "$f12 post 5 1000"
$ns at 170.0 "$f12 post 6 1000"
$ns at 171.0 "$f12 post 7 1000"
$ns at 172.0 "$f12 post 8 1000"
$ns at 173.0 "$f12 post 9 1000"
$ns at 174.0 "$f12 post 10 1000"
$ns at 175.0 "$f12 post 11 1000"
$ns at 176.0 "$f12 post 13 1000"
$ns at 177.0 "$f12 post 14 1000"
$ns at 178.0 "$f12 post 15 1000"
$ns at 179.0 "$f12 post 16 1000"
$ns at 180.0 "$f13 post 1 1000"
$ns at 181.0 "$f13 post 2 1000"
$ns at 182.0 "$f13 post 3 1000"
$ns at 183.0 "$f13 post 4 1000"
$ns at 184.0 "$f13 post 5 1000"
$ns at 185.0 "$f13 post 6 1000"
$ns at 186.0 "$f13 post 7 1000"
$ns at 187.0 "$f13 post 8 1000"
$ns at 188.0 "$f13 post 9 1000"
$ns at 189.0 "$f13 post 10 1000"
$ns at 190.0 "$f13 post 11 1000"
$ns at 191.0 "$f13 post 12 1000"
$ns at 192.0 "$f13 post 14 1000"
$ns at 193.0 "$f13 post 15 1000"
$ns at 194.0 "$f13 post 16 1000"
$ns at 195.0 "$f14 post 1 1000"
$ns at 196.0 "$f14 post 2 1000"
$ns at 197.0 "$f14 post 3 1000"
$ns at 198.0 "$f14 post 4 1000"
$ns at 199.0 "$f14 post 5 1000"
$ns at 200.0 "$f14 post 6 1000"
$ns at 201.0 "$f14 post 7 1000"
$ns at 202.0 "$f14 post 8 1000"
$ns at 203.0 "$f14 post 9 1000"
$ns at 204.0 "$f14 post 10 1000"
$ns at 205.0 "$f14 post 11 1000"
$ns at 206.0 "$f14 post 12 1000"
$ns at 207.0 "$f14 post 13 1000"
$ns at 208.0 "$f14 post 15 1000"
$ns at 209.0 "$f14 post 16 1000"
$ns at 210.0 "$f15 post 1 1000"
$ns at 211.0 "$f15 post 2 1000"
$ns at 212.0 "$f15 post 3 1000"
$ns at 213.0 "$f15 post 4 1000"
$ns at 214.0 "$f15 post 5 1000"
$ns at 215.0 "$f15 post 6 1000"
$ns at 216.0 "$f15 post 7 1000"
$ns at 217.0 "$f15 post 8 1000"
$ns at 218.0 "$f15 post 9 1000"
$ns at 219.0 "$f15 post 10 1000"
$ns at 220.0 "$f15 post 11 1000"
$ns at 221.0 "$f15 post 12 1000"
$ns at 222.0 "$f15 post 13 1000"
$ns at 223.0 "$f15 post 14 1000"
$ns at 224.0 "$f15 post 16 1000"
$ns at 225.0 "$f16 post 1 1000"
$ns at 226.0 "$f16 post 2 1000"
$ns at 227.0 "$f16 post 3 1000"
$ns at 228.0 "$f16 post 4 1000"
$ns at 229.0 "$f16 post 5 1000"
$ns at 230.0 "$f16 post 6 1000"
$ns at 231.0 "$f16 post 7 1000"
$ns at 232.0 "$f16 post 8 1000"
$ns at 233.0 "$f16 post 9 1000"
$ns at 234.0 "$f16 post 10 1000"
$ns at 235.0 "$f16 post 11 1000"
$ns at 236.0 "$f16 post 12 1000"
$ns at 237.0 "$f16 post 13 1000"
$ns at 238.0 "$f16 post 14 1000"
$ns at 239.0 "$f16 post 15 1000"
$ns at 240.0 "finish"

$ns run
