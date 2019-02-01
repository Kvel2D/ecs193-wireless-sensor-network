import simpy
import types


class Mode():
    receiving = 0
    sending = 1


class SimEnv():
    class Node():
        def __init__(self, id, x, y, z, q_size, slp_protocol, wk_protocol, snd_protocol, rcv_protocol):
            self.id = id
            self.x = x
            self.y = y
            self.z = z
            self.q = []
            self.q_size = q_size
            self.slp_protocol = types.MethodType(slp_protocol, self)
            self.wk_protocol = types.MethodType(wk_protocol, self)
            self.snd_protocol = types.MethodType(snd_protocol, self)
            self.rcv_protocol = types.MethodType(rcv_protocol, self)
            self.on = False
            self.on_for = 0
            self.mode = Mode.receiving
            self.dropped_messages = []

    class Connection():
        def __init__(self, receivers, signal_strengths):
            self.connections = list(
                zip(receivers, signal_strengths))  # id, signal [0,1]

    def __init__(self, nodes):
        self.nodes = nodes
        self.start_node = nodes[0]
        self.end_node = nodes[-1]
        for node_i in nodes:
            node_i.edges = SimEnv.Connection([i.id for i in nodes if i.id != node_i.id], [
                                             1 for i in nodes if i.id != node_i.id]).connections
        # tuple : (node_i, node_i's connections)
        # [(node: id, edge: (sender, connections: [(receiver_1, sig_strength_1),...])...]
        self.env = simpy.Environment()

    def send(self, sender_id, receiver_id, message):
        try:
            send_node = next(i for i in self.nodes if i.id == sender_id)
        except StopIteration:
            return False

        try:
            rcv_node = next(i for i in send_node.edges if i[0] == receiver_id)
        except StopIteration:
            return False

        # in future, implement receive method to put msg in queue with realistic delay and modifiers
        if rcv_node.on and rcv_node.mode == Mode.receiving:
            rcv_node.q.append(message)
        else:
            rcv_node.dropped_messages.append(message)
        return True

    def sleep(self, node, time):
        yield simpy.util.start_delayed(self.env, node.wk_protocol, delay=time)
        pass

    def wake(self):
        pass

    def monitor(self):
        while True:
            for node in self.nodes:
                if not node.sleep:
                    node.on_for = node.on_for + 1
            yield self.env.timeout(1)


def node_send_protocol():
    pass
