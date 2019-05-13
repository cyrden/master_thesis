#!/bin/bash

ITF=gre-mininet

ip tunnel add $ITF mode gre remote 130.104.229.25 local 130.104.229.24 ttl 255
ip link set $ITF up
ip address add dev $ITF 10.104.229.24/25


#!/bin/bash

ITF=gre-router

ip tunnel add $ITF mode gre remote 130.104.229.24 local 130.104.229.25 ttl 255
ip link set $ITF up
ip address add dev $ITF 10.104.229.25/25


