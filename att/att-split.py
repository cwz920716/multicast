#!/usr/bin/python

from operator import itemgetter
import cPickle as pk
import re 
import os, sys


def output2f(fname, line):
  if os.path.exists(fname):
    fout = file(fname, 'a')
  else:
    fout = file(fname, 'w')
  fout.write(line)
  fout.close()

def pdf_generator( l, start, step, vlan ):
  l.sort()
  cdf = []
  cnt = 0
  gap = start
  for e in l:
    if e > gap:
      while gap < e:
        # print gap
        cdf.append(cnt)
        output2f('groups-post-pdf/%r.txt' % int(vlan), '%d\t%d\n' % (gap, cnt))
        cnt = 0
        gap = gap + step
    cnt = cnt + 1
    # print cnt
  cdf.append(cnt)
  output2f('groups-post-pdf/%r.txt' % int(vlan), '%d\t%d\n' % (gap, cnt))
  return cdf

def cdf_generator( l, start, step, vlan ):
  l.sort()
  cdf = []
  cnt = 0
  gap = start
  for e in l:
    if e > gap:
      while gap < e:
        # print gap
        cdf.append(cnt)
        output2f('groups-post-cdf/%r.txt' % int(vlan), '%d\t%d\n' % (gap, cnt))
        gap = gap + step
    cnt = cnt + 1
    # print cnt
  cdf.append(cnt)
  output2f('groups-post-cdf/%r.txt' % int(vlan), '%d\t%d\n' % (gap, cnt))
  return cdf


fi = file('/home/mininet/trace/att/data20120618-24')
fname=fi.name
date=fname.split('data')[1]

(Switch, VLAN, VM, VMinVL, tfc_mat, tfc_port, tfc_ext, x_tfc, n_DHCP, n_DNS)=pk.load(fi)
fi.close()

for vlan in VLAN:
  mat = tfc_mat[vlan]
  if not len(mat) == 0:
    vm_o = {}
    for e in VMinVL[vlan]:
      vm_o[e] = 0
    for flow in mat.keys():
      src = flow[0]
      vm_o[src] += mat[flow]
    vm_o_list = []
    for vm in vm_o:
      # output2f('groups-post/%r.txt' % int(vlan), '%r\t%r\n' % (str(vm), vm_o[vm]))
      vm_o_list.append(vm_o[vm])
    # cdf_generator(vm_o_list, 0, 10000, vlan)
    pdf_generator(vm_o_list, 0, 10000, vlan)


