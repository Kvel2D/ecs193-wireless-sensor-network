import simpy
import random
import math
import numpy as np

SIM_TIME = 24 * 60 * 1000
PACKET_PERIOD = 5 * 60 * 1000

RX_RATE = 400
TX_RATE = 400

CHILD_COUNT = 1

TX_TRANSMIT_TIME = 2
TX_ACK_TIMEOUT = 10
RX_ACK_TIME = 2
RX_RECEIVE_TIMEOUT = 10
NO_ID = 255

class Packet:
    def __init__(self, create_time, current_id):
        self.create_time = create_time
        self.gateway_time = 0
        self.current_id = current_id
        self.origin_id = current_id

class Node:
    def __init__(self, id, has_sensor):
        self.id = id
        self.tx_time_total = 0
        self.rx_time_total = 0
        self.rx_buffer = []
        self.packet_queue = []
        self.has_sensor = has_sensor

def convert_time_to_sleepydog_time(time: int):
    temp = time
    temp = temp % 250
    temp = temp % 15
    sum = time - temp
    if sum < 15:
        sum = 15
    return sum

def format_time(time):
    milliseconds = time % 1000
    total_seconds = time / 1000
    seconds = total_seconds % 60 
    minutes = total_seconds / 60 % 60
    hours = total_seconds / 60 / 60 % 60
    return '%d:%d:%d' % (hours, minutes, seconds)

def create_packets(env, nodes, id):
    while True:
        nodes[id].packet_queue.append(Packet(env.now, id))
        yield env.timeout(PACKET_PERIOD)

def create_packets_if_not_empty(env, nodes, id):
    while True:
        if len(nodes[id].packet_queue) == 0:
            nodes[id].packet_queue.append(Packet(env.now, id))
        yield env.timeout(1000)

def run(env, nodes, my_id, parent_id, has_sensor = False):

    # Create node
    nodes[my_id] = Node(my_id, has_sensor)
    node = nodes[my_id]
    if has_sensor == True:
        # env.process(create_packets(env, nodes, my_id))
        node.packet_queue.append(Packet(env.now, my_id))

    # Wait for other nodes to be created
    yield env.timeout(10)

    while True:
        if (len(node.packet_queue) > 0) and (parent_id != NO_ID):
            # TX

            # Sleep
            sleep_time = random.expovariate(1.0 / TX_RATE)
            sleep_time = convert_time_to_sleepydog_time(sleep_time)
            yield env.timeout(sleep_time)

            tx_start_timestamp = env.now

            # "Transmit" packet by inserting it into parent's rx_buffer
            nodes[parent_id].rx_buffer.append(node.packet_queue[0])

            # Simulate time passing for transmission and ack timeout 
            yield env.timeout(TX_TRANSMIT_TIME)
            yield env.timeout(TX_ACK_TIMEOUT)

            node.tx_time_total += (env.now - tx_start_timestamp)
        else:
            # RX

            # Sleep
            sleep_time = random.expovariate(1.0 / RX_RATE)
            sleep_time = convert_time_to_sleepydog_time(sleep_time)
            yield env.timeout(sleep_time)

            node.rx_buffer.clear()

            rx_start_timestamp = env.now

            received_from_id = -1

            # Receive
            for i in range(RX_RECEIVE_TIMEOUT):
                if (len(node.rx_buffer) == 1):
                    received_from_id = node.rx_buffer[0].current_id
                    break
                else:
                    yield env.timeout(1)

            if received_from_id != -1:
                packet = node.rx_buffer.pop(0)
                packet.current_id = node.id

                # If current node has no parent, then it is the gateway, mark time for packet
                if parent_id == NO_ID:
                    packet.gateway_time = env.now

                sender = nodes[received_from_id]

                node.packet_queue.append(packet)
                sender.packet_queue.pop(0)

                if sender.has_sensor and len(sender.packet_queue) == 0:
                    sender.packet_queue.append(Packet(env.now, received_from_id))

            yield env.timeout(RX_ACK_TIME)

            node.rx_time_total += (env.now - rx_start_timestamp)

