# ecs193-wireless-sensor-network

# Jiahui Dai

my simulator, which simulates the 15 mins transmitting event from the sender to the receiver. 

My understanding:
We assume a data stream, which is only 1 packet, is sent every 15 minutes and it passes from source to node0 to node1 to gateway. The node0 is a receiver, which receives the packet from source, and then become a sender, which sends the packet to node1. The node1 do the same thing except it receives from node0 and sends to gateway. Finally, they are doing receiver's work in the rest of 15 mins.

I think node0 and node1 will use approximately same amount of time and consume approximately same amount of energy, because they are doing the same things.


TODO: make graph plotting

In the old protocal, in every 15 mins, sender will send 1000 ticks for ad and 9 ticks for sending a packet, receiver will be awake 8910 ticks(990 times and every time is 9 ticks for listening) and 9 ticks for receiveing the packet.

In the old protocal, sender sends 1 sec, and receiver listens 9 sec.
