#!/usr/bin/python

import os

def output2f(fname, line):
  if os.path.exists(fname):
    fout = file(fname, 'a')
  else:
    fout = file(fname, 'w')
  fout.write(line)
  fout.close()

largest = -1
fin = file('/home/cwz/trace/wve/anon-trace.txt', 'r')
while True:
  line = fin.readline()
  if len(line) == 0:
    break
  # print line
  tuples = line.split()
  node = tuples[0]
  time = (int(tuples[1])) / 1000.0
  if largest == -1 or time > largest:
    largest = time
  group = tuples[3]
  op = tuples[2]
  if op == 'post':
    msg = tuples[4]
    print '$ns at %f \"$f%s post %s %s\"' % (time, node, group, msg)
  elif op == 'subscribe':
    print '$ns at %f \"$f%s subscribe %s\"' % (time, node, group)
  elif op == 'unsubscribe':
    print '$ns at %f \"$f%s unsubscribe %s\"' % (time, node, group)

k = 8


for i in range(k * k * k / 4 + 1, k * k * k / 4 + k * k + k * k / 4 + 1):
  print '$ns at %f \"$f%d dump-mcast\"' % (largest + 1000, i)

'''


for i in range(k * k * k / 4 + 1, k * k * k / 4 + k * k + k * k / 4 + 1):
  print '$ns at %f \"$f%d dump-tfcsum\"' % (largest + 1, i)



print
print "#Dump links between the edge and host"
for host in range(1, k * k * k / 4 + 1):
  edge = (host - 1) / (k / 2) + k * k * k / 4 + 1
  print '$ns at %f \"$f%s dump-link-stat %s\"' % (largest + 1, host, edge)
  print '$ns at %f \"$f%s dump-link-stat %s\"' % (largest + 1, edge, host)

print "#Dump links between the aggr and edge"
for edge in range(k * k * k / 4 + 1, k * k * k / 4 + k * k / 2 + 1):
  pod = (edge - k * k * k / 4 - 1) / (k / 2) + 1
  aggr_r1 = k * k * k / 4 + k * k / 2 + (pod - 1) * k / 2 + 1
  aggr_r2 = aggr_r1 - 1 + k / 2
  for aggr in range(aggr_r1, aggr_r2 + 1):
    print '$ns at %f \"$f%s dump-link-stat %s\"' % (largest + 1, edge, aggr)
    print '$ns at %f \"$f%s dump-link-stat %s\"' % (largest + 1, aggr, edge)

print "#Dump links between the core and aggr, classic fattree"
for aggr in range(k * k * k / 4 + k * k / 2 + 1, k * k * k / 4 + k * k + 1):
  pod = (aggr - k * k * k / 4 - k * k / 2 - 1) / (k / 2) + 1
  core_r1 = (aggr - k * k * k / 4 - k * k / 2 - 1) % (k / 2) * (k / 2) + 1 + k * k * k / 4 + k * k
  core_r2 = core_r1 - 1 + k / 2
  for core in range(core_r1, core_r2 + 1):
    print '$ns at %f \"$f%s dump-link-stat %s\"' % (largest + 1, aggr, core)
    print '$ns at %f \"$f%s dump-link-stat %s\"' % (largest + 1, core, aggr)


ngroup = 2000
print
for g in range(1, ngroup + 1):
  print '$ns at %f \"$f%s dump-fair %s\"' % (largest + 1, 1, g)
'''

print '$ns at %f \"finish\"' % (largest + 1001)
print '--END\n'
fin.close()
