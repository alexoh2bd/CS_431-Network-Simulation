#!/bin/bash

ssh alexoh@192.168.64.2 'sudo ifconfig tap0 create -S'

ssh alexoh@192.168.64.2 'sudo ifconfig tap0 up -S'

ssh alexoh@192.168.64.2 'sudo vde_plug2tap tap0 -S'