def write_packet_stats(id, gateway_node, file):
    age_sum = 0
    packet_count = 0
    max_age = 0
    min_age = 100000

    for packet in gateway_node.packet_queue:
        if (packet.origin_id == id):
            packet_count = packet_count + 1

            age = packet.gateway_time - packet.create_time
            age_sum += age
            if age > max_age:
                max_age = age
            if age < min_age:
                min_age = age

    if packet_count > 0:
        avg_age = age_sum / packet_count
    else:
        avg_age = 0

    file.write('packets from node %d: ' % id)
    file.write('min age = %s, ' % format_time(min_age))
    file.write('max age = %s, ' % format_time(max_age))
    file.write('avg age = %s' % format_time(avg_age))
    file.write('\n')

def write_awake_stats(id, nodes, file):
    tx_time_percent = nodes[id].tx_time_total / SIM_TIME * 100
    rx_time_percent = nodes[id].rx_time_total / SIM_TIME * 100
    total = tx_time_percent + rx_time_percent

    file.write('stats for node %d: ' % id)
    file.write('tx%% = %f%%, ' % tx_time_percent)
    file.write('rx%% = %f%%, ' % rx_time_percent)
    file.write('total%% = %f%%, ' % total)
    file.write('\n')

def write_age_and_awake_line(age_id, awake_id, nodes, gateway_node, file):
    age_sum = 0
    packet_count = 0

    for packet in gateway_node.packet_queue:
        if (packet.origin_id == age_id):
            packet_count = packet_count + 1

            age = packet.gateway_time - packet.create_time
            age_sum += age

    if packet_count > 0:
        # NOTE: convert to seconds here
        avg_age = age_sum / packet_count / 1000
    else:
        avg_age = 0

    tx_time_percent = nodes[awake_id].tx_time_total / SIM_TIME * 100
    rx_time_percent = nodes[awake_id].rx_time_total / SIM_TIME * 100
    total_time = tx_time_percent + rx_time_percent

    file.write('%d,%d,%d,%f,%f,%f\n' % (TX_RATE, RX_RATE, avg_age, tx_time_percent, rx_time_percent, total_time))

def simulate():
    nodes = {}

    env = simpy.Environment()

    # Setup nodes
    # (env, nodes, id, parent_id, (has_sensor))
    # env.process(run(env, nodes, 35, 129, True))
    # env.process(run(env, nodes, 36, 129, True))
    # env.process(run(env, nodes, 129, 254))
    # env.process(run(env, nodes, 69, 132, True))
    # env.process(run(env, nodes, 130, 132, True))
    # env.process(run(env, nodes, 131, 132, True))
    # env.process(run(env, nodes, 132, 254))
    # env.process(run(env, nodes, 254, NO_ID))

    i = 0
    for x in range(CHILD_COUNT):
        env.process(run(env, nodes, i, 254, True))
        i = i + 1
    env.process(run(env, nodes, 254, NO_ID))

    env.run(until=SIM_TIME)

    # gateway_node = nodes[254]
    gateway_node = nodes[254]

    file = open("results.txt", "a")
    write_age_and_awake_line(0, 0, nodes, gateway_node, file)
    file.close()

    # file = open("results.txt", "a")

    # file.write("\n\nRX_RATE = %d, " % RX_RATE)
    # file.write("TX_RATE = %d" % TX_RATE)
    # file.write('\n')
    # file.write('transmitted %d packets\n' % len(gateway_node.packet_queue))
    # write_packet_stats(35, gateway_node, file)
    # write_packet_stats(36, gateway_node, file)
    # write_packet_stats(69, gateway_node, file)
    # write_packet_stats(130, gateway_node, file)
    # write_packet_stats(131, gateway_node, file)
    # write_packet_stats(130, gateway_node, file)
    # for id in range(255):
    #     if id in nodes:
    #         write_awake_stats(id, nodes, file)
    # file.close()

# simulate()

file = open("results.txt", "a")
file.write('tx_rate,rx_rate,age,tx_time,rx_time,total_time\n')
file.close()

# TX_SLEEP_RATES = np.arange(200, 800, 50)
# RX_SLEEP_RATES = np.arange(200, 800, 50)
# x, y = np.meshgrid(TX_SLEEP_RATES,RX_SLEEP_RATES)
# for col, row in zip(x, y):
#     for tx_ms, rx_ms in zip(col, row):

#         TX_RATE = tx_ms
#         RX_RATE = rx_ms

#         simulate()

COUNTS = [254]
for COUNT in COUNTS:
    CHILD_COUNT = COUNT

    simulate()