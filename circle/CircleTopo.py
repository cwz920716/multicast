#! /usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import CPULimitedHost, Controller, RemoteController
from mininet.link import TCLink
from mininet.util import irange,dumpNodeConnections
from mininet.log import setLogLevel
from mininet.cli import CLI

class CircleTopo(Topo):
  "Circle topology of k switches, with m host per switch."
   
  def __init__(self, k=3, h=2, **opts):
    """Init.
       k: number of switches
       h: number of hosts per switch
       blank..."""
    super(CircleTopo, self).__init__(**opts)
    
    self.k = k
    self.h = h

    firstSwitch = None
    lastSwitch = None
    for i in irange(1, k):
      switch = self.addSwitch('s%s' % i)
      for j in irange(1, h):
        host = self.addHost('h%s' % ((i - 1) * h + j))
        self.addLink(host, switch)
      if lastSwitch:
        self.addLink(switch, lastSwitch)
      else:
        firstSwitch = switch
      lastSwitch = switch
    self.addLink(firstSwitch, lastSwitch)

def simpleTest():
  topo = CircleTopo()
  net = Mininet(topo, controller=lambda name: RemoteController( name, ip='127.0.0.1' ))
  net.start()
  print "Dumping host connections"
  dumpNodeConnections(net.hosts)
  print "Testing host CLI"
  for i in irange(1, topo.k * topo.h):
    h = net.get('h%i' % i)
    h.cmd( 'ip route add 0.0.0.0/0 dev h%i-eth0' % i )
    h.cmd( 'python ms.py > /tmp/h%i-output.txt &' % i )
    # result = h.cmd('route -n')
    # print result
  # net.pingAll()
  CLI(net)
  net.stop()

if __name__ == '__main__':
  setLogLevel('info')
  simpleTest()
