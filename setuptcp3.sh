
network="192.168.0.0"
netmask="255.255.255.0"

vde_switch_ctrl_file="/tmp/net2.vde"
tap_device="tap2"
kernel_endpoint_ip="192.168.0.8"

# Use -hub so Wireshark can see unicast frames.
vde_switch -sock "$vde_switch_ctrl_file" -hub -daemon

sudo ifconfig tap2 destroy
sudo ifconfig tap2 create
sudo ifconfig tap2 "$kernel_endpoint_ip" netmask "$netmask" up

sudo vde_plug2tap --sock "$vde_switch_ctrl_file" "$tap_device" --daemon
