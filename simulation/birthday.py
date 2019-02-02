import simpy
import random
import math

#RANDOM_SEED = 29
SIM_TIME = (1 * 60 * 60 * 1000) #1000 minutes = 1000000 sec
FRAME = 1000

packet_number = 0
rate = 1/(15 * 60 * 1000) #15 minutes

ACK_TIME = 5
TRANSMIT_SLOT = 9



""" Packet class """
class Packet:
    def __init__(self, identifier, arrival_time):
        self.identifier = identifier
        self.arrival_time = arrival_time
        self.transmit_time = 0


class Node:

    def __init__(self, t_rate, r_rate, id, env):
        self.wake_up_rate = r_rate
        self.transmit_rate = t_rate
        self.id = id
        self.queue = 0
        self.transmitting = 0
        self.receiving = 0
        self.env = env

        self.awake_time = 0
        self.idle_time = 0
        self.start_time = 0

        self.transmit_attempt = 0
        self.packet_list = []

        self.awake = False

    def transmit(self, env, parent, child):
        self.packet_list[0].transmit_time += sleep_time
        self.transmit_attempt += 1


        self.transmitting = 1
        yield env.timeout(TRANSMIT_SLOT)
        self.transmitting = 0

        self.receive_ack(env)

        self.packet_list[0].transmit_time += TRANSMIT_SLOT + ACK_TIME

    def receive(self, env, parent, child):
        sleep_time = random.expovariate(self.wake_up_rate)
        self.idle_time += sleep_time
        yield env.timeout(sleep_time)

        #yield env.timeout(random.expovariate(self.wake_up_rate))
        #print("start receiving at: {0}".format(env.now))

        for i in range(TRANSMIT_SLOT):
            if self.parent.transmitting == 1:
                break
            else:
                yield self.env.timeout(1)

        transmission_ticks_recorded = 0
        while self.parent.transmitting == 1:
            transmission_ticks_recorded += 1
            yield env.timeout(1)

        if transmission_ticks_recorded == TRANSMIT_SLOT:
            
            yield env.timeout(ACK_TIME)

            self.packet_list.append(self.parent.packet_list.pop(0))
            print('transmission success')

    def run(self, env, parent, child):
        while True:
            sleep_time = random.expovariate(self.transmit_rate)
            yield env.timeout(sleep_time)
            self.idle_time += sleep_time
            self.start_time = env.now

            if (len(self.packet_list) > 0) & (child != None):
                self.transmit(env, parent, child)
            else:
                self.receive(env, parent, child)
        
    def transmit_ack(self):
        yield env.timeout(ACK_TIME)
            
    def receive_ack(self, env):
        yield env.timeout(ACK_TIME)


def create_packet(node, env, arrival_rate):
    global packet_number
    while True:
        # Infinite loop for generating packets
        #yield env.timeout(random.expovariate(arrival_rate))
        #print("pkr generation: {0}".format(env.now))
        yield env.timeout(900000)

        print('new packet')

        packet_number += 1
        arrival_time = env.now
        new_packet = Packet(packet_number, arrival_time)
        node.packet_list.append(new_packet)


if __name__ == '__main__':
    env = simpy.Environment()
    t_rate = 1/4000
    r_rate = 1/100
    n0 = Node(t_rate, r_rate, 0, env)
    n1 = Node(t_rate, r_rate, 1, env)

    env.process(create_packet(n0, env, rate))
    tx = env.process(n0.run(env, None, n1))
    rx = env.process(n1.run(env, n0, None))
    env.run(until=SIM_TIME)

    print("\n---results---")
    print("number of packets generated: {0}".format(packet_number))
    # print("number of packets transmitted: {0}".format(watcher.num_successful_packet))
    # print("successful transmition rate: {0:.3f}%".format(watcher.num_successful_packet / packet_number))
    print("number of attempted transmission rounds: {0}".format(n0.transmit_attempt))
    print("node 0 sleep time: {0:.5f}%".format(n0.idle_time / SIM_TIME))
    print("node 1 sleep time: {0:.5f}%\n".format(n1.idle_time / SIM_TIME))

    total_transmit_time = 0
    # for p in watcher.packet_list:
    #     print("{0}: {1:.3f}".format(p.identifier, p.transmit_time))
    #     total_transmit_time += p.transmit_time
    # avg_transmit_time = total_transmit_time / packet_number
    # print("avg time to transmit: {0:.3f}".format(avg_transmit_time))