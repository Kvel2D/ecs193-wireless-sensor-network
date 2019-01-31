import simpy
import random
import math

RANDOM_SEED = 29
SIM_TIME = 100000 #1000 minutes = 1000000 msec/ticks
MU = 1
FRAME = 1000

packet_number = 0
queue_len = 0
rate = 1/15000

ACK_TIME = 5
TRANSMIT_SLOT = 9



""" Packet class """
class Packet:
    def __init__(self, identifier, arrival_time):
        self.identifier = identifier
        self.arrival_time = arrival_time


def create_packet(env, arrival_rate, n0):
    global packet_number
    global queue_len
    while True:
        # Infinite loop for generating packets
        # change to every time increment, then determine for every host is there a packet arrival
        yield env.timeout(random.expovariate(arrival_rate))
        print("pkr generation: {0}".format(env.now))

        packet_number += 1
        arrival_time = env.now
        # print(self.num_pkt_total, "packet arrival")
        new_packet = Packet(packet_number, arrival_time)
        #if self.flag_processing == 0:
        #    self.flag_processing = 1
        #    idle_period = env.now - self.start_idle_time
        #    self.Server_Idle_Periods.addNumber(idle_period)
        # print("Idle period of length {0} ended".format(idle_period))
        queue_len += 1
        #env.process(process_packet(env, new_packet))
        n0.increment_queue(new_packet)


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

    def transmit(self, env):
        while True:
            idle_start = env.now
            while (env.now - idle_start) < FRAME:
                sleep_time = random.expovariate(self.transmit_rate)
                self.idle_time += sleep_time
                yield env.timeout(sleep_time)
                self.start_time = env.now
                if self.queue > 0:
                    try:
                        print("start transmitting at: {0}".format(env.now))
                        self.transmitting = 1
                        yield env.timeout(TRANSMIT_SLOT)
                        self.transmitting = 0
                        print("stop transmitting at {0}".format(env.now))
                    except simpy.Interrupt as i:
                        if i.cause == "ack":
                            print("ack received at {0}, queue: {1}".format(env.now, self.queue))
                            self.transmitting = 0
                            self.receive_ack(env)
                            self.decrement_queue()
                            yield env.timeout(TRANSMIT_SLOT)
                            idle_start = FRAME

    def transmit_ack(self):
        yield env.timeout(ACK_TIME)

    def receive(self, env):
        while True:
            sleep_time = random.expovariate(self.wake_up_rate)
            self.idle_time += sleep_time
            yield env.timeout(sleep_time)
            #yield env.timeout(random.expovariate(self.wake_up_rate))
            #print("start receiving at: {0}".format(env.now))
            start_time = env.now
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

    def decrement_queue(self):
        self.queue -= 1

class WatchNode:
    def __init__(self, env, n0, n1):
        self.num_successful_packet = 0
        self.n0 = n0
        self.n1 = n1

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

if __name__ == '__main__':
    env = simpy.Environment()
    t_rate = 1/1000
    r_rate = 1/50
    n0 = Node(t_rate, r_rate, 0, env)
    n1 = Node(t_rate, r_rate, 1, env)
    watcher = WatchNode(env, n0, n1)

    env.process(create_packet(env, rate, n0))
    tx = env.process(n0.transmit(env))
    rx = env.process(n1.receive(env))
    env.process(watcher.checkSuccess(env, tx, rx))
    env.run(until=SIM_TIME)

    print("---results---")
    print("number of packets generated: {0}".format(packet_number))
    print("successful transmition rate: {0}".format(watcher.num_successful_packet / packet_number))
    print("node 0 sleep time: {0}".format(n0.idle_time))
    print("node 1 sleep time: {0}".format(n1.idle_time))