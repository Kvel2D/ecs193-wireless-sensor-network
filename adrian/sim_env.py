import random
import string

import matplotlib.pyplot as plt
import networkx as nx
import simpy
from hierarchy_pos import hierarchy_pos

colors = ['b', 'r', 'g']  # used to color nodes in animation
animation_frames = []  # add frames in
finish_list = []  # list of messages collected at gateway node

fig = plt.gcf()
plt.axis('off')
pos = None
iteration = 0


def generate_draw_network(graph):
    # nodes = nx.draw_networkx_nodes(graph, pos = pos, node_color = [colors[graph.nodes[node]['node'].mode] for node
    # in graph.nodes])
    # edges = nx.draw_networkx_edges(graph, pos = pos, edge_color = ['g' if transmit and (sender, receiver) == e else
    # 'r' for e in graph.edges])
    # animation_frames.append([nodes, edges, ])
    plt.clf()
    plt.axis('off')
    global iteration
    iteration += 1
    plt.title('Iteration: {}'.format(iteration))
    nx.draw_networkx(graph, pos = pos,
                     node_color = [colors[graph.nodes[node]['node'].mode] for node in graph.nodes],
                     edge_color = ['g' if graph[e[0]][e[1]]['on'] and graph.nodes[e[0]][
                         'node'].sending_to == e[1] else 'r' if graph[e[0]][e[1]]['on'] else 'k' for e in
                                   graph.edges])
    # plt.pause(1)
    plt.show()


# def update_animation(i):
#     repr(animation_frames[i])
#     return animation_frames[i]


def generate_message(env, start_node, ticks_between_packets):
    '''

    Args:
        env: SimPy.Environment instance
        start_node: instance of a Node
        ticks_between_packets: how many ticks in between packet creation?
    Returns: None

    '''
    message_list = [''.join(random.choices(string.ascii_uppercase + string.digits, k = 10)) for _ in
                    range(100)]  # make some arbitrary messages
    i = 0
    while True:
        # start_node.message_queue.append((i, message_list[i]))
        for recv_id in start_node.receiver_lists.keys():
            start_node.receiver_lists[recv_id].append([(i, env.now)])
        yield env.timeout(ticks_between_packets)
        i += 1


def generate_multiple_messages(env, generator_nodes, ticks_between_packets):
    '''
    Generates packets simultaneously in start_nodes every ticks_between_packets
    Args:
        env ():
        generator_nodes ():
        ticks_between_packets ():

    Returns: None
    '''
    i = 0
    while True:
        for node in generator_nodes:
            for recv_id in node.receiver_lists.keys():
                node.receiver_lists[recv_id].append([(i, env.now)] + [(node.id, env.now)])
            i += 1
        yield env.timeout(ticks_between_packets)


def send_protocol(sender, receiver_id, message):
    '''

    Args:
        sender: Node instance of sending node
        receiver_id: Node id of the receiving node
        message: the message

    Returns: True if the message successfully transmitted, False otherwise

    '''
    global finish_list
    if sender.is_gateway:
        finish_list.append(message + [(sender.id, sender.env.now)])
        generate_draw_network(sender.graph)
        sender.message_transmitted_alert.succeed()
        return sender.message_transmitted_alert
    node = sender.graph.nodes[receiver_id]['node']
    if sender.graph[sender.id][receiver_id]['on'] is True:  # receiver is in RCV_MODE
        if node.is_gateway:  # if the recipient node is gateway, just append to finish_list
            finish_list.append(message + [(receiver_id, sender.env.now)])
        else:
            for recv_id in node.receiver_lists.keys():
                node.receiver_lists[recv_id].append(message + [(receiver_id, sender.env.now)])
        node.message_received_alert.succeed()
        sender.message_transmitted_alert.succeed()  # placeholder for ACK
        generate_draw_network(sender.graph)
    return sender.message_transmitted_alert


def receive_protocol(receiver):
    global finish_list
    for sender_id, _ in receiver.graph.in_edges(receiver.id):
        node = receiver.graph.nodes[sender_id]['node']
        if node.sending_to == receiver.id:  # TODO: implement listening to different channels
            if receiver.is_gateway:
                finish_list.append(node.transmitting_message + [(receiver.id, receiver.env.now)])
            else:
                for recv_id in receiver.receiver_lists.keys():
                    receiver.receiver_lists[recv_id].append(
                            node.transmitting_message + [(receiver.id, receiver.env.now)])
            node.message_transmitted_alert.succeed()
            receiver.message_received_alert.succeed()
            generate_draw_network(receiver.graph)
            break
    return receiver.message_received_alert


