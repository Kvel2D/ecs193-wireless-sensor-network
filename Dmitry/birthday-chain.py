import simpy
import random
import math

SIM_TIME = 24 * 60 * 60 * 1000
FRAME = 1000

packet_number = 0
TIME_BETWEEN_PACKETS = 15 * 60 * 1000

EXPOVARIATE = 0
RANDINT = 1
RANDOMIZATION_SCHEME = EXPOVARIATE
TX_SLEEP_RATE = 1/1000
RX_SLEEP_RATE = 1/1000
TX_SLEEP_RANGE = (1000, 1500)
RX_SLEEP_RANGE = (1000, 1500)
NODE_COUNT = 5

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
    def __init__(self, identifier, crete_time):
        self.identifier = identifier
        self.create_time = crete_time
        self.gateway_time = 0


class Node:
    def __init__(self, id):
        self.id = id
        self.transmitting = False

        self.sleep_time_total = 0

        self.packet_list = []

    def run(self, env, parent, child):
        while True:
            if (len(self.packet_list) > 0) and (child is not None):
                tx_cycle_start_time = env.now

                sleep_time = 0
                if RANDOMIZATION_SCHEME == EXPOVARIATE:
                    sleep_time = random.expovariate(TX_SLEEP_RATE)
                else :
                    sleep_time = int_from_range(TX_SLEEP_RANGE)

                self.sleep_time_total += sleep_time
                yield env.timeout(sleep_time)

                # print('%s> node %d starts transmitting' % (format_time(env.now), self.id))
                self.transmitting = True
                yield env.timeout(TRANSMIT_SLOT)
                self.transmitting = False

                yield env.timeout(ACK_TIME)
            else:
                sleep_time = 0
                if RANDOMIZATION_SCHEME == EXPOVARIATE:
                    sleep_time = random.expovariate(RX_SLEEP_RATE)
                else :
                    sleep_time = int_from_range(RX_SLEEP_RANGE)
                self.sleep_time_total += sleep_time
                yield env.timeout(sleep_time)

                # print('%s> node %d starts receiving' % (format_time(env.now), self.id))

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
                    packet = parent.packet_list.pop(0)
                    # If current node has no child, then it is the gateway, mark time for packet
                    if child is None:
                        packet.gateway_time = env.now
                    self.packet_list.append(packet)

                    yield env.timeout(ACK_TIME)


def create_packet(node, env):
    global packet_number
    while True:
        # print('%s> new packet' % format_time(env.now))

        new_packet = Packet(packet_number, env.now)
        packet_number += 1
        node.packet_list.append(new_packet)

        yield env.timeout(TIME_BETWEEN_PACKETS)


def simulate():
    env = simpy.Environment()

    nodes = []
    for i in range(NODE_COUNT):
        nodes.append(Node(i))

    env.process(create_packet(nodes[0], env))
    for i in range(NODE_COUNT):
        parent = None
        if i > 0:
            parent = nodes[i - 1]

        child = None
        if i < NODE_COUNT - 1:
            child = nodes[i + 1]

        env.process(nodes[i].run(env, parent, child))

    env.run(until=SIM_TIME)

    file = open("results.txt", "a")

    print("---INPUT---")
    print("simulation time: %s" % format_time(SIM_TIME))
    print("time between packets: %s" % format_time(TIME_BETWEEN_PACKETS))
    print("node count: %s" % NODE_COUNT)

    if RANDOMIZATION_SCHEME == EXPOVARIATE:
        print("randomization scheme: expovariate")
        print("TX_SLEEP_RATE: %f" % TX_SLEEP_RATE)
        print("RX_SLEEP_RATE: %f" % RX_SLEEP_RATE)
        file.write("%f, " % TX_SLEEP_RATE)
        file.write("%f, " % RX_SLEEP_RATE)
    else:
        print("randomization scheme: randint")
        print('tx range = (%d, %d)' % TX_SLEEP_RANGE)
        print('rx range = (%d, %d)' % RX_SLEEP_RANGE)

    print("\n---RESULTS---\n")
    print("packets: %d/%d" % (len(nodes[NODE_COUNT - 1].packet_list), packet_number))

    for i in range(NODE_COUNT):
        awake_time_percent = (SIM_TIME - nodes[i].sleep_time_total) / SIM_TIME * 100
        print("node[%d] awake time = %f %%" % (i, awake_time_percent))
        file.write("%f, " % ((SIM_TIME - nodes[i].sleep_time_total) / SIM_TIME * 100))

    total_packet_create_to_gateway_time = 0
    for packet in nodes[NODE_COUNT - 1].packet_list:
        create_to_gateway_time = packet.gateway_time - packet.create_time
        total_packet_create_to_gateway_time += create_to_gateway_time
    avg_packet_create_to_gateway_time = total_packet_create_to_gateway_time / packet_number
    # NOTE: (NODE_COUNT - 1) because for N nodes, there are (N-1) connections
    avg_packet_node_to_node_time = avg_packet_create_to_gateway_time / (NODE_COUNT - 1)

    file.write('%s\n' % format_time(avg_packet_create_to_gateway_time))
    file.write('%s\n' % format_time(avg_packet_node_to_node_time))
    print('avg_packet_create_to_gateway_time: %s' % format_time(avg_packet_create_to_gateway_time))
    print('avg_packet_node_to_node_time: %s' % format_time(avg_packet_node_to_node_time))

    file.close()


sleep_rates = [1/2000, 1/1500, 1/1000, 1/500, 1/250]

for tx_rate_i in range(len(sleep_rates)):
    for rx_rate_i in range(len(sleep_rates)):
        TX_SLEEP_RATE = sleep_rates[tx_rate_i]
        RX_SLEEP_RATE = sleep_rates[rx_rate_i]
        packet_number = 0

        simulate()

