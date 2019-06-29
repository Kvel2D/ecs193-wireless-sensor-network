#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 2
#define NUM_SENSORS 6
#define ADDRESS_LENGTH 8

DeviceAddress addresses[NUM_SENSORS];
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int num_sensors = 0;

void inline shift(int index, int size, DeviceAddress directory[NUM_SENSORS]) {
  for (int i = size - 1; i >= index; i--) {
    memcpy(directory[i + 1], directory[i], ADDRESS_LENGTH);
  }
}

bool inline less(DeviceAddress val1, DeviceAddress val2) {
  for (int i = 0; i < ADDRESS_LENGTH; i++) {
    if (val1[i] < val2[i]) return true;
    if (val2[i] < val1[i]) return false;
  }
  return false;
}

void insertion_sort(int size, DeviceAddress directory[NUM_SENSORS],
                    DeviceAddress entry) {
  for (int i = 0; i <= size; i++) {
    if (i == size) {
      memcpy(directory[i], entry, ADDRESS_LENGTH);
    } else if (!less(directory[i], entry)) {
      shift(i, size, directory);
      memcpy(directory[i], entry, ADDRESS_LENGTH);
      break;
    }
  }
}

void create_address_book() {
  num_sensors = 0;
  DeviceAddress new_entry;
  oneWire.reset_search();
  int size = 0;
  while (oneWire.search(new_entry)) {
    if (sensors.validAddress((const uint8_t*)new_entry)) {
      // insert addresses by their magnitude.
      insertion_sort(size, addresses, new_entry);
      size++;
    }
  }
  num_sensors = size;
}

void setup_sensors(void) {
  create_address_book();
  sensors.begin();
}

void read_temperatures(float temperatures[]) {
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures();  // Send the command to get temperatures
  for (int i = 0; i < NUM_SENSORS; i++) {
    temperatures[i] = sensors.getTempC(addresses[i]);
  }
}
