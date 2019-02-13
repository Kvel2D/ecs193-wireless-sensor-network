import simpy
import random
import math
import numpy as np

SIM_TIME = 6 * 60 * 60 * 1000
FRAME = 1000

packet_number = 0
TIME_BETWEEN_PACKETS = 15 * 60 * 1000

EXPOVARIATE = 0
RANDINT = 1
#RANDOMIZATION_SCHEME = EXPOVARIATE

# variables for fix slot
RANDOMIZATION_SCHEME = 1
SLOT_TIME = 9
#TX_SLEEP_RATE = 0.99
#RX_SLEEP_RATE = 0.99

TX_SLEEP_RATES = np.arange(0.98, 0.998, 0.003)
RX_SLEEP_RATES = np.arange(0.98, 0.998, 0.003)

#offset = [1, 2,3,4,5,6,7,8,9]

#TX_SLEEP_RATE = 1/1000
#RX_SLEEP_RATE = 1/1000
TX_SLEEP_RANGE = (500, 750)
RX_SLEEP_RANGE = (500, 750)

def int_from_range(range):
    return random.randint(range[0], range[1])

ACK_TIME = 5
TRANSMIT_SLOT = 9

def format_time(time: int):
    milliseconds = time % 1000
    total_seconds = time / 1000
    seconds = total_seconds % 60 
    minutes = total_seconds / 60 % 60
    hours = total_seconds / 60 / 60 % 60
    return '%d:%d:%d:%d' % (hours, minutes, seconds, milliseconds)


class Packet:
    def __init__(self, identifier, arrival_time):
        self.identifier = identifier
        self.arrival_time = arrival_time
        self.transmit_time = 0


class Node:
    def __init__(self, id):
        self.id = id
        self.transmitting = False

        self.sleep_time_total = 0
        self.tx_mode_ticks = 0

        self.packet_list = []

    def run(self, env, parent, child, tx, rx):
        # make offset here

        while True:
            if (len(self.packet_list) > 0) and (child is not None):
                tx_cycle_start_time = env.now

                sleep_time = 0
                if RANDOMIZATION_SCHEME == EXPOVARIATE:
                    sleep_time = random.expovariate(tx)
                elif RANDOMIZATION_SCHEME == 1:
                    while (random.random() < tx) and (SIM_TIME > self.sleep_time_total + sleep_time):
                        sleep_time += SLOT_TIME
                else :
                    sleep_time = int_from_range(TX_SLEEP_RANGE)

                self.sleep_time_total += sleep_time
                yield env.timeout(sleep_time)

                self.packet_list[0].transmit_time += sleep_time

                #print('%s> node %d starts transmitting' % (format_time(env.now), self.id))
                self.transmitting = True
                yield env.timeout(TRANSMIT_SLOT)
                self.transmitting = False

                yield env.timeout(ACK_TIME)

                self.tx_mode_ticks += env.now - tx_cycle_start_time
            else:
                sleep_time = 0
                if RANDOMIZATION_SCHEME == EXPOVARIATE:
                    sleep_time = random.expovariate(rx)
                elif RANDOMIZATION_SCHEME == 1:
                    while (random.random() < rx) and (SIM_TIME > self.sleep_time_total + sleep_time):
                        sleep_time += SLOT_TIME
                else :
                    sleep_time = int_from_range(RX_SLEEP_RANGE)
                self.sleep_time_total += sleep_time
                yield env.timeout(sleep_time)

                #print('%s> node %d starts receiving' % (format_time(env.now), self.id))

                for i in range(TRANSMIT_SLOT):
                    if (parent is not None) and (parent.transmitting == True):
                        break
                    else:
                        yield env.timeout(1)

                transmission_ticks_recorded = 0
                while (parent is not None) and (parent.transmitting == True):
                    transmission_ticks_recorded += 1
                    yield env.timeout(1)

                if transmission_ticks_recorded == TRANSMIT_SLOT:
                    # print('%s> node %d received packet' % (format_time(env.now), self.id))
                    self.packet_list.append(parent.packet_list.pop(0))
                    self.packet_list[0].transmit_time += TRANSMIT_SLOT + ACK_TIME

                    yield env.timeout(ACK_TIME)

def create_packet(node, env):
    global packet_number
    while True:
        # Infinite loop for generating packets
        #yield env.timeout(random.expovariate(arrival_rate))
        #print("pkr generation: {0}".format(env.now))
        yield env.timeout(TIME_BETWEEN_PACKETS)

        # print('%s> new packet' % format_time(env.now))

        arrival_time = env.now
        new_packet = Packet(packet_number, arrival_time)
        packet_number += 1
        node.packet_list.append(new_packet)


if __name__ == '__main__':
    f = open("fixSlot_varying_rate.csv", "w")
    f.write("tx_rate,rx_rate,pkt_success,n0_awake_time,n1_awake_time,avg_transmission_time_per_pkt\n")
    x, y = np.meshgrid(TX_SLEEP_RATES,RX_SLEEP_RATES)

    #print(x, y)

    for col, row in zip(x, y):
        for tx_rate , rx_rate in zip(col, row):
            f.write("{0},{1},".format(tx_rate , rx_rate))
            packet_number = 0

            env = simpy.Environment()
            n0 = Node(0)
            n1 = Node(1)

            env.process(create_packet(n0, env))
            tx = env.process(n0.run(env, None, n1, tx_rate, rx_rate))
            rx = env.process(n1.run(env, n0, None, tx_rate, rx_rate))
            env.run(until=SIM_TIME)

            print("\n---INPUT---")
            print("simulation time: %s" % format_time(SIM_TIME))
            print("time between packets: %s" % format_time(TIME_BETWEEN_PACKETS))


            if RANDOMIZATION_SCHEME == EXPOVARIATE:
                print("randomization scheme: expovariate")
                print("tx sleep rate: %f" % tx_rate)
                print("rx sleep rate: %f" % rx_rate)
            elif RANDOMIZATION_SCHEME == 1:
                print("randomization scheme: fix slot")
                print("tx sleep rate: %f" % tx_rate)
                print("rx sleep rate: %f" % rx_rate)
            else:
                print("randomization scheme: randint")
                print("tx sleep rate: %f" % tx_rate)
                print("rx sleep rate: %f" % rx_rate)

            print("\n---RESULTS---")
            pkt_success = len(n1.packet_list) / packet_number
            n0_awake_time = (SIM_TIME - n0.sleep_time_total) / SIM_TIME * 100
            n1_awake_time = (SIM_TIME - n1.sleep_time_total) / SIM_TIME * 100
            print("packets: %d/%d" % (len(n1.packet_list), packet_number))
            print("node 0 awake time: {0:.5f}%".format(n0_awake_time))
            print("node 1 awake time: {0:.5f}%\n".format(n1_awake_time))

            avg_time_to_transmit_packet = n0.tx_mode_ticks / packet_number
            print('avg time to transmit packet = %s' % format_time(avg_time_to_transmit_packet))
            f.write("{0},{1},{2},{3}\n".format(pkt_success, n0_awake_time, n1_awake_time, avg_time_to_transmit_packet))
    f.close()