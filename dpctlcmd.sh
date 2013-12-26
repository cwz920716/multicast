#! /bin/sh

sudo dpctl unix:/tmp/s1 flow-mod table=0,cmd=add in_port=1 apply:output=4
sudo dpctl unix:/tmp/s1 flow-mod table=0,cmd=add in_port=2 apply:output=4

sudo dpctl unix:/tmp/s1 group-mod cmd=add,type=all,group=1 weight=0,port=any,group=any output=1 weight=0,port=any,group=any output=2
sudo dpctl unix:/tmp/s1 flow-mod table=0,cmd=add in_port=4 apply:group=1
