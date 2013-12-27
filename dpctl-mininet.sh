#! /bin/sh

sudo dpctl del-flows tcp:127.0.0.1:6634 
sudo dpctl del-flows tcp:127.0.0.1:6635
sudo dpctl del-flows tcp:127.0.0.1:6636

# sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=1,idle_timeout=0,actions=output:3
# sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=2,idle_timeout=0,actions=output:2,output:1,output:3
# sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=3,idle_timeout=0,actions=output:1,output:2

sudo dpctl add-flow tcp:127.0.0.1:6634 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2
# sudo dpctl add-flow tcp:127.0.0.1:6634 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1
sudo dpctl add-flow tcp:127.0.0.1:6635 nw_dst=224.1.1.1,idle_timeout=0,actions=output:in_port,output:2,output:3
# sudo dpctl add-flow tcp:127.0.0.1:6635 in_port=3,idle_timeout=0,actions=output:2,output:3
# sudo dpctl add-flow tcp:127.0.0.1:6635 in_port=1,idle_timeout=0,actions=output:2,output:3
sudo dpctl add-flow tcp:127.0.0.1:6636 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1,output:2
# sudo dpctl add-flow tcp:127.0.0.1:6636 nw_dst=224.1.1.1,idle_timeout=0,actions=output:1
