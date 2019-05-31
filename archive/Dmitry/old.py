import simpy
import random

SIMULATION_TIME = 6 * 60 * 60 * 1000
TIME_BETWEEN_PACKETS = 15 * 60 * 1000
TX_AD_TIME = 1000
TX_TRANSMIT_TIME = 9
RX_SLEEP_TIME = 900
RX_AWAKE_TIME = 9
ACK_TIME = 5
RX_OFFSET = 700

packets_in_alpha_queue = 0
alpha_advertising = False
alpha_sleep_time = 0
beta_sleep_time = 0
alpha_tx_mode_ticks = 0


def format_time(time: int):
    milliseconds = time % 1000
    total_seconds = time / 1000
    seconds = total_seconds % 60 
    minutes = total_seconds / 60 % 60
    hours = total_seconds / 60 / 60 % 60
    return '%d:%d:%d:%d' % (hours, minutes, seconds, milliseconds)


def create_packet_at_alpha(env):
    global packets_in_alpha_queue

    while True:
        packets_in_alpha_queue += 1
        print('%s> new packet created at ALPHA' % format_time(env.now))

        yield env.timeout(TIME_BETWEEN_PACKETS)


def node_alpha(env):
    global alpha_sleep_time
    global alpha_advertising
    global alpha_tx_mode_ticks

    packet_transmission_start_time = -1

    while True:
        if packets_in_alpha_queue > 0:
            # There is a packet in queue, Tx mode
            tx_cycle_start_time = env.now

            # Try to transmit packet
            alpha_advertising = True
            yield env.timeout(TX_AD_TIME)
            alpha_advertising = False

            yield env.timeout(TX_TRANSMIT_TIME)

            # Receive ACK(fake, assume always succeeds)
            yield env.timeout(ACK_TIME)

            alpha_tx_mode_ticks += env.now - tx_cycle_start_time
        else:
            # Packet queue is empty, Rx mode(fake, no node to receive from)
            
            # Sleep for 900
            sleep_ticks = RX_SLEEP_TIME
            yield env.timeout(sleep_ticks)
            alpha_sleep_time += sleep_ticks

            # Try to receive transmission
            yield env.timeout(RX_AWAKE_TIME)


def node_beta(env):
    global beta_sleep_time
    global packets_in_alpha_queue

    yield env.timeout(RX_OFFSET)

    while True:
        sleep_ticks = RX_SLEEP_TIME
        yield env.timeout(sleep_ticks)
        beta_sleep_time += sleep_ticks

        # print('%s> BETA wakes up to receive' % format_time(env.now))

        # Wake up, try to receive transmission
        received_ad = False
        for i in range(RX_AWAKE_TIME):
            if alpha_advertising == True:
                received_ad = True
                break
            else:
                yield env.timeout(1)

        # If caught transmission, stay awake time until transmission is over
        while alpha_advertising == True:
            yield env.timeout(1)

        yield env.timeout(TX_TRANSMIT_TIME)

        # Successfully received whole transmission if recorded 10 ticks
        if received_ad == True:
            packets_in_alpha_queue -= 1
            print('%s> BETA received packet' % format_time(env.now))

            # Send ACK(fake, assume always succeeds)
            yield env.timeout(ACK_TIME)


env = simpy.Environment()
env.process(create_packet_at_alpha(env))
env.process(node_alpha(env))
env.process(node_beta(env))
env.run(SIMULATION_TIME)


print('INPUTS')
print('simulation time = %s' % format_time(SIMULATION_TIME))
print('time between packets = %s' % format_time(TIME_BETWEEN_PACKETS))
print('')
print('OUTPUTS')
packets_created = SIMULATION_TIME / TIME_BETWEEN_PACKETS
packets_transmitted = packets_created - packets_in_alpha_queue
print('packets transmitted = %d/%d' % (packets_transmitted, packets_created))
alpha_awake_time = SIMULATION_TIME - alpha_sleep_time
alpha_awake_percentage = alpha_awake_time / SIMULATION_TIME * 100
beta_awake_time = SIMULATION_TIME - beta_sleep_time
beta_awake_percentage = beta_awake_time / SIMULATION_TIME * 100
print('alpha awake time = %s = %f%%' % (format_time(alpha_awake_time), alpha_awake_percentage))
print('beta awake time = %s = %f%%' % (format_time(beta_awake_time), beta_awake_percentage))
avg_time_to_transmit_packet = alpha_tx_mode_ticks / packets_transmitted
print('avg time to transmit packet = %s' % format_time(avg_time_to_transmit_packet))
