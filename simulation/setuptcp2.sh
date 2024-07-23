
network="192.168.0.0"
netmask="255.255.255.0"

vde_switch_ctrl_file="/tmp/net1.vde"
tap_device="tap1"
kernel_endpoint_ip="192.168.0.7"

# Use -hub so Wireshark can see unicast frames.
vde_switch -sock "$vde_switch_ctrl_file" -hub -daemon

sudo ifconfig tap1 destroy
sudo ifconfig tap1 create
sudo ifconfig tap1 "$kernel_endpoint_ip" netmask "$netmask" up

sudo vde_plug2tap --sock "$vde_switch_ctrl_file" "$tap_device" --daemon
