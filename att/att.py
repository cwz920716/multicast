#!/usr/bin/python

from operator import itemgetter
import cPickle as pk
import re 
import os

def cdf_generator( l, start, step ):
  l.sort()
  cdf = []
  cnt = 0
  gap = start
  for e in l:
    if e > gap:
      while gap < e:
        # print gap
        cdf.append(cnt)
        print '%d\t%d' % (gap, cnt)
        gap = gap + step
    cnt = cnt + 1
    # print cnt
  cdf.append(cnt)
  print '%d\t%d' % (gap, cnt)
  return cdf

def sort_set( l ): 
    """ Sort the given iterable in the way that humans expect.""" 
    convert = lambda text: int(text) if text.isdigit() else text 
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ] 
    return sorted(l, key = alphanum_key)

fi = file('/home/mininet/trace/att/data20120618-24')
fname=fi.name
date=fname.split('data')[1]

(Switch, VLAN, VM, VMinVL, tfc_mat, tfc_port, tfc_ext, x_tfc, n_DHCP, n_DNS)=pk.load(fi)
fi.close()

fi = file('/home/mininet/trace/att/Location0705')
(BFVL, BFlg, VMtoLo)=pk.load(fi)
fi.close()

# print type(VMinVL)
n_group = []
for e in VMinVL.keys():
  n_group.append(len(VMinVL[e]))
  # print '%s\t%r' % (e, len(VMinVL[e]))
  pass

# cdf_generator(n_group, 0, 10)

# print len(tfc_mat.keys())
tfc_vlan = []
for e in tfc_mat.keys():
  # print '%s\t' % e,
  sum_e = 0
  for t in tfc_mat[e].keys():
    sum_e = sum_e + int(tfc_mat[e][t])
  # print '%d' % sum_e
  tfc_vlan.append(sum_e)
  pass

# cdf_generator(tfc_vlan, 1, 10)

# print len(VM)
tfc_vm_o = {}
tfc_vm_i = {}
for vm in VM:
  tfc_vm_i[vm] = 0
  tfc_vm_o[vm] = 0

for vlan in VLAN:
  for flow in tfc_mat[vlan].keys():
    src = flow[0]
    dest = flow[1]
    tfc_vm_o[src] += tfc_mat[vlan][flow]
    tfc_vm_i[dest] += tfc_mat[vlan][flow]

tfc_vm_o_list = []
tfc_vm_i_list = []
for vm in VM:
  tfc_vm_o_list.append(tfc_vm_o[vm])
  tfc_vm_i_list.append(tfc_vm_i[vm])
  # print '%r\t%r' % (e, tfc_vm_o[e])
cdf_generator(tfc_vm_o_list, 0, 100000)
