#! /bin/sh

sudo ovs-ofctl del-flows s1
sudo ovs-ofctl del-flows s2
sudo ovs-ofctl del-flows s3

# sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=1,idle_timeout=0,actions=output:3
# sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=2,idle_timeout=0,actions=output:2,output:1,output:3
# sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=3,idle_timeout=0,actions=output:1,output:2
sudo ovs-ofctl add-flow s1 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2
# sudo ovs-ofctl add-flow s1 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2,in_port
# sudo ovs-ofctl add-flow s1 in_port=1,idle_timeout=0,actions=output:2
# sudo ovs-ofctl add-flow s1 in_port=2,idle_timeout=0,actions=output:1
# sudo dpctl add-flow tcp:127.0.0.1:6634 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1

# sudo ovs-ofctl add-flow s2 nw_dst=224.1.1.1,idle_timeout=0,actions=output:2,output:3
sudo ovs-ofctl add-flow s2 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2,output:3
# sudo ovs-ofctl add-flow s2 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2,output:3,in_port
# sudo dpctl add-flow tcp:127.0.0.1:6635 in_port=3,idle_timeout=0,actions=output:2,output:3
# sudo dpctl add-flow tcp:127.0.0.1:6635 in_port=1,idle_timeout=0,actions=output:2,output:3

sudo ovs-ofctl add-flow s3 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2
# sudo ovs-ofctl add-flow s3 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2,in_port
# sudo dpctl add-flow tcp:127.0.0.1:6636 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1
