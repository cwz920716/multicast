#!/usr/bin/python

import os

def output2f(fname, line):
  if os.path.exists(fname):
    fout = file(fname, 'a')
  else:
    fout = file(fname, 'w')
  fout.write(line)
  fout.close()

fin = file('/home/mininet/trace/wve/anon-trace.txt', 'r')

group_stat = {}
node_stat = {}
while True:
  line = fin.readline()
  if len(line) == 0:
    break
  # print line
  tuples = line.split()
  node = tuples[0]
  group = tuples[3]
  op = tuples[2]
  if not group_stat.has_key(group):
    group_stat[group] = 0
  if not node_stat.has_key(node):
    node_stat[node] = 0
  # output2f('nodes/%s.txt' % node, line)
  if op == 'post':
    # output2f('nodes-post/%s.txt' % node, line)
    # output2f('groups-post/%s.txt' % group, line)
    msg = tuples[4]
    group_stat[group] = group_stat[group] + int(msg)
    node_stat[node] = node_stat[node] + int(msg)

gslist = []
for key in group_stat.keys():
  gslist.append(group_stat[key])
  # print '%s\t%d' % (key, group_stat[key])
gslist.sort()

nslist = []
for key in node_stat.keys():
  nslist.append(node_stat[key])
  # print '%s\t%d' % (key, node_stat[key])
nslist.sort()

cdf = []
sum = 0
cnt = 0
gap = 10000
for e in gslist:
  sum = sum + e
  if e >= gap:
    while gap <= e:
      # print gap
      cdf.append(cnt)
      gap = gap + 10000
  cnt = cnt + 1
  # print cnt
cdf.append(cnt)
# print sum

gap = 10000
for e in cdf:
  print '%d\t%d' % (gap, e)
  gap = gap + 10000
  pass

fin.close()
