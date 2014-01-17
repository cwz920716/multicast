#!/usr/bin/python

import os, sys

'''
   0 for single
   1 for uniform
'''
def pattern(filename):
  fin = file(filename, 'r')
  single_poster = 0
  isSingle = True
  ret = 0
  while True:
    line = fin.readline()
    if len(line) == 0:
      break
    # print line
    tuples = line.split()
    node = tuples[0]
    if single_poster == 0:
      single_poster = int(node)
    elif not single_poster == int(node):
      print filename
      isSingle = False
      break
  fin.close()
  if not isSingle:
    ret = 1
  return ret

# 0 for single, 1 for uniform
stat = [0, 0]
dir = 'groups-post'
flist = os.listdir(dir)
for line in flist:
  path = os.path.join(dir, line)
  if os.path.isfile(path):
    ret = pattern(path)
    stat[ret] += 1

print 'single: %d\nuniform: %d' % (stat[0], stat[1])
