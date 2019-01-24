import simpy
import random

# two nodes, simulation runs for N ticks, nodes can wake up each tick with some probability, a collision happens if both nodes wake up in the same tick

simulation_count = 100
simulation_length = 1000
awake_chance = 0.05

awake = [False, False]
collision_count = 0

simulation_with_collision_count = 0
total_collision_count = 0

def node(env, node_i):
    global collision_count

    while True:
        awake[node_i] = (random.random() < awake_chance)

        yield env.timeout(1)

def watch_nodes(env):
    global collision_count

    while True:
        if (awake[0] and awake[1]):
            collision_count += 1

        for i in range(2):
            awake[i] = False

        yield env.timeout(1)


for i in range(simulation_count):
    env = simpy.Environment()

    for i in range(2):
        env.process(node(env, i))
    # NOTE: watch_nodes() must be created after node()'s so that it's run after them
    env.process(watch_nodes(env))

    env.run(until=simulation_length)

    if collision_count > 0:
        simulation_with_collision_count += 1

    total_collision_count += collision_count
    collision_count = 0

print('simulation length = %d\nawake chance = %f\nprobability of at least one collision = %f\naverage collisions per simulation = %f' % (simulation_length, awake_chance, (simulation_with_collision_count / simulation_count), (total_collision_count / simulation_count))) 





