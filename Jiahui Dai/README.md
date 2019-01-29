# ecs193-wireless-sensor-network

# Jiahui Dai

I finish my basic simulator, which simulates the transmitting event from the sender to the receiver. 

My understanding:
We assume a data stream, which is only 1 packet, is sent every 15 minutes and it passes from source to node0 to node1 to gateway. The node0 is a receiver, which receives the packet from source, and then become a sender, which sends the packet to node1. The node1 do the same thing except it receives from node0 and sends to gateway. Finally, they are receiver in the rest of 15 mins.

I think node0 and node1 will use approximately same amount of time and consume approximately same amount of energy, because they are doing the same things, but the final success rate need to be power of 3.

Receiver awake num in 15 mins = awake chance * 900000 / ticks per check 

Total consumption of node0 = node1 = how many times it sends * consumption per send + how many times it wakes up to check * consumption per check + Receiver awake num in 15 mins * consumption per check


TODO: make graph plotting
