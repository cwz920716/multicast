#! /usr/bin/python

import random
import numpy as np
import matplotlib.pyplot as plt
import scipy.special as sps

def powerlaw(min, max, n):
  max += 1
  pl = ((max ** (n+1) - min ** (n + 1)) * random.random() + min ** (n + 1)) ** (1.0 / (n+1))
  return (max - 1 - int(pl)) + min

def wvelaw(maxp):
  max = 10**maxp
  x = random.random()
  z = 0.3
  if x < z:
    return 0
  step = (1 - z) / (maxp + 1)
  s0 = int((x - z) / step) - 1
  s1 = s0 + 1
  return int(random.random() * (10 ** s1 - 10 ** s0) + 10 ** s0)

def chunk(size):
  if (size < 16):
    return 1
  elif (size < 128):
    return 64
  elif (size < 512):
    return 256
  elif (size < 2048):
    return 1024
  else:
    return 2048

def chunk_gap(size):
  if (size < 16):
    return 10
  elif (size < 128):
    return 20
  elif (size < 512):
    return 400
  elif (size < 2048):
    return 2000
  else:
    return 10 * 1000

def nextchunk(left, total):
  c = chunk(total)
  if left < c:
    return left
  else:
    return c

k = 8
H = k * k * k / 4
N = 8 * 1000
P = 6
smember = []
while len(smember) < N:
  x = int(np.random.zipf(2, 1))
  if (x < H - 3):
    smember.append(x + 4)
smember = np.array(smember)
# print min(smember)
# print max(smember)
# print len(smember[smember == 5])
# print len(smember[smember < 16])
# print len(smember[smember > 100])

stfc = []
Z = 0.32
while len(stfc) < N:
  x = int(np.random.zipf(1.1, 1))
  zr = np.random.random()
  if (zr <= Z):
    stfc.append(0)
  elif (x < 1024 * 1024 + 1):
    stfc.append(x)
stfc = np.array(stfc)
# print min(stfc)
# print max(stfc)
# print len(stfc[stfc == 0])
# print len(stfc[stfc > 10 ** 3])
# print len(stfc[stfc > 10 ** 5])

timestamp = {}
groupbase = {}
MS = 1000 * 1000
for h in range(1, H + 1):
  timestamp[h] = int(random.random() * MS * 2)

for g in range(1, N + 1):
  groupbase[g] = int(random.random() * MS)

largest = 0;
p = 0.25
for g in range(1, N + 1):
  m = smember[g - 1]
  t = stfc[g - 1]
  # randomly placed VM
  # hosts = np.random.randint(H - 1, size = m) + 1
  # nearby placed VM
  host_0 = random.randint(1, H - m + 1)
  nhosts = []
  total = t
  for h in range(host_0, host_0 + m): # hosts : range(host_0, host_0 + m)
    nhosts.append(h)
    print '%d %d subscribe %d' % (h, groupbase[g], g)
    if (t > 0):
      size_h = random.randint(0, t)
      if h == host_0 + m - 1 : # hosts[m - 1] : host_0 + m - 1
        size_h = t
      t = t - size_h
      total_h = size_h
      while size_h > 0:
        cs = nextchunk(size_h, total)
        x = x + cs
        if (timestamp[h] < groupbase[g]):
          timestamp[h] = groupbase[g]
        timestamp[h] = timestamp[h] + chunk_gap(cs)
        print '%d %d post %d %d' % (h, timestamp[h], g, cs)
        if (timestamp[h] > largest):
          largest = timestamp[h]
        size_h = size_h - cs
  k = m
  for i in range(0, k):
    time = int(random.random() * (largest - groupbase[g])) + groupbase[g]
    if random.random() > p and len(nhosts) > 0:
      event = nhosts[random.randint(0, len(nhosts) - 1)]
      print '%d %d unsubscribe %d' % (event, time, g)
      nhosts = [ i for i in nhosts if i != event]
    else:
      event = random.randint(1, H)
      while event in nhosts:
        event = random.randint(1, H)
      print '%d %d subscribe %d' % (event, time, g)
      nhosts.append(event)
