#!/bin/bash





ssh alexoh@192.168.64.3 'tshark -l -i tap1 -w -' | /Applications/Wireshark.app/Contents/MacOS/Wireshark -k -i -
ssh alexoh@192.168.64.3 'tshark -l -i tap0 -w -' | /Applications/Wireshark.app/Contents/MacOS/Wireshark -k -i -