class Node:
    class Mode:
        slp = 0
        rx = 1
        tx = 2

    def __init__(self, env, graph, _id, mq_length, senders, receivers, rx_sleep_ticks = 900, tx_sleep_ticks = 900,
                 receive_ticks = 9,
                 transmit_ticks = 9, is_gateway = False, is_generator = False):
        self.env = env
        self.graph = graph
        self.id = _id
        self.mode = Node.Mode.slp
        self.rx_sleep_ticks = rx_sleep_ticks
        self.tx_sleep_ticks = tx_sleep_ticks
        self.receive_ticks = receive_ticks
        self.transmit_ticks = transmit_ticks
        self.is_gateway = is_gateway
        self.is_generator = is_generator
        self.action = env.process(self.run())
        self.message_transmitted_alert = None
        self.message_received_alert = None
        if mq_length >= 0:
            self.message_queue_length = mq_length
        else:
            self.message_queue_length = float('inf')
        self.senders = senders  # id list of nodes that send to this node
        self.sending_to = None
        self.transmitting_message = None
        self.receivers = receivers  # id list of nodes that receive messages from this node
        self.receiver_lists = {rec_id: [] for rec_id in receivers}
        self.sleep_for = 0
        self.transmit_for = 0
        self.receive_for = 0

    def run(self):
        while True:
            if self.is_gateway:  # gateway will only receive
                # slp mode
                self.mode = Node.Mode.slp
                last = self.env.now
                yield self.env.timeout(random.expovariate(1 / self.tx_sleep_ticks))
                self.increment_timer(self.env.now - last)
                # rx mode
                self.mode = Node.Mode.rx
                for e in self.graph.in_edges(self.id):
                    self.graph[e[0]][e[1]]['on'] = True  # allow transmission
                last = self.env.now
                self.message_received_alert = self.env.event()
                try:
                    if receive_protocol(self).ok:
                        pass
                except AttributeError:
                    yield self.message_received_alert | self.env.timeout(self.receive_ticks)
                self.message_received_alert = None
                for e in self.graph.in_edges(self.id):
                    self.graph[e[0]][e[1]]['on'] = False  # disallow transmission
                self.increment_timer(self.env.now - last)
                # print('TESTING')
                # for k, v in self.receiver_lists.items():
                #     print('{}, {}'.format(k, v))
                # global finish_list
                # finish_list.append(self.receiver_lists[self.id].pop(0))
            else:
                if any(self.receiver_lists.values()):  # if the node has any messages to transmit
                    # slp mode
                    self.mode = Node.Mode.slp
                    last = self.env.now
                    yield self.env.timeout(random.expovariate(1 / self.tx_sleep_ticks))
                    self.increment_timer(self.env.now - last)
                    # tx mode
                    self.mode = Node.Mode.tx
                    last = self.env.now
                    send_time = 0
                    active_lists = {receiver: queue for receiver, queue in self.receiver_lists.items() if queue}
                    for receiver, queue in active_lists.items():
                        self.sending_to = receiver
                        message = queue[0]
                        self.transmitting_message = message
                        self.message_transmitted_alert = self.env.event()
                        if not send_protocol(self, receiver, self.transmitting_message).processed:
                            yield self.message_transmitted_alert | self.env.timeout(self.receive_ticks - send_time)
                        send_time = self.env.now - last
                        try:
                            if self.message_transmitted_alert.ok:
                                self.receiver_lists[receiver].pop(0)
                        except AttributeError:
                            pass
                        if not send_time < self.transmit_ticks:
                            break
                    self.sending_to = None
                    self.message_transmitted_alert = None
                    self.increment_timer(self.env.now - last)
                else:  # enter receiving mode
                    # slp mode
                    self.mode = Node.Mode.slp
                    last = self.env.now
                    yield self.env.timeout(random.expovariate(1 / self.tx_sleep_ticks))
                    self.increment_timer(self.env.now - last)
                    # rx mode
                    self.mode = Node.Mode.rx
                    for e in self.graph.in_edges(self.id):
                        self.graph[e[0]][e[1]]['on'] = True  # allow transmission
                    last = self.env.now
                    self.message_received_alert = self.env.event()
                    try:
                        if receive_protocol(self).ok:
                            pass
                    except AttributeError:
                        yield self.message_received_alert | self.env.timeout(self.receive_ticks)
                    self.message_received_alert = None
                    for e in self.graph.in_edges(self.id):
                        self.graph[e[0]][e[1]]['on'] = False  # allow transmission
                    self.increment_timer(self.env.now - last)

    def increment_timer(self, time):
        if self.mode == Node.Mode.tx:
            self.transmit_for += time
        elif self.mode == Node.Mode.rx:
            self.receive_for += time
        else:
            self.sleep_for += time


