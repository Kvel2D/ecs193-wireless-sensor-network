# ecs193-wireless-sensor-network

## Naming Conventions
- node 0 = alpha = node that transmits
- node 1 = beta = node that receives

## Simulation Standards
- packet transmission = 9 ticks (node 0 to 1)
- packet reception = 9 ticks (node 1 to 0)
- receiver/transmitter wake up at the same time = success
  - receiver can extend its receiving time if transmiter starts in the middle of its reception slot
  - ie. if receiver starts hearing transmitter at tick 5, it will extend extra 4 ticks to listen to the full packet
- immediate transmission/reception = 0 delay in between transmission
- acknowledgement takes 5 ticks and is always successful

## Data to Record
- number of packets total generated vs number of packets successfully transmitted
- sleep time --> calculate ratio of awake time
