import simpy
import random
import math

#RANDOM_SEED = 29
SIM_TIME = 36000000 #1000 minutes = 1000000 sec
FRAME = 1000

packet_number = 0
rate = 1/900000 #15 minutes

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
        self.pkt = None

    def transmit(self, env):
        while True:
            idle_start = env.now
            while (env.now - idle_start) < FRAME:
                sleep_time = random.expovariate(self.transmit_rate)
                yield env.timeout(sleep_time)
                self.idle_time += sleep_time
                self.start_time = env.now

                if self.queue > 0:
                    self.pkt.transmit_time += sleep_time
                    self.transmit_attempt += 1
                    try:
                        #print("start transmitting at: {0}".format(env.now))
                        self.transmitting = 1
                        yield env.timeout(TRANSMIT_SLOT)
                        self.transmitting = 0
                        #print("stop transmitting at {0}".format(env.now))
                    except simpy.Interrupt as i:
                        if i.cause == "ack":
                            print("ack received at {0}, queue: {1}".format(env.now, self.queue))
                            self.transmitting = 0
                            self.receive_ack(env)
                            self.decrement_queue()
                            yield env.timeout(TRANSMIT_SLOT)
                            #print("stop transmitting at {0}".format(env.now))
                            #break
                    self.pkt.transmit_time += TRANSMIT_SLOT

    def transmit_ack(self):
        yield env.timeout(ACK_TIME)

    def receive(self, env):
        while True:
            sleep_time = random.expovariate(self.wake_up_rate)
            self.idle_time += sleep_time
            yield env.timeout(sleep_time)
            #yield env.timeout(random.expovariate(self.wake_up_rate))
            #print("start receiving at: {0}".format(env.now))
            self.start_time = env.now
            try:
                self.receiving = 1
                yield env.timeout(TRANSMIT_SLOT)
                self.receiving = 0
            except simpy.Interrupt as i:
                yield env.timeout(TRANSMIT_SLOT)
                self.receiving = 0

            #print("stop receiving: {0}".format(env.now))

    def receive_ack(self, env):
        yield env.timeout(ACK_TIME)

    def increment_queue(self, pkt):
        self.queue += 1
        self.pkt = pkt

    def decrement_queue(self):
        self.queue -= 1

class WatchNode:
    def __init__(self, env, n0, n1):
        self.num_successful_packet = 0
        self.n0 = n0
        self.n1 = n1
        self.packet_list = []

    def checkSuccess(self, env, tx, rx):
        while True:
            if self.n0.transmitting == 1 & self.n1.receiving == 1:
                #if math.ceil(self.n0.start_time) == env.now:
                print("tx started at: {0}".format(math.ceil(self.n0.start_time)))
                print("success at {0}".format(env.now))
                self.num_successful_packet += 1
                tx.interrupt("ack")
                rx.interrupt(self.n0.start_time)
            yield env.timeout(1)

    def create_packet(self, env, arrival_rate):
        global packet_number
        while True:
            # Infinite loop for generating packets
            #yield env.timeout(random.expovariate(arrival_rate))
            #print("pkr generation: {0}".format(env.now))
            yield env.timeout(900000)

            packet_number += 1
            arrival_time = env.now
            new_packet = Packet(packet_number, arrival_time)
            self.packet_list.append(new_packet)
            self.n0.increment_queue(new_packet)


if __name__ == '__main__':
    env = simpy.Environment()
    t_rate = 1/4000
    r_rate = 1/100
    n0 = Node(t_rate, r_rate, 0, env)
    n1 = Node(t_rate, r_rate, 1, env)
    watcher = WatchNode(env, n0, n1)

    env.process(watcher.create_packet(env, rate))
    tx = env.process(n0.transmit(env))
    rx = env.process(n1.receive(env))
    env.process(watcher.checkSuccess(env, tx, rx))
    env.run(until=SIM_TIME)

    print("\n---results---")
    print("number of packets generated: {0}".format(packet_number))
    print("number of packets transmitted: {0}".format(watcher.num_successful_packet))
    print("successful transmition rate: {0:.3f}%".format(watcher.num_successful_packet / packet_number))
    print("number of attempted transmission rounds: {0}".format(n0.transmit_attempt))
    print("node 0 sleep time: {0:.5f}%".format(n0.idle_time / SIM_TIME))
    print("node 1 sleep time: {0:.5f}%\n".format(n1.idle_time / SIM_TIME))

    total_transmit_time = 0
    for p in watcher.packet_list:
        print("{0}: {1:.3f}".format(p.identifier, p.transmit_time))
        total_transmit_time += p.transmit_time
    avg_transmit_time = total_transmit_time / packet_number
    print("avg time to transmit: {0:.3f}".format(avg_transmit_time))