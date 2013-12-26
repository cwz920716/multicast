#! /bin/sh

sudo dpctl del-flows tcp:127.0.0.1:6634 

sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=1,idle_timeout=0,actions=output:3
sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=2,idle_timeout=0,actions=output:2,output:1,output:3
sudo dpctl add-flow tcp:127.0.0.1:6634 in_port=3,idle_timeout=0,actions=output:1,output:2
