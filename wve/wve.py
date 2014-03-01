#!/usr/bin/python

import os

def output2f(fname, line):
  if os.path.exists(fname):
    fout = file(fname, 'a')
  else:
    fout = file(fname, 'w')
  fout.write(line)
  fout.close()

fin = file('/home/cwz/trace/wve/anon-trace.txt', 'r')

group_stat = {}
node_stat = {}
group_num = {}
group_mem = {}
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
    group_num[group] = 0
    group_mem[group] = []
  if not node_stat.has_key(node):
    node_stat[node] = 0
  # output2f('nodes/%s.txt' % node, line)
  if op == 'post':
    # output2f('nodes-post/%s.txt' % node, line)
    # output2f('groups-post/%s.txt' % group, line)
    msg = tuples[4]
    group_stat[group] = group_stat[group] + int(msg)
    node_stat[node] = node_stat[node] + int(msg)
  elif op == 'subscribe':
    group_num[group] = group_num[group] + 1
    group_mem[group].append(node)
  else:
    group_num[group] = group_num[group] - 1

gslist = []
sumtfc = 0
sumOK = 0
for key in group_stat.keys():
  gslist.append(group_stat[key])
  setk = set(group_mem[key])
  sumtfc = sumtfc + group_stat[key]
  if ('1' in setk or '127' in setk) or len(setk) <= 2:# ('1' in setk and '127' in setk and len(setk) == 2) or len(setk) < 2:
    sumOK = sumOK + group_stat[key]
    pass
  else:
    print '%s\t%d\t%d\t[' % (key, group_stat[key], len(setk)),
    for k in setk:
      print '%s, ' % k,
    print ']'
gslist.sort()

print '%d/%d=%f' % (sumtfc-sumOK, sumtfc, float(sumtfc-sumOK) / float(sumtfc))

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
  # print '%d\t%d' % (gap, e)
  gap = gap + 10000
  pass

fin.close()
