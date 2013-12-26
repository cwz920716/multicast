dpctl add-flow tcp:127.0.0.1:6634 in_port=1,actions=output:3
dpctl add-flow tcp:127.0.0.1:6634 in_port=2,actions=output:3
dpctl add-flow tcp:127.0.0.1:6634 in_port=3,actions=output:1,2