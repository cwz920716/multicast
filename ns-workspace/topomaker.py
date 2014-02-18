#! /usr/bin/python

template = file('template.tcl', 'r')
k = 8

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
trace = file('trace.tcl', 'r')
while True:
  line = trace.readline()
  if line == '--END\n':
    break
  print line,

print 
print "$ns run"

template.close()
