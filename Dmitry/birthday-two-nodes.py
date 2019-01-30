import simpy
import random

SIMULATION_TICKS = 6 * 60 * 60 * 1000
TICKS_BETWEEN_PACKETS = 15 * 60 * 1000
TX_SLEEP_RANGE = (2000, 3000)
RX_SLEEP_RANGE = (2000, 3000)

packets_in_alpha_queue = 0
alpha_transmitting_packet = False
beta_awake_ticks = 0
alpha_awake_ticks = 0
alpha_tx_mode_ticks = 0


def int_from_range(range):
    return random.randint(range[0], range[1])


def ticks_to_time(ticks: int):
    total_seconds = ticks / 1000
    milliseconds = ticks % 1000
    seconds = total_seconds % 60 
    minutes = total_seconds / 60 % 60
    hours = total_seconds / 60 / 60 % 60
    return '%d:%d:%d:%d' % (hours, minutes, seconds, milliseconds)


def create_packet_at_alpha(env):
    global packets_in_alpha_queue

    while True:
        packets_in_alpha_queue += 1
        # print('%s> new packet created at ALPHA' % ticks_to_time(env.now))

        yield env.timeout(TICKS_BETWEEN_PACKETS)


def node_alpha(env):
    global alpha_awake_ticks
    global alpha_transmitting_packet
    global alpha_tx_mode_ticks

    packet_transmission_start_time = -1

    while True:
        if packets_in_alpha_queue > 0:
            # There is a packet in queue, Tx mode

            # Sleep for random amount
            sleep_time = int_from_range(TX_SLEEP_RANGE)
            yield env.timeout(sleep_time)

            # print('%s> ALPHA wakes up to transmit' % ticks_to_time(env.now))

            # Try to transmit packet
            alpha_transmitting_packet = True
            yield env.timeout(10)
            alpha_awake_ticks += 10
            alpha_transmitting_packet = False

            # Receive ACK(fake, assume always succeeds)
            yield env.timeout(10)
            alpha_awake_ticks += 10

            alpha_tx_mode_ticks += 10 + 10 + sleep_time
        else:
            # Packet queue is empty, Rx mode(fake, no node to receive from)
            
            # Sleep for random amount
            yield env.timeout(int_from_range(RX_SLEEP_RANGE))
            
            # Try to receive transmission
            yield env.timeout(10)
            alpha_awake_ticks += 10


def node_beta(env):
    global beta_awake_ticks
    global packets_in_alpha_queue

    while True:
        # Sleep for random amount
        yield env.timeout(int_from_range(RX_SLEEP_RANGE))

        # print('%s> BETA wakes up to receive' % ticks_to_time(env.now))

        # Wake up, try to receive transmission
        for i in range(10):
            if alpha_transmitting_packet == True:
                break
            else:
                yield env.timeout(1)
                beta_awake_ticks += 1

        # If caught transmission, stay awake time until transmission is over
        transmission_ticks_recorded = 0
        while alpha_transmitting_packet == True:
            transmission_ticks_recorded += 1
            yield env.timeout(1)
            beta_awake_ticks += 1

        # Successfully received whole transmission if recorded 10 ticks
        if transmission_ticks_recorded == 10:
            packets_in_alpha_queue -= 1
            # print('%s> BETA received packet' % ticks_to_time(env.now))

            # Send ACK(fake, assume always succeeds)
            yield env.timeout(10)
            beta_awake_ticks += 10


env = simpy.Environment()
env.process(create_packet_at_alpha(env))
env.process(node_alpha(env))
env.process(node_beta(env))
env.run(SIMULATION_TICKS)


print('INPUTS')
print('tx range = (%d, %d)' % TX_SLEEP_RANGE)
print('rx range = (%d, %d)' % RX_SLEEP_RANGE)
print('simulation time = %s' % ticks_to_time(SIMULATION_TICKS))
print('time between packets = %s' % ticks_to_time(TICKS_BETWEEN_PACKETS))
print('')
print('OUTPUTS')
packets_created = SIMULATION_TICKS / TICKS_BETWEEN_PACKETS
packets_transmitted = packets_created - packets_in_alpha_queue
print('packets transmitted = %d/%d' % (packets_transmitted, packets_created))
alpha_awake_percentage = alpha_awake_ticks / SIMULATION_TICKS * 100
beta_awake_percentage = beta_awake_ticks / SIMULATION_TICKS * 100
print('alpha awake time = %s = %f%%' % (ticks_to_time(alpha_awake_ticks), alpha_awake_percentage))
print('beta awake time = %s = %f%%' % (ticks_to_time(beta_awake_ticks), beta_awake_percentage))
avg_time_to_transmit_packet = alpha_tx_mode_ticks / packets_transmitted
print('avg time to transmit packet = %s' % ticks_to_time(avg_time_to_transmit_packet))
