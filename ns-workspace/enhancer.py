#! /usr/bin/python

import random
import numpy as np

fin = file('/home/cwz/trace/wve/anon-trace.txt', 'r')
while True:
  line = fin.readline()
  if len(line) == 0:
    break
  # print line
  tuples = line.split()
  node = int(tuples[0])
  if tuples[2] == 'post':
    tuples[4] = str(int(tuples[4]) * 1000)
    print '%s %s %s %s %s' % (tuples[0], tuples[1], tuples[2], tuples[3], tuples[4])
  else:
    print '%s %s %s %s' % (tuples[0], tuples[1], tuples[2], tuples[3])
