import random

import networkx as nx
import simpy


def generate_message(env, start_node):
    '''

    Args:
        env: SimPy.Environment instance
        start_node: instance of a Node

    Returns: None

    '''
    i = 0
    while True:
        start_node.message_queue.append(i)
        yield env.timeout(1000)
        i += 1


finish_list = []


def send_protocol(env, graph, sender, receiver, message):
    '''

    Args:
        env: SimPy.Environment instance
        graph: NetworkX DiGraph instance
        sender: Node id of the sending node
        receiver: Node id of the receiving node
        message: the message

    Returns: True if the message successfully transmitted, False otherwise

    '''
    if receiver == 2:
        if message not in finish_list:
            finish_list.append(message)
        return True
    node = graph.nodes[receiver]['node']
    if receiver in graph[sender]:  # if this edge exists, then receiver is in RCV_MODE
        node.message_queue.append(message)
        if message in node.dropped_messages:
            node.dropped_messages.remove(message)
        node.message_received_alert.succeed()
        return True  # placeholder for ACK
    else:
        if message not in node.dropped_messages:
            node.dropped_messages.append(message)
    return False


class Node:
    class Mode:
        slp = 0
        rx = 1
        tx = 2

    def __init__(self, env, graph, id, mq_length, senders, receivers):
        self.env = env
        self.graph = graph
        self.id = id
        self.mode = Node.Mode.slp
        self.sleep_ticks = 900
        self.receive_ticks = 15
        self.transmit_ticks = 15
        self.action = env.process(self.run())
        self.message_queue = list()
        self.message_queue_length = mq_length
        self.dropped_messages = list()
        self.senders = senders  # id list of nodes that send to this node
        self.receivers = receivers  # id list of nodes that receive messages from this node

    def run(self):
        while True:
            # sleep mode
            yield self.env.timeout(random.randint(500, 900))
            # receiving mode
            self.mode = Node.Mode.rx
            self.message_received_alert = self.env.event()
            for i in self.senders:
                self.graph.add_edge(i, self.id)  # add these edges to allow transmission
            yield self.env.timeout(
                    self.receive_ticks) | self.message_received_alert  # wait for transmission or end of receive time
            for i in self.senders:
                self.graph.remove_edge(i, self.id)  # end receive mode
            # sending mode
            while self.message_queue:  # NOTE: rn transmission time will actually last at most 9*num_messages ticks
                message = self.message_queue.pop(0)
                sent = {receiver: False for receiver in self.receivers}  # keep track of who successfully received ACK
                for i in range(self.transmit_ticks):
                    for receiver in sent:
                        if send_protocol(self.env, self.graph, self.id, receiver, message):
                            sent[receiver] = True
                    yield self.env.timeout(1)


# def tester():
graph = nx.DiGraph()
env = simpy.Environment()
node = Node(env, graph, 0, 10, [], [1])
env.process(generate_message(env, node))
graph.add_node(node.id, node = node)
node = Node(env, graph, 1, 10, [0], [2])
graph.add_node(node.id, node = node)
env.run(until = 99999)
for node in graph.nodes:
    _node = graph.nodes[node]['node']
    print('{}: {}'.format(node, _node.dropped_messages))
    print(finish_list)

# if __name__ == '__main__':
#     tester()
