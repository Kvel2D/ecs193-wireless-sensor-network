# ecs193-wireless-sensor-network

# Jiahui Dai

# In the old protocal, 
  sender sends 96 sec (in 24 hours)
  
  receiver listens 864 sec (in 24 hours)

# My Output:

  simulation_num =  96 (packet number in 24 hours)
  
  sender slots per awake =  1
  
  receiver_slots_per_awake =  1
  
  slot_num per frame =  100000.0
  
  sender_awake_chance =  0.014
  
  receiver_awake_chance =  0.014
  
  success num in 96 simulation =  96

  success rate =  1.0

  how many time node 0 send before success =  77.01041666666667
  
  how many time node 1 listen before success =  77.52083333333333

  node 0 send time =  0:0:0:693 = 1.4430000439814814 %
  
  node 0 awake time =  0:0:12:293
  
  node 1 awake time =  0:0:12:606 = 1.4007268958333332 %
  
  avg time to transmit packet = 0:0:49:367

# explanation
my simulator, which simulates the 15 mins transmitting event from the sender to the receiver. 

My understanding:
We assume a data stream, which is only 1 packet, is sent every 15 minutes and it passes from source to node0 to node1 to gateway. The node0 is a receiver, which receives the packet from source, and then become a sender, which sends the packet to node1. The node1 do the same thing except it receives from node0 and sends to gateway. Finally, they are doing receiver's work in the rest of 15 mins.

I think node0 and node1 will use approximately same amount of time and consume approximately same amount of energy, because they are doing the same things.


TODO: make graph plotting

In the old protocal, in every 15 mins, sender will send 1000 ticks for ad and 9 ticks for sending a packet, receiver will be awake 8910 ticks(990 times and every time is 9 ticks for listening) and 9 ticks for receiveing the packet.


