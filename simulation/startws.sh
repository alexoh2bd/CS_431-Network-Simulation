#!/bin/bash





ssh alexoh@192.168.64.2 'tshark -l -i tap1 -w -' | /Applications/Wireshark.app/Contents/MacOS/Wireshark -k -i -
