#!/usr/bin/python

import os
import sys

fin = file('groups-post/%s.txt' % sys.argv[1], 'r')
fout = file('groups-aggr/%s.txt' % sys.argv[1], 'w')

time_stat = {}
while True:
  line = fin.readline()
  if len(line) == 0:
    break
  # print line
  tuples = line.split()
  node = tuples[0]
  time = tuples[1]
  group = tuples[3]
  op = tuples[2]
  if not time_stat.has_key(time):
    time_stat[time] = 0
  if op == 'post':
    msg = tuples[4]
    time_stat[time] = time_stat[time] + int(msg)

tlist = []
for key in time_stat.keys():
  tlist.append(int(key))
tlist.sort()

for e in tlist:
  key = str(e)
  fout.write('%s\t%d\n' % (key, time_stat[key]))
