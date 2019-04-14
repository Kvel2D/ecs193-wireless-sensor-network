#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define NUM_SENSORS 6
#define ADDRESS_LENGTH 8

DeviceAddress addresses[NUM_SENSORS];

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


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

void insertion_sort(int size, DeviceAddress directory[NUM_SENSORS], DeviceAddress entry) {
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
  Serial.println("START");
  DeviceAddress new_entry;
  oneWire.reset_search();
  int size = 0;
  while (oneWire.search(new_entry)) {
    if (sensors.validAddress((const uint8_t*)new_entry)) {
      for (int i = 0; i < ADDRESS_LENGTH; i++) {
        Serial.print(new_entry[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      insertion_sort(size, addresses, new_entry); // insert addresses by their magnitude.
      size++;
    }
  }
  Serial.println("END");
}

void begin_sensors() {
  create_address_book();
  sensors.begin();
}

void request_temp() {
  sensors.requestTemperatures();
}

void read_temp(float buffer[]) {
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  for (int i = 0; i < NUM_SENSORS; i++) {
    for (int j = 0; j < ADDRESS_LENGTH; j++) {
      Serial.print(addresses[i][j], HEX);
      Serial.print(" ");
    }
    Serial.print(": ");
    float reading = sensors.getTempC(addresses[i]);
    Serial.println(reading);
    buffer[i] = reading;
  }
}
