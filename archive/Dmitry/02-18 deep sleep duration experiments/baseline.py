import simpy
import random
import math

# record tx/rx time separately
# simulate 4 nodes but record second node's time only, as a float
# record rate as the x part in 1/x, not (1/x) value itself 

SIM_TIME = 12 * 60 * 60 * 1000

packet_number = 0
TIME_BETWEEN_PACKETS = 15 * 60 * 1000

TX_SLEEP_RATE = 1000
RX_SLEEP_RATE = 1000
NODE_COUNT = 4

def int_from_range(range):
    return random.randint(range[0], range[1])

ACK_TIME = 4
TRANSMIT_SLOT = 8
RECEIVE_SLOT = 4

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

        self.tx_time_total = 0
        self.rx_time_total = 0

        self.packet_list = []

    def run(self, env, parent, child):
        while True:
            if (len(self.packet_list) > 0) and (child is not None):

                sleep_time = 0
                sleep_time = random.expovariate(1.0 / TX_SLEEP_RATE)

                yield env.timeout(sleep_time)

                tx_start_timestamp = env.now

                # print('%s> node %d starts transmitting' % (format_time(env.now), self.id))
                self.transmitting = True
                yield env.timeout(TRANSMIT_SLOT)
                self.transmitting = False

                yield env.timeout(ACK_TIME)

                self.tx_time_total += (env.now - tx_start_timestamp)
            else:
                sleep_time = 0
                sleep_time = random.expovariate(1.0 / RX_SLEEP_RATE)
                yield env.timeout(sleep_time)

                rx_start_timestamp = env.now

                # print('%s> node %d starts receiving' % (format_time(env.now), self.id))

                for i in range(RECEIVE_SLOT):
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

                self.rx_time_total += (env.now - rx_start_timestamp)


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

    file.write("%d," % TX_SLEEP_RATE)
    file.write("%d," % RX_SLEEP_RATE)

    tx_time_percent = nodes[1].tx_time_total / SIM_TIME * 100
    rx_time_percent = nodes[1].rx_time_total / SIM_TIME * 100
    file.write("%f," % tx_time_percent)
    file.write("%f," % rx_time_percent)

    total_packet_create_to_gateway_time = 0
    for packet in nodes[NODE_COUNT - 1].packet_list:
        create_to_gateway_time = packet.gateway_time - packet.create_time
        total_packet_create_to_gateway_time += create_to_gateway_time
    avg_packet_create_to_gateway_time = total_packet_create_to_gateway_time / len(nodes[NODE_COUNT - 1].packet_list)
    # NOTE: (NODE_COUNT - 1) because for N nodes, there are (N-1) connections
    avg_packet_node_to_node_time = avg_packet_create_to_gateway_time / (NODE_COUNT - 1)

    file.write('%f,' % avg_packet_node_to_node_time)
    file.write('%s\n' % format_time(avg_packet_node_to_node_time))
    file.close()


sleep_rates = [400, 1000, 2000]

for rx_rate_i in range(len(sleep_rates)):
    for tx_rate_i in range(len(sleep_rates)):
        TX_SLEEP_RATE = sleep_rates[tx_rate_i]
        RX_SLEEP_RATE = sleep_rates[rx_rate_i]
        packet_number = 0

        simulate()