def tester(num_nodes = 4, mq_length = 10, rx_sleep_ticks = 1000, tx_sleep_ticks = 1000,
           ticks_between_packets = 15 * 60 * 1000, run_until = 12 * 60 * 60 * 1000):
    '''

    Args:
        num_nodes ():
        mq_length ():
        rx_sleep_ticks ():
        tx_sleep_ticks ():
        ticks_between_packets ():
        run_until ():

    Returns:

    '''
    graph = nx.DiGraph()
    env = simpy.Environment()
    for i in range(num_nodes):
        if i == 0:
            node = Node(env, graph, i, mq_length, [], [i + 1], rx_sleep_ticks = rx_sleep_ticks,
                        tx_sleep_ticks = tx_sleep_ticks)
            env.process(generate_message(env, node, ticks_between_packets))
        elif i == num_nodes - 1:
            node = Node(env, graph, i, mq_length, [i - 1], [i + 1], rx_sleep_ticks = rx_sleep_ticks,
                        tx_sleep_ticks = tx_sleep_ticks, is_gateway = True)
        else:
            node = Node(env, graph, i, mq_length, [i - 1], [i + 1], rx_sleep_ticks = rx_sleep_ticks,
                        tx_sleep_ticks = tx_sleep_ticks)
        graph.add_node(node.id, node = node)
    env.run(until = run_until)
    with open('results.csv', 'a') as file:
        file.write('{},{},'.format(tx_sleep_ticks, rx_sleep_ticks))
        tx_time_percent = graph.nodes[1]['node'].transmit_for / run_until * 100
        rx_time_percent = graph.nodes[1]['node'].receive_for / run_until * 100
        file.write('{},{},'.format(tx_time_percent, rx_time_percent))
        total_packet_create_to_gateway_time = 0
        for l in finish_list:
            total_packet_create_to_gateway_time += l[-1][1] - l[1][1]
        avg_packet_create_to_gateway_time = total_packet_create_to_gateway_time / len(finish_list)
        avg_packet_node_to_node_time = avg_packet_create_to_gateway_time / (num_nodes - 1)
        file.write('{}\n'.format(avg_packet_node_to_node_time))
    print('RX_SLEEP_RATE: {}, TX_SLEEP_RATE{}'.format(rx_sleep_ticks, tx_sleep_ticks))
    for node in graph.nodes:
        _node = graph.nodes[node]['node']
        print('{}: transmitted for: {}, received for: {}, slept for: {}'.format(node, _node.transmit_for,
                                                                                _node.receive_for, _node.sleep_for))
        print('\tUnable to transmit messages:')
        for recv_id in _node.receiver_lists.keys():
            print('\t\tTo {}: {}'.format(recv_id, _node.receiver_lists[recv_id]))
    print('\nSuccessfully passed messages:\n\t\t{}'.format(finish_list))


def tree_tester(branch_factor = 2, height = 3, mq_length = 10, rx_sleep_ticks = 1000, tx_sleep_ticks = 1000,
                ticks_between_packets = 15 * 60 * 1000, run_until = 12 * 60 * 60 * 1000):
    env = simpy.Environment()
    graph = nx.balanced_tree(branch_factor, height, nx.DiGraph)
    global pos
    pos = hierarchy_pos(graph, root = 0)
    graph = graph.reverse()  # we want the edges to all point to root
    generator_nodes = []
    for i in graph.nodes:
        senders = [j for j, _ in graph.in_edges(i)]
        receivers = [j for _, j in graph.out_edges(i)]
        node = Node(env, graph, i, mq_length, senders, receivers, rx_sleep_ticks, tx_sleep_ticks)
        if not receivers:  # this is a gateway
            node.is_gateway = True
        elif not senders:  # this is a start_node
            node.is_generator = True
            generator_nodes.append(node)
        graph.nodes[i]['node'] = node
    for i in graph.nodes:  # assign initial transmission edges
        for e in graph.in_edges(i):
            graph[e[0]][e[1]]['on'] = False  # allow transmission
    # for i in graph.nodes:
    #     print('{}: {}, {}'.format(graph.nodes[i]['node'].id, graph.nodes[i]['node'].receivers,
    #                               graph.nodes[i]['node'].senders))
    # print()
    env.process(generate_multiple_messages(env, generator_nodes, ticks_between_packets))
    env.run(until = run_until)
    for node in graph.nodes:
        _node = graph.nodes[node]['node']
        print('{}: transmitted for: {}, received for: {}, slept for: {}'.format(node, _node.transmit_for,
                                                                                _node.receive_for, _node.sleep_for))
        print('\tUnable to transmit messages:')
        for recv_id in _node.receiver_lists.keys():
            print('\t\tTo {}: {}'.format(recv_id, _node.receiver_lists[recv_id]))
    print('\nSuccessfully passed messages:')
    for l in finish_list:
        print('\t\tMessage {} generated at: {}'.format(l[0][0], l[0][1]))
        print('\t\t\t(Node #, Time): {}'.format(l[1:]))

    # anim = animation.ArtistAnimation(fig, animation_frames, interval = 50)
    # anim.save('sim.html', writer = 'html', savefig_kwargs = {
    #         'facecolor': 'white'
    # }, fps = 1)


if __name__ == '__main__':
    sleep_rates = [100, 200, 400, 800, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 6000, 7000, 8000]
    tree_tester(branch_factor = 2, run_until = 1000 * 60 * 60)
    # for rx_rate_i in range(len(sleep_rates)):
    #     for tx_rate_i in range(len(sleep_rates)):
    #         TX_SLEEP_RATE = sleep_rates[tx_rate_i]
    #         RX_SLEEP_RATE = sleep_rates[rx_rate_i]
    #         tester(rx_sleep_ticks = RX_SLEEP_RATE, tx_sleep_ticks = TX_SLEEP_RATE)
