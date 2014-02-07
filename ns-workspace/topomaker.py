#! /usr/bin/python

template = file('template.tcl', 'r')
k = 4

while True:
  line = template.readline()
  if line == '--END\n':
    break
  print line,

print "set n0 [$ns node]"
print "# Create [1 - k * k * k / 4] host nodes"
for i in range(1, k * k * k / 4 + 1):
  print "set n%d [$ns node]" % i

print "# Create k * k * k / 4 + [1 - k * k / 2] edge swicthes"
for i in range(k * k * k / 4 + 1, k * k * k / 4 + k * k / 2 + 1):
  print "set n%d [$ns node]" % i

print "# Create k * k * k / 4 + k * k / 2 + [1 - k * k / 2] aggr swicthes"
for i in range(k * k * k / 4 + k * k / 2 + 1, k * k * k / 4 + k * k + 1):
  print "set n%d [$ns node]" % i

print "# Create k * k * k / 4 + k * k + [1 - k * k / 4] edge swicthes"
for i in range(k * k * k / 4 + k * k + 1, k * k * k / 4 + k * k + k * k / 4 + 1):
  print "set n%d [$ns node]" % i

print
print "# Attach agent to node and call init"
for i in range(1, k * k * k / 4 + k * k + k * k / 4 + 1):
  print "set f%d [new Agent/Fattree]" % i
  print "$ns attach-agent $n%d $f%d" % (i, i)
  print "$f%d addr %d" % (i, i)

print
print "#Create links between the edge and host"
for host in range(1, k * k * k / 4 + 1):
  edge = (host - 1) / (k / 2) + k * k * k / 4 + 1
  print "$ns duplex-link $n%d $n%d 1Gb 0ms DropTail" % (host, edge)
#  print "addExplicitRoute $n%d $n%d $n%d" % (host, edge, edge)
#  print "addExplicitRoute $n%d $n%d $n%d" % (edge, host, host)

print "#Create links between the aggr and edge"
for edge in range(k * k * k / 4 + 1, k * k * k / 4 + k * k / 2 + 1):
  pod = (edge - k * k * k / 4 - 1) / (k / 2) + 1
  aggr_r1 = k * k * k / 4 + k * k / 2 + (pod - 1) * k / 2 + 1
  aggr_r2 = aggr_r1 - 1 + k / 2
  for aggr in range(aggr_r1, aggr_r2 + 1):
    print "$ns duplex-link $n%d $n%d 1Gb 0ms DropTail" % (edge, aggr)
#    print "addExplicitRoute $n%d $n%d $n%d" % (edge, aggr, aggr)
#    print "addExplicitRoute $n%d $n%d $n%d" % (aggr, edge, edge)

print "#Create links between the core and aggr, classic fattree"
for aggr in range(k * k * k / 4 + k * k / 2 + 1, k * k * k / 4 + k * k + 1):
  pod = (aggr - k * k * k / 4 - k * k / 2 - 1) / (k / 2) + 1
  core_r1 = (aggr - k * k * k / 4 - k * k / 2 - 1) % (k / 2) * (k / 2) + 1 + k * k * k / 4 + k * k
  core_r2 = core_r1 - 1 + k / 2
  for core in range(core_r1, core_r2 + 1):
    print "$ns duplex-link $n%d $n%d 1Gb 0ms DropTail" % (aggr, core)
#    print "addExplicitRoute $n%d $n%d $n%d" % (aggr, core, core)
#    print "addExplicitRoute $n%d $n%d $n%d" % (core, aggr, aggr)

print
time = 0.0
print "$ns at %.1f \"$f1 subscribe 1\"" % (time + 1)
print "$ns at %.1f \"$f1 unsubscribe 1\"" % (time + 2)
print "$ns at %.1f \"$f2 subscribe 2\"" % (time + 2)
print "$ns at %.1f \"$f3 subscribe 2\"" % (time + 2)
print "$ns at %.1f \"$f7 subscribe 1\"" % (time + 2)
print "$ns at %.1f \"$f12 subscribe 1\"" % (time + 2)
print "$ns at %.1f \"$f16 subscribe 2\"" % (time + 2)
print "$ns at %.1f \"$f17 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f18 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f20 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f22 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f24 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f25 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f26 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f27 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f29 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f32 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f34 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"$f35 dump-mcast\"" % (time + 3)
print "$ns at %.1f \"finish\"" % (time + 100)
'''
for i in range(1, k * k * k / 4 + 1):
  for j in range(1, k * k * k / 4 + 1):
    if (not i == j) and time <= 1000:
      print "$ns at %.1f \"$f%d post %d 1000\"" % (time, i, j)
      time = time + 1
print "$ns at %.1f \"finish\"" % time
'''

print 
print "$ns run"

template.close()
