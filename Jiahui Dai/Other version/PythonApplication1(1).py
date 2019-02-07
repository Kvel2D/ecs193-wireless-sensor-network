import simpy
import random
#import matplotlib.pyplot as plt

# two nodes, simulation runs for N ticks, nodes can wake up each tick with some probability, 
# a match happens if sender sends after ot at the same time receiver wake up

# we simulate one packet transmit in 15 mins (900000 ticks)
# In the old protocal every 15 mins, sender will send 1000 ticks, receiver will be awake 8910 ticks

ticks_per_slot = 20 


simulation_num = 100
# parameter 
slot_num = 900000 / ticks_per_slot
#slot_num = 1000
sender_awake_chance = 0.05
receiver_awake_chance = 0.05

sender_slots_per_awake = 1
receiver_slots_per_awake = 2

awake = [False, False]

# controller variables
sender_switch = sender_slots_per_awake - 1
receiver_switch = receiver_slots_per_awake - 1
success = False

# output variables
match_count = 0
same_time_match_count = 0
sender_awake_slots_num = 0
receiver_awake_slots_num = 0
succeed_time = 0

def node(env):
    global sender_switch, receiver_switch, success

    while (success == False):
        # Once the node wake up, it will be awake in N slots
        sender_switch += 1
        if(sender_switch == sender_slots_per_awake):
            awake[0] = False
            awake[0] = (random.random() < sender_awake_chance)

        receiver_switch += 1
        if(receiver_switch == receiver_slots_per_awake):
            awake[1] = False
            awake[1] = (random.random() < receiver_awake_chance)

        
        yield env.timeout(1)

def watch_nodes(env):
    global match_count, sender_switch, receiver_switch, success, sender_awake_slots_num, receiver_awake_slots_num, same_time_match_count

    # when success = True, this sending cycle ends
    while (success == False):
        # when sender must sends after or at the same time receiver receives
        if (awake[0] == True and awake[1] == True and sender_switch == sender_slots_per_awake):
            match_count += 1
            success = True

            if(receiver_switch == receiver_slots_per_awake):
                same_time_match_count += 1

                # the sender send and receiver wake up at the same time. because drift, there is 50% fail chance
                if(random.random() < 0.5):
                    success = False
                    match_count -= 1

            # add remaining awake slots to make the output neat
            sender_awake_slots_num += sender_slots_per_awake - 1
            receiver_awake_slots_num += receiver_slots_per_awake - 1

        # tester
        #print(awake)
        #print(success)

        if(awake[0] == True):
            sender_awake_slots_num += 1
        if(awake[1] == True):
            receiver_awake_slots_num += 1

        # reset
        if(sender_switch == sender_slots_per_awake):
            sender_switch = 0

        if(receiver_switch == receiver_slots_per_awake):
            receiver_switch = 0
        # reset ends

        yield env.timeout(1)


for i in range(simulation_num):
    success = False
    sender_switch = sender_slots_per_awake - 1
    receiver_switch = receiver_slots_per_awake - 1
    
    env = simpy.Environment()

    env.process(node(env))
    # NOTE: watch_nodes() must be created after node()'s so that it's run after them
    env.process(watch_nodes(env))
    if(success == False):
        succeed_time += 1
    env.run(until=slot_num)
    
    #print(simulation_num)

print("simulation_num = ", simulation_num)

print("sender slots per awake = ", sender_slots_per_awake)
print("receiver_slots_per_awake = ", receiver_slots_per_awake)
print("slot_num per frame = ", slot_num)
print("sender_awake_chance = ", sender_awake_chance)
print("receiver_awake_chance = ", receiver_awake_chance)
print("success num in", simulation_num, "simulation = ", match_count)
print("same_time_match_count = ", same_time_match_count)

print("\nsuccess rate = ", (match_count / simulation_num))
print("succeed_time = ", succeed_time)
average_succeed_time = succeed_time/simulation_num*ticks_per_slot
print("average_succeed_time =", average_succeed_time / 1000,"secs")

print("node 0 awake time = ", (sender_awake_slots_num/simulation_num))
print("node 1 awake time = ", (receiver_awake_slots_num/simulation_num))

print("\nI assume ticks per slot = ", ticks_per_slot)

# per slot, sender uses 9 ticks to send packet, 2 ticks to switch mode, 9 ticks to wait for ak
# per slot, receiver uses 20 ticks to wait for packet

print("node 0 ticks for send = ", (sender_awake_slots_num/simulation_num)*sender_slots_per_awake*9)
print("node 0 ticks for wake up = ", (sender_awake_slots_num/simulation_num)*sender_slots_per_awake*9)
print("node 1 ticks for wake up = ", (receiver_awake_slots_num/simulation_num)*receiver_slots_per_awake*ticks_per_slot )

